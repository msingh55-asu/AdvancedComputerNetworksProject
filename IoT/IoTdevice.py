import socket
import sys
import re
from random import randint


dict_app_id = {}
arr_app_id = [0,0,0]

print "IoT Device Server Initializing"
filename = "File_Request.txt"

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('localhost', 9998)

def SendTemp(ID):
        TempK = randint(203, 343)
        TempC = 273 - TempK
        ID = "<ID>" + app_id + "</ID>"
        Type = "<Type>Temperature</Type>"
        TempValue = "<Value>" + TempC + "</Value>"
        if app_id != 0:
                Message = "<IoTdevice>" + ID + Type + TempValue + "</IoTdevice>"
                print "Message to be sent:" , Message

        sock.connect(server_address)

def routine():
	for id in arr_app_id
		if id not 0
	##Code Here	
def process(new_line)
	if new_line[0:13] is "<Application>" and new_line[-14:] is "</Application>":
		print "Message from Application"
		StripApp = re.split('<Application>|</Application>', new_line)
		StripID = re.split('<ID>|</ID>', StripApp)
		appID = StripID[1]
		StripType = re.split('<Type>|</Type>', StripApp)
		reqType = StripType[1]
		if reqType is "OneTime":
			reqValue = 0
		else:
			StripValue = re.split('<Value>|</Value>', StripApp)
			reqValue = StripValue[1]
		arr_app_id[appID] = reqValue
	else:
		print "Invalid Message"
	routine()
	

print >>sys.stderr, 'starting up on %s port %s' % server_address

while True:
	if os.path.getsize(filename):
		filep = open(filename, 'r+')
        	line = filep.readline()
		print "Read Line: %s" % (line)
		filep.seek(0)
		filep.truncate()
        	filep.close()
	else:
		line = ""
	if line not ""
		process(line)
	else:
		routine()
