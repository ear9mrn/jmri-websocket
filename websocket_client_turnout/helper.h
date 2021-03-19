

#ifndef Helper_h
#define Helper_h

#include <Arduino.h>
#include <ArduinoWebsockets.h>
//#include <ESPAsyncWebServer.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

using namespace websockets;

int split(String data, char separator, String result[]);
ESP8266WiFiClass connect_wifi(String ssid, String password);
bool send_message(WebsocketsClient *Myclient, String client_id, String client_ip,  String which_pin, String curr_state);
bool connect_socket(WebsocketsClient *ws_client, String client_id, String ws_server_host, String ws_server_port); 

  
#endif
