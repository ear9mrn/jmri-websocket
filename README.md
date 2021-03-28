JMRI – Websocket/Wifi based turnout control and sensors: Proof of concept.

I would like to present a proof concept method for controlling model railway turnouts and monitoring sensors via wifi using the websocket protocol. The system is currently based on the ESP8266 wifi microchip that are both very small and cheap (£0.30 AliEpxress). They run on 3.3v (or on many breakout boards 5v).















The system is very simple and works as follow: A websocket server is started (jython script) when JMRI is started. This then listens for changes in turnout state in the turnout table and sends a message via websocket over wifi to the appropriate websocket client. The client then reacts to the message and changes the turnout as required.




















There is very little configuration required. Once the jython script is saved to the jmri script library/directory it can be run manually (from the panels menu) or set to automatically run at start up via the preferences. That is all that is required on the server (apart for populating the turnout/sensor table) side unless you wish to change the port number of the websocket server which is a simple matter of editing the script (default is 8000).  









To configure the turnout table you just create a new turnout (Internal) enter the client address (as set on the client side, see below) followed by a colon and the pin number, this enable multiple pins to be controlled via a single client if required. 


On the client side all that is required is to upload my proof of concept Arduino sketch (you do need to set your wifi ssid and password in config.h file but you only have to do this once). Once the sketch is uploaded and running you just then navigate to the ip address of that client with a browser. 

You can find the ip address of the client in the serial window of Arduino IDE as the board starts up and configures itself. There are other methods but this is the simplest.





Below is an example of the client configuration interface. The configuration is saved to the eeprom so will survive a reboot, as it can be edited via a web browser there is no need to have to plug device back into pc via USB to change any settings. The only reason you would need to do this is if you change your wifi ssid or password.

The first setting is the IP address of the machine running JMRI (with websocket   script running). Every time you make a change hit the “write to eeprom” button to the right of the box you edited. Only one change can be made at a time.





















The second box is the port the websocket is listening on (default is 8000). This can be change but the server and all clients need to use the same port. 

The next is the client address. This should be a 4 digit number with each wifi client being different. This address will also be used by JMRI turnout and sensor table so it knows which client to send a message to (see above). 

Active pins refers to the actual GPIO pins on the client board that a device is attached (e.g. relay). You can list as many as the board has (comma separated list).  e.g.   “0,4,5”


PWM pins is a list of pins that will have a servo/PWM device attached. They can be initiated at the same time as a pin listed in the active pin list. For example. If you want to initiate a relay (say pin 4) and drive a servo at the same time (on pin 6) your active pin list may look like:

0,4,5

and your PWM list looks like this:

55,6,55

in this example when pin zero is activated the “companion” pwm pin is 55. This is just a dummy pin and will do nothing. But when pin 4 in the active list is initiated pin six will also be initiated at the same time.

The order of the pins does not matter as long as the position in the list is correct for the pairs you wish to activate at the same time.

The same applies for the sensors, albeit in the opposite direction. If a pin listed as a sensor changes state. A message is sent to JMRI websocket and as long as a sensor is listed in the sensor table (internal sensor) with the appropriate client address and pin number (clientid:pin) the state will be changed. 













Step by step jmir websocket setup instructions:

Server:
    • This is carried on the machine running JMRI.
    • Download the code from github https://github.com/ear9mrn/jmri-websocket.
    • Place the JMRI_WebSocket folder (and content) in the JMRI/jython directory. 
    • Start JMRI and open “display message log” from the debug menu.
    • Start jmri websocket. From the “Panels” menu select “Run Script…” select the WebSocketInterface.py file from JMRI/jython/JMRI_WebSocket directory.
    • You should now see log information in the message log window indicating the websocket server is now running.
    • The only configuration possible it to change the default port from 8000 if required (edit the script appropriately if necassary).

Client:
    • This sketch has been tested on a range of ESP8266 based devices including:
        ◦ ESP-01 (attached to various break out boards)
            ▪ for 2x and 4x relay boards you need to set the “ESP01_2_4” flag to 1 in the vars.h file.
        ◦ ESP8266
            ▪ D1 Mini
            ▪ Node MCU Amica
        ◦ ESP-12F
            ▪ 8x relay board
    • From the same github repository https://github.com/ear9mrn/jmri-websocket download the jmir_websocket_client folder to your Arduino IDE folder.
    • Edit the config.h file entering the ssid and password for you wifi network.
    • Edit the vars.h file enter the ip address of the machine running jmri. Also change to port if you changed it on the server side from the default of 8000.
    • Open the serial monitor, compile and upload script to you ESP8266 device. Once running it should automatically connect to the jmri websocket, check the serial monitor this should also show the ip address the device has been assigned.
    • Enter the ip address assigned into a browser.
    • Change the “Client address” from the default 1001 to some other value between 1001 and 9999. Each client should have a unique number, this is how we identify a turnout or sensor. Once changed hit the “write to EEPROM” to save it permanently.
    • Add a list of pins that will be used to control relay (or some other device). This should be a comma separated list. (e.g. 3,2,5…). As before, hit the “write to EEPROM” to save it permanently.
    • Add PWM pins. These are pins that have a servo attached. There should be the same number of pins in the PWM pin list as listed in the active pin list. This is so you can active a relay for a frog and a servo with one command (pairs). If you do not have a paired servo pin for an active pin just enter a high value such as “99” then it will simply be ignored. As before, hit the “write to EEPROM” to save it permanently.
    • All changes are saved to EEPROM and so survive a reboot. You can make changes by simply entering the device IP address in a browser/ Th only exception is the wifi ssid and password, this can only be changed using the Arduino IDE and re-uploading the sketch.

JMRI turnout and sensor table:
    • Enter the id and pin numbers as defined above as internal turnout (IT) or internal sensors (IS).
    • For example if your called your device 3001 and pin 1 was connected to a relay for your turnout you enter 3001:1. You don’t need to worry about the servo (PWM) pin and that was linked to the active pin when you setup the device.
    • Sam applies to the sensor. If you called your device 3005 and the sensor was attached to pin 4 you need to create an internal sensor in the sensor table called 3005:4.



