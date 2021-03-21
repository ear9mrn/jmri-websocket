#include "jmri_web.h"
#include  "EepromStore.h"
#include  "jmri_ws.h"

const char* PARAM_INPUT_1 = "JMRIHostIp";
const char* PARAM_INPUT_2 = "JMRIHostPort";
const char* PARAM_INPUT_3 = "ClientAddr";
const char* PARAM_INPUT_4 = "ActivePins";
const char* PARAM_INPUT_5 = "PWMPins";
const char* PARAM_INPUT_6 = "SensorPins";

JMRI_WEB *JMRI_WEB::jmri_web_ptr=NULL;
int JMRI_WEB::webAddress=0;


void JMRI_WEB::jmri_web(AsyncWebServer *server, jmriData jmri_data){

    jmri_web_ptr=(JMRI_WEB *)calloc(1,sizeof(JMRI_WEB));
    jmri_web_ptr->jmri_data_web = jmri_data;
    
    //Send web page with input fields to client
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
           request->send_P(200, "text/html", index_html, processor);
           
    });
    
    server->on("/test", HTTP_GET, [](AsyncWebServerRequest *request){
           //run_test = true;
           request->send(200, "text/html", "Conducting pin configuration test....  <br><br><a href=\"/\">Return to Config Page</a>");              
    });

    server->on("/eeprom", HTTP_GET, [](AsyncWebServerRequest *request){
           //reset_eeprom(); 
           request->send(200, "text/html", "Resetting eeprom...<br><br><a href=\"/\">Return to Config Page</a>");             
    });
    
    // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
    server->on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {

    String inputMessage;
    String inputParam;
    
        // GET input1 value for JRMI host ip addres
        if (request->hasParam(PARAM_INPUT_1)) {
              inputMessage = request->getParam(PARAM_INPUT_1)->value();
              inputParam = PARAM_INPUT_1;
              update_ws_host(inputMessage);
              Serial.println("Updated JRMI host IP address: " + inputMessage);
              //reset_connection = true;
              //JMRI_WEB::sendheartbeat();
              
        }


        // GET input2 value for JMRI host port
        else if (request->hasParam(PARAM_INPUT_2)) {
              inputMessage = request->getParam(PARAM_INPUT_2)->value();
              inputParam = PARAM_INPUT_2;
              update_ws_port(inputMessage);
              Serial.println("Updated JRMI host port number: " + inputMessage);
              //reset_connection = true;
              //JMRI_WEB::sendheartbeat();
        }


        // GET input3 value this client address. unique upto 7 characters.
        else if (request->hasParam(PARAM_INPUT_3)) {
              inputMessage = request->getParam(PARAM_INPUT_3)->value();
              inputParam = PARAM_INPUT_3;
              update_client_id(inputMessage);
              Serial.println("Updated client address: " + inputMessage);
              //reset_connection = true;
              //JMRI_WEB::sendheartbeat();
        }

        // GET input4 value comma seperated list of active pins for tunrnout activation (frog relay)
        else if (request->hasParam(PARAM_INPUT_4)) {
              inputMessage = request->getParam(PARAM_INPUT_4)->value();
              inputParam = PARAM_INPUT_4;
              update_active_pins(inputMessage);
              Serial.println("Active pins to EEPROM: " + inputMessage);
    
        }

        // GET input5 value comma seperated list of pwm pins for tunrnout motor (enter -1 for not active)
        // there should be an equal number in both lists.
        else if (request->hasParam(PARAM_INPUT_5)) {
              inputMessage = request->getParam(PARAM_INPUT_5)->value();
              inputParam = PARAM_INPUT_5;
              update_pwm_pins(inputMessage);
              Serial.println("Written PWM pins to EEPROM: " + inputMessage);
        }

        // GET input6 value comma seperated list of sensor pins
        else if (request->hasParam(PARAM_INPUT_6)) {
              inputMessage = request->getParam(PARAM_INPUT_6)->value();
              inputParam = PARAM_INPUT_6;
              update_sensor_pins(inputMessage);             
              Serial.println("Written sensor pins to EEPROM: " + inputMessage);            
        }

    else {
          inputMessage = "No message sent";
          inputParam = "none";
    }

        request->send(200, "text/html", "JMRI Client configuration updated: " 
               + inputParam + " with value: " + inputMessage +
               ". Saved to EEPROM.<br><a href=\"/\">Return to Config Page</a>");
     });
    
    server->onNotFound(notFound);
    server->begin();
    
}

void JMRI_WEB::notFound(AsyncWebServerRequest *request) {
  
          request->send(404, "text/plain", "Not found");
}

void JMRI_WEB::update_ws_host(String ws_host){
  
        jmri_web_ptr->jmri_data_web.websockets_server_host = ws_host; 
        EEStore::updateWebSockHost(ws_host);
  
}

void JMRI_WEB::update_ws_port(String ws_port){
  
        jmri_web_ptr->jmri_data_web.websockets_server_port = ws_port; 
        EEStore::updateWebSockPort(ws_port);
  
}

void JMRI_WEB::update_client_id(String client_id){
  
        jmri_web_ptr->jmri_data_web.client_id = client_id; 
        EEStore::updateClientId(client_id);
  
}
void JMRI_WEB::update_active_pins(String active_pins){

       String inmesg[PINS];
       jmri_web_ptr->jmri_data_web.active_count = split(active_pins, ',', inmesg);
       
       for (int i=0; i<PINS; i++){
           if (i < jmri_web_ptr->jmri_data_web.active_count){
              Serial.println(inmesg[i]);
              jmri_web_ptr->jmri_data_web.active_pins[i] = inmesg[i].toInt();
              pinMode(jmri_web_ptr->jmri_data_web.active_pins[i],OUTPUT);
            } else {
              jmri_web_ptr->jmri_data_web.active_pins[i] = MYNULL;
            }
       }
       Serial.println("Updated active pins: " + active_pins);
       EEStore::updateActivePins(jmri_web_ptr->jmri_data_web.active_pins);
          
}

void JMRI_WEB::update_pwm_pins(String pwm_pins){

        String inmesg[PINS];
        jmri_web_ptr->jmri_data_web.pwm_count = split(pwm_pins, ',', inmesg);
        
        for (int i=0; i<PINS; i++){
              if (i < jmri_web_ptr->jmri_data_web.pwm_count){
                jmri_web_ptr->jmri_data_web.pwm_pins[i] = inmesg[i].toInt();
                jmri_web_ptr->jmri_data_web.servos[i].attach (jmri_web_ptr->jmri_data_web.pwm_pins[i], 700, 2500);
              } else {
                jmri_web_ptr->jmri_data_web.pwm_pins[i] = MYNULL;
              }
        }
        Serial.println("Updated PWM(servo) pins : " + pwm_pins);
        EEStore::updatePWMPins(jmri_web_ptr->jmri_data_web.pwm_pins);
}

void JMRI_WEB::update_sensor_pins(String sensor_pins){

        String inmesg[PINS];
        jmri_web_ptr->jmri_data_web.sensor_count = split(sensor_pins, ',', inmesg);
        for (int i=0; i<PINS; i++){
          if (i < jmri_web_ptr->jmri_data_web.sensor_count){
                jmri_web_ptr->jmri_data_web.sensor_pins[i] = inmesg[i].toInt();
                pinMode(jmri_web_ptr->jmri_data_web.sensor_pins[i],INPUT_PULLUP);
                jmri_web_ptr->jmri_data_web.sensor_state[i] = digitalRead(jmri_web_ptr->jmri_data_web.sensor_pins[i]);
          } else {
                jmri_web_ptr->jmri_data_web.sensor_pins[i] = MYNULL;
          }
        }
        Serial.println("Updated sensor pins : " + sensor_pins);
        EEStore::updateSensorPins(jmri_web_ptr->jmri_data_web.sensor_pins);
}


void JMRI_WEB::sendheartbeat()
{
      //JMRI_Ws::send_heartbeat(jmri_web_ptr->jmri_data_web);
}


String JMRI_WEB::processor(const String& var)
{
        if(var == "CLIENTADDRESS")
          //return WiFi.localIP().toString();
          return jmri_web_ptr->jmri_data_web.client_ip;
          
        if(var == "CLIENTMACADDRESS")
          //return WiFi.macAddress();
          return jmri_web_ptr->jmri_data_web.client_mac;
          
        if(var == "WEBSOCKETHOST")
          return jmri_web_ptr->jmri_data_web.websockets_server_host;
      
        if(var == "WEBSOCKETPORT")
          return jmri_web_ptr->jmri_data_web.websockets_server_port;
      
        if(var == "CLIENTADDR")
           return jmri_web_ptr->jmri_data_web.client_id;
      
        if(var == "ACTIVEPINS") {
          String pins = ""; 
          int i = 0;
          for (int i=0; i<jmri_web_ptr->jmri_data_web.active_count; i++){
              pins += String(jmri_web_ptr->jmri_data_web.active_pins[i]);
              pins += ",";
            }
          pins.remove(pins.length()-1);
          return pins;
        }

        if(var == "PWMPINS") {
          String pins = ""; 
          int i = 0;
          for (int i=0; i<jmri_web_ptr->jmri_data_web.pwm_count; i++){
              pins += String(jmri_web_ptr->jmri_data_web.pwm_pins[i]);
              pins += ",";
            }
          pins.remove(pins.length()-1);
          return pins;
        }
       
        if(var == "JMRITURNOUTS") {
              String jmripins = "";
              for (int i=0; i<jmri_web_ptr->jmri_data_web.active_count; i++){
                    if (i>0){
                       jmripins += ",";
                    }
                    jmripins += "IT";
                    jmripins += jmri_web_ptr->jmri_data_web.client_id;
                    jmripins += ":";
                    jmripins += String(jmri_web_ptr->jmri_data_web.active_pins[i]);
      
                  }

              return jmripins;
        }

        if(var == "SENSORPINS") {
          String pins = ""; 
          int i = 0;
          for (int i=0; i<jmri_web_ptr->jmri_data_web.sensor_count; i++){
              pins += String(jmri_web_ptr->jmri_data_web.sensor_pins[i]);
              pins += ",";
            }
          pins.remove(pins.length()-1);
          return pins;
        }
}


int JMRI_WEB::split(String data, char separator, String result[])
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex; i++ ){ 
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
            result[found-1] =  data.substring(strIndex[0], strIndex[1]) ;
        }
    }
    return found;
}
