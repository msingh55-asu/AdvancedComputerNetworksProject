import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to the address given on the command line
server_name = sys.argv[1]
server_address = ('0.0.0.0', 9999)
print >>sys.stderr, 'starting up on %s port %s' % server_address
sock.bind(server_address)
sock.listen(1)
recvd_data = ""
while True:
    print >>sys.stderr, 'waiting for a connection'
    connection, client_address = sock.accept()
    try:
        print >>sys.stderr, 'connection from', client_address

        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(16)
	    recvd_data = recvd_data + data
            print >>sys.stderr, 'received "%s"' % data
            if data:
                print >>sys.stderr, 'Receiving Data'
            else:
                print >>sys.stderr, 'no more data from', client_address
                break
            
    finally:
	print "Data Received: ",recvd_data
        # Clean up the connection
        connection.close()


