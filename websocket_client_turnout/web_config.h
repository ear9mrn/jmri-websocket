#ifndef WEBconf_h
#define WEBconf_h

#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include "Arduino.h"


const char index_html[] PROGMEM =  R"rawliteral(
<!DOCTYPE HTML>
<style>.hover {
  position: relative;
  top: 50px;
  left: 50px;
}</style>
<html><head>
  <a href="https://www.jmri.org/">
  <img src="https://www.jmri.org/images/logo-jmri.gif" alt="JMRI"></a>
  <title>JRMI Websocket Client Configuraiton</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h1>JMRI WebSocket Client Configuration.</h1>
  <p>Client IP Address: %CLIENTADDRESS%</p>
  <p>Client MAC Address: %CLIENTMACADDRESS%</p>
  <form action="/get">
    JMRI Host IP:    <input type="text" name="JMRIHostIp" value="%WEBSOCKETHOST%">
    <input type="submit" value="Write To EEPROM">
  </form><br>
  <form action="/get">
    JMRI Host Port:  <input type="text" name="JMRIHostPort" value="%WEBSOCKETPORT%">
    <input type="submit" value="Write To EEPROM">
  </form><br>
  <form action="/get">
    Client Address:    <input type="text" name="ClientAddr" value="%CLIENTADDR%">
    <input type="submit" value="Write To EEPROM">
  </form><br>
  <form action="/get">
    Active Pins:  <input type="text" name="ActivePins" value="%ACTIVEPINS%" 
    title="Comma seperated list of active digital pins."> 
    <input type="submit" value="Write To EEPROM">
  </form><br>
    <a>Internal Turnout(s) (IT) required based on active pins listed: <br>%JMRITURNOUTS%<a><br><br>
    <form action="/get">
    PWM Pins:    <input type="text" name="PWMPins" value="%PWMPINS%"
    title="Comma seperated list of PWM (servo) pins. Should be same number listed as active pins. If not used enter high value such as 99..."> 
    <input type="submit" value="Write To EEPROM">
  </form><br>
  <form action="/get">
    Sensor Pins:    <input type="text" name="SensorPins" value="%SENSORPINS%"
    title="Comma seperated list of pins use for sensors."> 
    <input type="submit" value="Write To EEPROM">
  </form><br>
  <button type="button" onclick=location.href='/test'>Test pin setttings</button>
  <button type="button" onclick=location.href='/eeprom'>Reset Eeprom</button>
</body></html>)rawliteral";


void notFound(AsyncWebServerRequest *request);
void pin_activate(int thepin, int thestate, int * active_pins, int * servo_pins, Servo *servos  );
void test(AsyncWebServerRequest *request, int * active_pins, int * servo_pins, Servo *servos);
  

#endif
