# Copyright 2012 James McCauley
#
# This file is part of POX.
#
# POX is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# POX is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with POX.  If not, see <http://www.gnu.org/licenses/>.

"""
This component is for use with the OpenFlow tutorial.

It acts as a simple hub, but can be modified to act like an L2
learning switch.

It's quite similar to the one for NOX.  Credit where credit due. :)
"""

from pox.core import core
import pox.openflow.libopenflow_01 as of

from pox.lib.packet.ethernet import ethernet, ETHER_BROADCAST
from pox.lib.packet.ipv4 import ipv4
from pox.lib.packet.ipv6 import ipv6
import pox.lib.packet as pkt

log = core.getLogger()



class Tutorial (object):
  """
  A Tutorial object is created for each switch that connects.
  A Connection object for that switch is passed to the __init__ function.
  """
  def __init__ (self, connection):
    # Keep track of the connection to the switch so that we can
    # send it messages!
    
    self.connection = connection

    # This binds our PacketIn event listener
    connection.addListeners(self)

    # Use this table to keep track of which ethernet address is on
    # which switch port (keys are MACs, values are ports).
    self.mac_to_port = {}
    self.mac_to_app = {}

  def resend_packet (self, packet_in, out_port):
    """
    Instructs the switch to resend a packet that it had sent to us.
    "packet_in" is the ofp_packet_in object the switch had sent to the
    controller due to a table-miss.
    """
    msg = of.ofp_packet_out()
    msg.data = packet_in

    # Add an action to send to the specified port
    action = of.ofp_action_output(port = out_port)
    msg.actions.append(action)

    # Send message to switch
    self.connection.send(msg)

  def act_like_hub (self, packet, packet_in):
    self.resend_packet(packet_in, of.OFPP_ALL)

  def act_like_switch (self, packet, packet_in):

    if packet.src not in self.mac_to_port:
	print "Learning that " + str(packet.src) + " is attached at port " + str(packet_in.in_port)
	self.mac_to_port[packet.src] = packet_in.in_port
   
    if packet.dst in self.mac_to_port:
        print str(packet.dst) + " destination known. only send message to it"
        #self.resend_packet(packet_in, self.mac_to_port[packet.dst])      

        #log.debug("Installing flow...")

    else:
      	print str(packet.dst) + " Message not known, not in mac_to_port"
    
    if packet.type == ethernet.IPV6_TYPE:
        print "IPv6 TYPE Address IPv6 IPv6 IPv6"
        ipv4_packet = packet.find("ipv6")
        src_ip = ipv4_packet.srcip
        print "Source IP is: ", src_ip
    	if src_ip == "::FF:0":
		print "Message received from IoT Device"
        	if 255 not in self.mac_to_app:
        		print "Message received from IoT"
                	self.mac_to_app[255] = packet.src
                	print "Value of MAC_TO_APP", self.mac_to_app
        	print "Registering IoT, IoT has MAC: ", self.mac_to_app[255]
      	#self.resend_packet(packet_in, of.OFPP_ALL)
    if 255 not in self.mac_to_app:
	print "IOT device not registered, register Iot device first"
    	return

    if packet.type == ethernet.IPV6_TYPE:
        print "IPv6 TYPE Address IPv6 IPv6 IPv6"
        ipv4_packet = packet.find("ipv6")
        src_ip = ipv4_packet.srcip
        print "Source IP is: ", src_ip
        if src_ip == "::1:0":
                print "Message received from App1"
                if 1 not in self.mac_to_app:
                        print "Message received from App1"
                        self.mac_to_app[1] = packet.src
                        print "Value of MAC_TO_APP", self.mac_to_app
                #print packet
		print "Sending to MAC: ", self.mac_to_app[255]
		#packet.type = 0x8600
		#packet.srcip = "10.10.1.1"
		#packet.dstip = "10.10.1.4"
		#print "NEW Source IP", packet.srcip
		#print "NEW Destination IP", packet.dstip	
		#print "New Packet type", packet.type
		self.resend_packet(packet_in, self.mac_to_port[self.mac_to_app[255]])
        elif src_ip == "::2:0":
                print "Message received from App2"
                if 2 not in self.mac_to_app:
                        print "Message received from App2"
                        self.mac_to_app[2] = packet.src
                        print "Value of MAC_TO_APP", self.mac_to_app
                print "Sending to MAC: ", self.mac_to_app[255]
		self.resend_packet(packet_in, self.mac_to_port[self.mac_to_app[255]])
        elif src_ip == "::3:0":
                print "Message received from App3"
		MessageFrom = "Apps"
                if 3 not in self.mac_to_app:
                        print "Message received from App3"
                        self.mac_to_app[3] = packet.src
                        print "Value of MAC_TO_APP", self.mac_to_app
		print "Sending to MAC: ", self.mac_to_app[255]
		self.resend_packet(packet_in, self.mac_to_port[self.mac_to_app[255]])
	elif src_ip == "::1":
                print "Message received from IoT device for App1"
                if 1 not in self.mac_to_app:
                        print "Message to be sent to App1"
                        self.mac_to_app[1] = packet.src
                        print "Value of MAC_TO_APP", self.mac_to_app
                print "Sending to MAC: ", self.mac_to_app[1]
                self.resend_packet(packet_in, self.mac_to_port[self.mac_to_app[1]])
        elif src_ip == "::2":
                print "Message received from IoT device for App2"
                if 2 not in self.mac_to_app:
                        print "Message to be sent to App2"
                        self.mac_to_app[2] = packet.src
                        print "Value of MAC_TO_APP", self.mac_to_app
                print "Sending to MAC: ", self.mac_to_app[2]
                self.resend_packet(packet_in, self.mac_to_port[self.mac_to_app[3]])
        elif src_ip == "::3":
                print "Message received IoT device for from App3"
                if 3 not in self.mac_to_app:
                        print "Message to be sent to App3"
                        self.mac_to_app[3] = packet.src
                        print "Value of MAC_TO_APP", self.mac_to_app
                print "Sending to MAC: ", self.mac_to_app[3]
                self.resend_packet(packet_in, self.mac_to_port[self.mac_to_app[3]])
        else:
                print "Message received from Unknown Application"
    else:
        if packet.type == ethernet.IP_TYPE:
                print "IPv4 TYPE Address IPv4 IPv4 IPv4"
                ipv4_packet = packet.find("ipv4")
                src_ip = ipv4_packet.srcip
                print "Source IP is: ", src_ip
		if packet.dst in self.mac_to_port:
                        print str(packet.dst) + " destination known. only send message to it"
                        self.resend_packet(packet_in, self.mac_to_port[packet.dst])
                else:
                        print "Sending to All ports"
                        self.resend_packet(packet_in, of.OFPP_ALL)
        else:
                print "Not an IP packet"
		if packet.dst in self.mac_to_port:
			print str(packet.dst) + " destination known. only send message to it"
			self.resend_packet(packet_in, self.mac_to_port[packet.dst])
   		else:
			print "Sending to All ports"
			self.resend_packet(packet_in, of.OFPP_ALL)
 
    log.debug("Installing flow...")
 


  def _handle_PacketIn (self, event):

    print "***************New Message received************"

    packet = event.parsed # This is the parsed packet data.
    if not packet.parsed:
      log.warning("Ignoring incomplete packet")
      return

    packet_in = event.ofp # The actual ofp_packet_in message.
    
     
    if packet.type == 0x86DD:
        print "IPv6 Packet"
    else:
        print "IPv4 packet"
     
    # Comment out the following line and uncomment the one after
    # when starting the exercise.
    #self.act_like_hub(packet, packet_in)
    self.act_like_switch(packet, packet_in)
	


def launch ():
  """
  Starts the component
  """
  def start_switch (event):
    log.debug("Controlling %s" % (event.connection,))
    Tutorial(event.connection)
  core.openflow.addListenerByName("ConnectionUp", start_switch)
