JMRI – Websocket/Wifi based turnout control and sensors: Proof of concept.


I would like to present a proof concept method for controlling model railway turnouts and monitoring sensors via wifi using the websocket protocol. The system is currently based on the ESP8266 wi-fi microchip that are both very small and cheap (£0.30 AliEpxress). They run on 3.3v (or on many breakout boards 5v).















The system is very simple and works as follow: A websocket server is started (jython script) when JMRI is started. This then listens for changes in turnout state in the turnout table and sends a message via websocket over wifi to the appropriate websocket client. The client then reacts to the message and changes the turnout as required.






















There is very little configuration required. Once the jython script is saved to the jmri script library/directory it can be run manually (from the panels menu) or set to automatically run at start up via the preferneces. That is all that is required on the server (apart for populating the turnout/sensor table) side unless you wish to change the port number of the websocket server which is a simple matter of editing the script (default is 8000). 






















To configure the turnout table you just create a new turnout (Internal) enter the client address (as set on the client side, see below) followed by a colon and the pin number, this enable mutiple pins to be controlled via a single clinet if required. 


On the client side all that is required is to upload my proof of concept Arduino sketch (you do need to set your wifi ssid and password in config.h file but you only have to do this once). Once the sketch is uploaded and running you just then naviagate to the ip address of that client with a browser. 

You can find the ip address of the client in the serial window of Arduino IDE as the board starts up and configures itself. There are other methods but this is the simplest.





Below is an example of the client configuration interface. The configuration is saved to the eeprom so will survive a reboot, as it can be edited via a web broswer there is no need to have to plug device back into pc via USB to change any settings. The only reason you would need to do this is if you change your wifi ssid or password.

The first setting is the IP address of the machine running JMRI (with websocket script running). Every time you make a change hit the “write to eeprom” button to the right of the box you edited. Only one change can be made at a time.


























The second box is the port the websocket is listening on (default is 8000). This can be change but the server and all clients need to use the same port. 

The next is the client address. This should be a 4 digit number with each wifi client being differnet. This address will also be used by JMRI turnout and sensor table so it knows which client to send a message to (see above). 

Active pins refers to the actual GPIO pins on the client board that a device is attached (e.g. relay). You can list as many as the board has (comma seperated list). e.g. “0,4,5”


PWM pins is a list of pins that will have a servo/PWM device attached. They can be initiated at the same time as a pin listed in the active pin list. For example. If you want to intiate a relay (say pin 4) and drive a servo at the same time (on pin 6) your active pin list may look like:

0,4,5

and your PWM list looks like this:

55,6,55

in this example when pin zero is activated the “companion” pwm pin is 55. This is just a dummy pin and will do nothing. But when pin 4 in the active list is initiated pin six will also be initiated at the same time.

The order of the pins does not matter as long as the postion in the list is correct for the pairs you wish to activate at the same time.

The same applies for the sensors, albeit in the opposite direction. If a pin listed as a sensor changes state. A message is sent to JMRI websocket and as long as a sensor is listed in the sensor table (internal sensor) with the appropriate client address and pin number (clientid:pin) the state will be changed. 










