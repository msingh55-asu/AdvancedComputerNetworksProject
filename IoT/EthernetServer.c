#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netdb.h>


#define DEST_MAC0	0x02
#define DEST_MAC1	0x4a
#define DEST_MAC2	0xb7
#define DEST_MAC3	0x1e
#define DEST_MAC4	0xcb
#define DEST_MAC5	0x0f

//#define ETHER_TYPE	0x08DD
//#define ETHER_TYPE    0x0800
#define ETHER_TYPE	ETH_P_ALL

#define DEFAULT_IF	"eth1"
#define BUF_SIZ		1024

int main(int argc, char *argv[])
{
	char sender[INET6_ADDRSTRLEN];
	int sockfd, sockfd_client, ret, i;
	int sockopt;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct ifreq if_ip;	/* get ip addr */
	struct sockaddr_storage their_addr;
	uint8_t buf[BUF_SIZ];
	char ifName[IFNAMSIZ];
	struct sockaddr_in serv_addr_client; 
	char * payload;

	
	/* Get interface name */
	strcpy(ifName, DEFAULT_IF);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;
	struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
	struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	memset(&if_ip, 0, sizeof(struct ifreq));
	
	printf("Create Etehrnet socket\n");
	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");	
		return -1;
	}
	
	printf("Creatin promiscuous settings\n");
	/* Set interface to promiscuous mode - do we need to do this every time? */
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	
	printf("Creating socket for application\n");
	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	
	printf("Binding driver for application\n");
	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

        serv_addr_client.sin_family = AF_INET;
        serv_addr_client.sin_port = htons(9999);
	serv_addr_client.sin_addr.s_addr = inet_addr("127.0.0.1");	
	memset(serv_addr_client.sin_zero, '\0', sizeof serv_addr_client.sin_zero);

	printf("listener: Waiting to recvfrom...\n");
	while(1)
	{
		memset(buf, 0, BUF_SIZ);
		numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
		if (eh->ether_dhost[0] == DEST_MAC0 && eh->ether_dhost[1] == DEST_MAC1 && eh->ether_dhost[2] == DEST_MAC2 && eh->ether_dhost[3] == 
				DEST_MAC3 && eh->ether_dhost[4] == DEST_MAC4 && eh->ether_dhost[5] == DEST_MAC5) 
		{
			printf("Packet received\n");
			payload = buf + 82;
        		printf("Payload is %s\n", payload );
		}
		else
			continue;
		
		printf("Creating client for application\n");
        	if((sockfd_client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        	{
        	        printf("\n Error : Could not create socket \n");
                	return 1;
        	}
		
		printf("Connect to application socket\n");
        	if(connect(sockfd_client, (struct sockaddr *)&serv_addr_client, sizeof(serv_addr_client)) < 0)
        	{
               		printf("\n Error : Connect Failed \n");
               		return 1;
        	}
		if (write(sockfd_client, payload, strlen(payload)) < 0)
         		printf("Cannot write on python packet on Python socket\n");
		printf("Data sent to Python IoT Application\n");
		close(sockfd_client);
		printf("listener: Waiting to recvfrom...\n");
	} 
	
	close(sockfd);
	return 0;
}
