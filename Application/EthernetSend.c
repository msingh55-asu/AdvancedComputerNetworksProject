#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

#define MY_DEST_MAC0	0x02
#define MY_DEST_MAC1	0x4a
#define MY_DEST_MAC2	0xb7
#define MY_DEST_MAC3	0x1e
#define MY_DEST_MAC4	0xcb
#define MY_DEST_MAC5	0x0f

#define DEFAULT_IF	"eth1"
#define BUF_SIZ		1024

int main(int argc, char *argv[])
{
	int sockfd, sockfd_server, connfd_server, i;
	struct ifreq if_idx;
	struct ifreq if_mac;
	int tx_len = 0;
	char sendbuf[BUF_SIZ];
	char recvbuf_server[BUF_SIZ];
	char recvbuf[BUF_SIZ];
	struct ether_header *eh = (struct ether_header *) sendbuf;
	struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
	struct sockaddr_ll socket_address;
	struct sockaddr_in serv_addr;
	char ifName[IFNAMSIZ];
	
	sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
    	memset(&serv_addr, '0', sizeof(serv_addr));
    	memset(recvbuf_server, '0', sizeof(recvbuf_server)); 

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	serv_addr.sin_port = htons(9999); 

    	bind(sockfd_server, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

   

	/* Get interface name */
	strcpy(ifName, DEFAULT_IF);

	/* Open RAW socket to send on */
	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
	    perror("socket");
	}

	/* Get the index of the interface to send on */
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	/* Get the MAC address of the interface to send on */
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	/* Construct the Ethernet header */
	memset(sendbuf, 0, BUF_SIZ);
	/* Ethernet header */
	eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh->ether_dhost[0] = MY_DEST_MAC0;
	eh->ether_dhost[1] = MY_DEST_MAC1;
	eh->ether_dhost[2] = MY_DEST_MAC2;
	eh->ether_dhost[3] = MY_DEST_MAC3;
	eh->ether_dhost[4] = MY_DEST_MAC4;
	eh->ether_dhost[5] = MY_DEST_MAC5;
	/* Ethertype field */
	eh->ether_type = htons(ETH_P_IPV6);
	tx_len += sizeof(struct ether_header);

	/* Index of the network device */
        socket_address.sll_ifindex = if_idx.ifr_ifindex;
        /* Address length*/
        socket_address.sll_halen = ETH_ALEN;
        /* Destination MAC */
        socket_address.sll_addr[0] = MY_DEST_MAC0;
        socket_address.sll_addr[1] = MY_DEST_MAC1;
        socket_address.sll_addr[2] = MY_DEST_MAC2;
        socket_address.sll_addr[3] = MY_DEST_MAC3;
        socket_address.sll_addr[4] = MY_DEST_MAC4;
        socket_address.sll_addr[5] = MY_DEST_MAC5;

	printf("Destination MAC: %x:%x:%x:%x:%x:%x\n", eh->ether_dhost[0],eh->ether_dhost[1],
		eh->ether_dhost[2],eh->ether_dhost[3],eh->ether_dhost[4],eh->ether_dhost[5]);
	printf("Source MAC: %x:%x:%x:%x:%x:%x\n", eh->ether_shost[0],eh->ether_shost[1],
                eh->ether_shost[2],eh->ether_shost[3],eh->ether_shost[4],eh->ether_shost[5]);
	
	/* Packet data */
	unsigned char sendData[] ={0x60,0x00,0x00,0x00,0x00,0x67,0x04,0x7f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x67,0x04,0xd2,0x00,0x00,0x7f,0x11,0x20,0x9c,0x0a,0x0a,0x01,0x01,0x0a,0x0a,0x01,0x04,0x00,0x00,0x22,0xb8,0x00,0x53,0xdd,0xb5}; 
	//const char payload[] = "<Application><ID>1</ID><Type>Periodic</Type><Value>50</Value></Application>";
	printf("Size of IP Data is %ld\n", sizeof(sendData));
        memcpy(sendbuf + tx_len, sendData,sizeof(sendData));
        tx_len = tx_len + sizeof(sendData);
	
	listen(sockfd_server, 10);
	while(1)
	{
		printf("Receiving from client...\n");
		connfd_server = accept(sockfd_server, (struct sockaddr*)NULL, NULL);

		/*****************
		while(1)
		{
			int read = recv(connfd_server, recvbuf, BUF_SIZ, 0);
			strcpy(recvbuf_server, recvbuf);
			if (!read) 
				break; // done reading
      			if (read < 0) 
				perror("Client read failed\n");
		}
		*********************************/
		int read_size;
		while( (read_size = recv(connfd_server , recvbuf , BUF_SIZ , 0)) > 0 )
    		{
			strcpy(recvbuf_server, recvbuf);
    		}
     
    		if(read_size == 0)
    		{
        		puts("Client disconnected");
        		fflush(stdout);
    		}
    		else if(read_size == -1)
    		{
        		perror("recv failed");
    		}

		//char * payload = argv[1];
		printf("Payload:%s\n",  recvbuf_server);
		/*sendbuf[tx_len++] = 0xde;
		sendbuf[tx_len++] = 0xad;
		sendbuf[tx_len++] = 0xbe;
		sendbuf[tx_len++] = 0xef;
		*/
		if(strcmp(recvbuf_server,""))
		{
			printf("Buffer is received\n");
			memcpy(sendbuf + tx_len , recvbuf_server, strlen(recvbuf_server));
			tx_len = tx_len + sizeof(recvbuf_server);

			printf("\tData:");
			for (i=0; i<tx_len; i++) 
				printf("%02x:", sendbuf[i]);
			printf("\n");

			/* Send packet */
			if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
	    			printf("Send failed\n");
			tx_len = tx_len - sizeof(recvbuf_server);
		}
		memset(recvbuf_server,0,BUF_SIZ);
	}
	
	return 0;
}
