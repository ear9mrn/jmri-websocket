/*
   This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    Created by Peter Nevill 2021
*/
    
#include <ESPAsyncWebServer.h>
#include <ArduinoWebsockets.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Servo.h>
#include "helper.h"
#include "EepromStore.h"
#include "web_config.h"
#include "config.h"


      
void setup() {

      Serial.begin(115200);

      while (! Serial); //wait for serial port to get warmed up!  
      delay(random(1000));

      
      EEStore::init(); 
      websockets_server_host = EEStore::eeStore->data.websockets_server_host;
      websockets_server_port = EEStore::eeStore->data.websockets_server_port;
      client_addr            = EEStore::eeStore->data.client_addr;
      

      Serial.println("Websocket server address: " + websockets_server_host);  
      Serial.println("Websocket server port: "    + websockets_server_port);
      Serial.println("Client address: "           + client_addr); 
      

      Serial.print("Active Pins: ");
      for (int i=0; i<PINS; i++){
               active_pins[i] = EEStore::eeStore->data.active_pins[i];
               if (active_pins[i] != MYNULL && active_pins[i] != 99) {
                  pinMode(active_pins[i],OUTPUT);
                  Serial.print(active_pins[i]);
                  Serial.print(",");
                  active_count += 1;
               } else {
                  active_pins[i] = MYNULL;
               }
      } Serial.println(" ");
     
      //get the PWM pin numbers from EEPROM
      Serial.print("PWM(Servo) Pins: ");
      for (int i=0; i<PINS; i++){
             servo_pins[i] = EEStore::eeStore->data.servo_pins[i];
             if (servo_pins[i] != MYNULL && servo_pins[i] != 99) {
                servos[i].attach (servo_pins[i], 700, 2500);
                servo_count += 1;
                Serial.print(servo_pins[i]);
                Serial.print(",");
             } else {
                  servo_pins[i] = MYNULL;
             }
             
      } Serial.println(" ");

      //get the sensor pin numbers from EEPROM
      Serial.print("Sensor Pins: ");
      for (int i=0; i<PINS; i++){
            sensor_pins[i] = EEStore::eeStore->data.sensor_pins[i];
           if (sensor_pins[i] != MYNULL && sensor_pins[i] != 99) {
              pinMode(sensor_pins[i],INPUT_PULLUP);
              sensor_state[i] = digitalRead(sensor_pins[i]);
              sensor_count += 1;
              Serial.print(sensor_pins[i]);
              Serial.print(",");
           }  else {
                  sensor_pins[i] = MYNULL;
             }
      }
    
    
    Serial.println(" ");
    Serial.println(" ");

    //connect to wifi
    client_wifi = connect_wifi(ssid, password);
    
    //connect to websocket
    connected = connect_socket(&client,  client_addr, websockets_server_host, websockets_server_port);  

  
    // Send web page with input fields to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
           request->send_P(200, "text/html", index_html, processor);
           
    });
    
    server.on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
           run_test = true;
           request->send(200, "text/html", "Conducting pin configuration test....  <br><br><a href=\"/\">Return to Config Page</a>");              
    });

    server.on("/eeprom", HTTP_GET, [](AsyncWebServerRequest *request){
           reset_eeprom(); 
           request->send(200, "text/html", "Resetting eeprom...<br><br><a href=\"/\">Return to Config Page</a>");             
    });
    
    // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    String inputMessage;
    String inputParam;
    
    // GET input1 value for JRMI host ip addres
    if (request->hasParam(PARAM_INPUT_1)) {
          inputMessage = request->getParam(PARAM_INPUT_1)->value();
          inputParam = PARAM_INPUT_1;
          websockets_server_host = inputMessage;
          Serial.println("Updated JRMI host IP address: " + websockets_server_host);
          EEStore::eeStore->updateWebSockHost(websockets_server_host);
          reset_connection = true;
    }
    
    // GET input2 value for JMRI host port
    else if (request->hasParam(PARAM_INPUT_2)) {
          inputMessage = request->getParam(PARAM_INPUT_2)->value();
          inputParam = PARAM_INPUT_2;
          websockets_server_port = inputMessage;
          EEStore::eeStore->updateWebSockPort(websockets_server_port);
          Serial.println("Updated JRMI host port number: " + websockets_server_port);
          reset_connection = true;
    }
    
    // GET input3 value this client address. unique upto 7 characters.
    else if (request->hasParam(PARAM_INPUT_3)) {
          inputMessage = request->getParam(PARAM_INPUT_3)->value();
          inputParam = PARAM_INPUT_3;
          client_addr = inputMessage;
          EEStore::eeStore->updateClientAddr(client_addr);
          Serial.println("Updated client address: " + inputMessage);
          reset_connection = true;
    }
    
     // GET input4 value comma seperated list of active pins for tunrnout activation (frog relay)
    else if (request->hasParam(PARAM_INPUT_4)) {
          inputMessage = request->getParam(PARAM_INPUT_4)->value();
          Serial.println("Active pins to EEPROM: " + inputMessage);
          inputParam = PARAM_INPUT_4;
          String inmesg[PINS];
          active_count = split(inputMessage, ',', inmesg);
          for (int i=0; i<PINS; i++){
            if (i < active_count){
                  Serial.println(inmesg[i]);
                  active_pins[i] = inmesg[i].toInt();
                  pinMode(active_pins[i],OUTPUT);
            } else {
                  active_pins[i] = MYNULL;
            }
            EEStore::eeStore->data.active_pins[i] = active_pins[i];
          }
          Serial.println("Updated active pins: " + inputMessage);
          EEStore::eeStore->updateStore();

    }
    
    // GET input5 value comma seperated list of pwm pins for tunrnout motor (enter -1 for not active)
    // there should be an equal number in both lists.
    else if (request->hasParam(PARAM_INPUT_5)) {
          inputMessage = request->getParam(PARAM_INPUT_5)->value();
          Serial.println("Written PWM pins to EEPROM: " + inputMessage);
          inputParam = PARAM_INPUT_5;
          String inmesg[PINS];
          servo_count = split(inputMessage, ',', inmesg);
          for (int i=0; i<PINS; i++){
            if (i < servo_count){
                  servo_pins[i] = inmesg[i].toInt();
                  servos[i].attach (servo_pins[i], 700, 2500);
            } else {
                  servo_pins[i] = MYNULL;
            }
            EEStore::eeStore->data.servo_pins[i] = servo_pins[i];
          }
          Serial.println("Updated PWM(servo) pins : " + inputMessage);
          EEStore::eeStore->updateStore();
          
    }

    // GET input6 value comma seperated list of sensor pins
    else if (request->hasParam(PARAM_INPUT_6)) {
          inputMessage = request->getParam(PARAM_INPUT_6)->value();
          Serial.println("Written sensor pins to EEPROM: " + inputMessage);
          inputParam = PARAM_INPUT_6;
          String inmesg[PINS];
          sensor_count = split(inputMessage, ',', inmesg);
          for (int i=0; i<PINS; i++){
            if (i < sensor_count){
                  sensor_pins[i] = inmesg[i].toInt();
                  pinMode(sensor_pins[i],INPUT_PULLUP);
                  sensor_state[i] = digitalRead(sensor_pins[i]);
            } else {
                  sensor_pins[i] = MYNULL;
            }
            EEStore::eeStore->data.sensor_pins[i] = sensor_pins[i];
          }
          Serial.println("Updated sensor pins : " + inputMessage);
          EEStore::eeStore->updateStore();
         
    }

    
    else {
          inputMessage = "No message sent";
          inputParam = "none";
    }

    
    request->send(200, "text/html", "JMRI Client configuration updated: " 
               + inputParam + " with value: " + inputMessage +
               ". Saved to EEPROM.<br><a href=\"/\">Return to Config Page</a>");
     });
  
    server.onNotFound(notFound);
    server.begin();
    
    
}
void reset_eeprom(){

     EEStore::configeeprom();
     websockets_server_host = "123.123.123.123";
     websockets_server_port = "8000";
     client_addr            = "1001";
     for (int i=0; i<PINS; i++){
            active_pins[i] = MYNULL;
            servo_pins[i] =  MYNULL;
            sensor_pins[i] = MYNULL;
     }
     
}

void re_connect(){

          Serial.println("Lost connection, checking...");
          if (WiFi.status() != WL_CONNECTED){
             Serial.println("Lost wifi. Trying to re-connect...");
              WiFi = connect_wifi(ssid, password);
          } else {
            Serial.println("Wifi connection ok...Checking websocket...");
          }
          
          connected = false;
          
          while (!connected ) {
            //Serial.println("Trying again...");
            connected = connect_socket(&client, client_addr, 
                           websockets_server_host, websockets_server_port);
            delay(500);
          }
          
          Serial.println("Connected...");
}

String processor(const String& var)
{
        if(var == "CLIENTADDRESS")
          return WiFi.localIP().toString();
          
        if(var == "CLIENTMACADDRESS")
          return WiFi.macAddress();
          
        if(var == "WEBSOCKETHOST")
          return websockets_server_host;
      
        if(var == "WEBSOCKETPORT")
          return websockets_server_port;
      
        if(var == "CLIENTADDR")
           return client_addr;
      
        if(var == "ACTIVEPINS") {
          String pins = ""; 
          int i = 0;
          for (int i=0; i<active_count; i++){
              pins += String(active_pins[i]);
              pins += ",";
            }
          pins.remove(pins.length()-1);
          return pins;
        }

        if(var == "PWMPINS") {
          String pins = ""; 
          int i = 0;
          for (int i=0; i<servo_count; i++){
              pins += String(servo_pins[i]);
              pins += ",";
            }
          pins.remove(pins.length()-1);
          return pins;
        }
       
        if(var == "JMRITURNOUTS") {
              String jmripins = "";
              for (int i=0; i<active_count; i++){
                    if (i>0){
                       jmripins += ",";
                    }
                    jmripins += "IT";
                    jmripins += client_addr;
                    jmripins += ":";
                    jmripins += String(active_pins[0]);
      
                  }

              return jmripins;
        }

        if(var == "SENSORPINS") {
          String pins = ""; 
          int i = 0;
          for (int i=0; i<sensor_count; i++){
              pins += String(sensor_pins[i]);
              pins += ",";
            }
          pins.remove(pins.length()-1);
          return pins;
        }
}

void pin_activate(int thepin, int thestate){

         for (int i = 0; i < PINS; i++){
              if (active_pins[i] == MYNULL){
                break;
              }
              if (thepin == active_pins[i]){

                  if (servo_pins[i] != MYNULL){
                      servos[i].write(180 * thestate);
                  }
                  digitalWrite( thepin, thestate );
                  Serial.print("Pin: " + String(thepin) + " ");
                  Serial.print("PWM Pin: " + String(servo_pins[i]) + ", ");
                  Serial.println("State: " + String(thestate));
                  client.send("ack:" + client_addr); //send ack
                  
              }
          }
            
}

void loop() {

    currentMillis = millis();

    while (reset_connection){ // bit of a hack to get the websocket client to restart...
      Serial.println("Restarting websocket...");
      delay(100);
      client.close();
      delay(1000);
      reset_connection = false;
     }
      
    //check to see if there any any requests
    if(client.available()) {
        client.poll();
    }
    delay(10);

    if (run_test == true){

       for (int i = 0; i<PINS; i++){
                 if (active_pins[i] == MYNULL){
                  break;
                 }
               
                 pin_activate(active_pins[i], 1);
                 delay(2000);
                
                 pin_activate(active_pins[i], 0);
                 delay(2000);               

           }
       run_test = false;    
    }

    
    //let the server know state we are still connected...
    if (currentMillis - previousMillis >= OnTime  ) {
      //this is a heartbeat message
      while (! send_message(&client, client_addr, WiFi.localIP().toString(), "x", "x")) {
            //If we do not successfully send the message, check our connection and try again.
            Serial.println("Problem sending websocket message...");
            re_connect();
        }
      previousMillis = currentMillis;
    }

    client.onMessage([](WebsocketsMessage msg){
           
            String inmesg[10];
            int count;
            
            count = split(msg.data(), ',', inmesg);
            Serial.println("Message: " + msg.data());

            pin_activate (inmesg[4].toInt(), inmesg[5].toInt());
            
    });    

    //check for any changes in sensor state
    for (int i=0; i < sensor_count; i++){

       if ( sensor_state[i] !=  digitalRead(sensor_pins[i]) ){
            send_message(&client,  client_addr,  WiFi.localIP().toString(), 
                      String(sensor_pins[i]), String(digitalRead(sensor_pins[i])) );
            sensor_state[i] =  digitalRead(sensor_pins[i]);
       }
    }
    
}
