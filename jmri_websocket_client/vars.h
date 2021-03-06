#ifndef JMRI_VARS_h
#define JMRI_VARS_h

#include  "Arduino.h"
#define   DEFAULTWSSERVER "123.123.123.123"              //Websocket server ip address
#define   DEFAULTWSPORT "8000"                           //Websocket server port
#define   DEFAULTWSCLIENTID "1001"                       //Websocket client id each client has to have a unique id.
#define   OnTime 30000                                   //lag(ms) between sending keep alive to ws server.
#define   PINS 18                                        //number of GPIO pins available (>= to number available)

#define   MYNULL  9999   

#include  <Servo.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

#include <ArduinoWebsockets.h>
using namespace websockets;

struct jmriData{ 
  String websockets_server_host;                    
  String websockets_server_port;
  String client_id;  
  String client_mac; 
  const char* ssid;        
  const char* password; 
  String client_ip; 
  int active_pins[PINS];
  int pwm_pins[PINS];  
  int sensor_pins[PINS];
  int sensor_state[PINS];
  int sensor_count = 0;
  int pwm_count = 0;
  int active_count = 0;
  Servo servos [PINS];
  ESP8266WiFiClass wifi_client;
  WebsocketsClient *ws_client;
  bool connected;
  jmriData *jmri_ptr;
};

#endif
