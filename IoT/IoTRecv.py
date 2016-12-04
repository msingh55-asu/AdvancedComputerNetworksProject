import socket
import sys
import os

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
filename = "File_Request.txt"


# Bind the socket to the address given on the command line
server_address = ('0.0.0.0', 9999)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
sock.listen(1)
recvd_data = ""
while True:
    print "waiting for a connection"
    connection, client_address = sock.accept()
    try:
        print "connection from ", client_address

        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(16)
	    recvd_data = recvd_data + data
            print  "Received Data: ",  data
            if data:
                print "Receiving Data"
            else:
                print "No more data from", client_address
                break
            
    finally:
	print "Data Received: ",recvd_data
        
	filep = open(filename, 'w')
        while os.path.getsize(filename):
		filep.write(recvd_data)
		filep.write("\n")
	filep.close()

	recvd_data = ""
	connection.close()
