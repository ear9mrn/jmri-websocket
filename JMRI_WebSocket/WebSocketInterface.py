#   Created by Peter Nevill 2021
#   This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

import java, java.beans
from org.apache.log4j import Logger
import jmri

# Websocket is not park of jython so we have to use a local module
from sys import path
import os, threading, socket, time
path.append(os.getcwd() + '/jython/JMRI_WebSocket/simplewebsocketserver')
from simplewebsocketserver import WebSocket, SimpleWebSocketServer


# The ip address of this machine, does not have to be set as it will try and determin it automatically. 
# Don't use loopback (127.0.0.1)
JRMI_SERVER_ADDRESS =  None  

# The port the websocket will listens on. All client need to use this port
JRMI_SERVER_PORT    =  8000

JMRI_Websocket_log = Logger.getLogger("jmri.jmrit.jython.exec.JMRI_Websocket")
clients = {}            #containes a list of connected websocket clients

# define the listener class for Sensors
class JMRI_Websocket_Sensor_Listener(java.beans.PropertyChangeListener):

    def propertyChange(self, event):
        sensor = event.getSource()
        sensorName = sensor.getDisplayName(jmri.NamedBean.DisplayOptions.USERNAME_SYSTEMNAME)
        JMRI_Websocket_log.info("Sensor=" + sensorName + " property=" + event.propertyName + "]: oldValue=" + str(event.oldValue) + " newValue=" )
        return

# define the listener class for Turnouts
class JMRI_Websocket_Turnout_Listener(java.beans.PropertyChangeListener):

    def propertyChange(self, event):
        #JMRI_Websocket_log.info(event)
        turnout = event.getSource()
        turnoutName = turnout.getDisplayName(jmri.NamedBean.DisplayOptions.USERNAME_SYSTEMNAME)

        # only respond to one of the two events triggered
        if event.propertyName == "KnownState":
            #JMRI_Websocket_log.info("Turnout=" + turnoutName + " property=" + \
	    #		event.propertyName + "]: oldValue=" + str(event.oldValue) + " newValue=" + str(event.newValue) )

            client_id,  pin = get_clienid_pin(turnoutName)
            state = 0
            
            # there are four states, will just assume everything other than state 4 is closed!
            # I need to understand a bit bettter the reasons for the 4 different states!
            if event.newValue == 4:
                state = 1
                
            #if we have a websocket listener that has the same id send them the change state command.
            if client_id in clients.keys():
                    # the packet that is sent using this strucuture. <clint id, from ip address,to ip address, turnout/senor, pin, state>
                    message = client_id + "," + JRMI_SERVER_ADDRESS + "," + str(clients[client_id].address[0]) + ",turnout," + pin + "," + str(state) 
                    JMRI_Websocket_log.info("Sending message to: " + message)
                    feedback = clients[client_id].sendMessage(message)
                    JMRI_Websocket_log.info(feedback)
            else:
                    JMRI_Websocket_log.info("No client with client id of " + client_id + " is currently listenting....")

        return

# Event associated with adding or removing turnout from turnout table.
class ManagerTurnoutListener(java.beans.PropertyChangeListener):
  
  def propertyChange(self, event):
    
    #JMRI_Websocket_log.info(event)
    # first we delete all the current  listners using the first event (of three)
    if event.propertyName == "beans" and event.propagationId is None :
       JMRI_Websocket_log.info("deleting beans")
       list = event.source.getNamedBeanSet()
       
       for turnout in list :
            turnout.removePropertyChangeListener(turnoutListener)

   # now listen to all the valid turnouts in the updated table (full list in thrid event")
    if event.propertyName == "length":
    	
        list = event.source.getNamedBeanSet()
        #JMRI_Websocket_log.info(event)
        #JMRI_Websocket_log.info("List length: " + str(len(list)) )
        JMRI_Websocket_log.info("Turnout table has changed...")
        count = 0
        for turnout in list :
            turnoutid = turnout.getSystemName()
            
            if check_valid_name(turnoutid): # check for valid name and add a listner 
                #JMRI_Websocket_log.info("Turnout table changed: " + turnoutName )
                turnout.addPropertyChangeListener(turnoutListener)
                count = count + 1
                
        JMRI_Websocket_log.info("Now listening for " +  str(count) + " turnouts...")

turnoutListener = JMRI_Websocket_Turnout_Listener()
turnouts.addPropertyChangeListener(ManagerTurnoutListener())

class JMRI_Websocket(WebSocket):
    
    def handleMessage(self):

       #JMRI_Websocket_log.info(self.data)

       if self.data[:3] == 'ack': #just a heartbeat message
	    JMRI_Websocket_log.info("ACK")
            return

       client_id = self.data.split(',')

       if client_id[3] == 'x': #just a heartbeat message
            return
                  
       # determin if we have a sensor in the sensor table with this id.
       sensorSysName = "IS" + client_id[1] + ":" + client_id[3]
       #MRI_Websocket_log.info(sensorSysName + " Change state")
       sensor = sensors.getBySystemName(sensorSysName)
       if sensor != None: # sensor exists
            JMRI_Websocket_log.info(self.data)
            if int(client_id[4]) == 1:
               sensor.setKnownState(jmri.Sensor.ACTIVE)
            else:
               sensor.setKnownState(jmri.Sensor.INACTIVE)
       else: # sensor does not exist
             JMRI_Websocket_log.error("'JMRI_Websocket' - non-existent Sensor [" + sensorSysName + "]")


    def handleConnected(self):

        client_id = self.request.raw_requestline.split(" ")
        client_id = client_id[1][1:]
        JMRI_Websocket_log.info('Client connected: ' + str(client_id) + "  From: " + str(self.address[0]))
       
        if self not in clients:
              clients[client_id] = self
              #JMRI_Websocket_log.info(str(client_id) + ":" + str(self))
 
    def handleClose(self):

        JMRI_Websocket_log.info(self)
        val_list = list(clients.values())
        key_list = list(clients.keys())
        position = val_list.index(self)
        client_id = key_list[position]
        
        JMRI_Websocket_log.info( "Client disconnected: " + client_id + ": " + clients[client_id].address[0] + "..." )

        del clients[key_list[position]]

def get_clienid_pin(name):
        
        name = name.split(':')
        name[0] = name[0][2:] #This remove the "TI" part of the name
    
        return name[0], name[1] 
        
def check_valid_name(item_name):

    use = True

    if item_name[:2] != "IT": # we are only interested in internal (IT) turnouts.
                use = False

    if use and item_name.find(':') == -1:
                use = False
                JMRI_Websocket_log.info(item_name + ": Turnout name does not contain a colon.... e.g. AAAA:P. Ignoring..." )
                
    #test if there is some value after the colon.
    client_id = item_name.split(':')	
    if len(client_id) < 2 and use:
                use = False
                JMRI_Websocket_log.info(item_name + ": Turnout name does not contain a pin number after the : .... e.g. AAAA:P. Ignoring...")
                
    return use


def client_ping():

   while True:

	   time.sleep(30)
	   JMRI_Websocket_log.info("ping")
	   #try:
           for client in clients:
                	
			#JMRI_Websocket_log.info(client)
                	clients[client].sendMessage("ping")
           #except:
           #   e = sys.exc_info()[0]
           #   JMRI_Websocket_log.info( e )
   return

# hack to get this machines lan ip address (interface) to listen on.
def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP

if globals().get("JMRI_Websocket_running") != None: # Script already loaded so exit script

    JMRI_Websocket_log.warn("'JMRI Websocket Server' already loaded and running. Restart JMRI before load this script.")

else: # Continue running script

    JMRI_Websocket_log.info("JMRI Websocket Interface started...")
    JMRI_Websocket_running = True

    # get this server LAN IP if not already set
    if JRMI_SERVER_ADDRESS == None:
       JRMI_SERVER_ADDRESS = get_ip()

    # create a websocket server and spin off as a thread.
    wss = SimpleWebSocketServer(JRMI_SERVER_ADDRESS, JRMI_SERVER_PORT, JMRI_Websocket)
    wsst = threading.Thread(target=wss.serveforever)
    wsst.daemon = True
    wsst.start()

    
    # get a list of all turnouts in the table and add listeners to the internal ones. 
    count = 0
    for turnout in turnouts.getNamedBeanSet():
        turnoutid = turnout.getSystemName()
        
        if check_valid_name(turnoutid):
                count = count + 1
                #JMRI_Websocket_log.info("'JMRI Websocket' - Sensor SystemName [" + turnoutid+ "]")
                turnout.addPropertyChangeListener(turnoutListener)

    JMRI_Websocket_log.info("'JMRI Websocket' - " + str(count) + " Valid internal turnouts found...")

