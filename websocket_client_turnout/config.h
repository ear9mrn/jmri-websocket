#ifndef Config_h
#define Config_h

#include <Servo.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoWebsockets.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

#define PINS 18
#define MYNULL 9999

const char* ssid =                    "myssid";       //Enter SSID
const char* password =                "mypass";         //Enter Password
String client_addr;                   //configured using web interface, saved in EEPROM
long OnTime = 30000;                  //interval of alive message in ms.  


int active_pins[PINS] ;               //configured using web interface, saved in EEPROM
int servo_pins[PINS] ;                //configured using web interface, saved in EEPROM
int sensor_pins[PINS] ;               //configured using web interface, saved in EEPROM
int sensor_state[PINS] ;
int sensor_count = 0;
int servo_count = 0;
int active_count = 0;

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
bool connected;
bool run_test = false;
bool reset_connection = false;

using namespace websockets;

WebsocketsClient client;
AsyncWebServer server(80);
Servo servos [PINS];
ESP8266WiFiClass client_wifi;

const char* PARAM_INPUT_1 = "JMRIHostIp";
const char* PARAM_INPUT_2 = "JMRIHostPort";
const char* PARAM_INPUT_3 = "ClientAddr";
const char* PARAM_INPUT_4 = "ActivePins";
const char* PARAM_INPUT_5 = "PWMPins";
const char* PARAM_INPUT_6 = "SensorPins";



String websockets_server_host;        //configured using web interface, saved in EEPROM
String websockets_server_port;        //configured using web interface, saved in EEPROM

void notFound(AsyncWebServerRequest *request);
String processor(const String& var);
void pin_activate(int thepin, int thestate);


#endif
