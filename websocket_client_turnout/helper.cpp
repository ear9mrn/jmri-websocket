
#include "helper.h"


int split(String data, char separator, String result[])
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


ESP8266WiFiClass connect_wifi(String ssid, String password){

    // Connect to wifi
    WiFi.begin(ssid, password);

    // Wait some time to connect to wifi
    Serial.print("Waiting for Wifi to come up...");
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    // Check if connected to wifi
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("No Wifi!");

    } else {
        Serial.println("");
        Serial.println("Connected to Wifi: " + String(ssid) + "...");
        Serial.print("Client IP: ");
        Serial.println(WiFi.localIP().toString());
        Serial.print("MAC: ");
        Serial.println(WiFi.macAddress());
        Serial.println("");
    }
    
    return WiFi;
  
}

bool send_message(WebsocketsClient *Myclient, String client_id, String client_ip, String which_pin, String curr_state){

        String message;
        message = "jmri,";                  //target id i.e to... normally jmri websocket server
        message += client_id;               //client id i.e. from 
        message += ","; 
        message += client_ip;              // sender ip address
        message += ",";
        message += which_pin;                 //pin
        message += ",";
        message += curr_state;                //pin state
                
        //try sending our message
        Serial.println(message);
        bool send = Myclient->send(message);

        return send;
        
}

bool connect_socket(WebsocketsClient *ws_client, String client_id, String ws_server_host, String ws_server_port  ){

    String clientid = "/" + client_id;
    const uint16_t port =  atoi( ws_server_port.c_str() );
    bool conn;
    
    Serial.println(" ");
    Serial.println("Trying to connect to Websocket...");

    conn = ws_client->connect(ws_server_host, port, clientid);
    
    if(conn) {
        Serial.print("Connected to Websocket server: ");
        Serial.println(ws_server_host);
        Serial.print("On port: ");
        Serial.println(ws_server_port);
     
        // run callback when messages are received
        ws_client->onMessage([&](WebsocketsMessage incmessage) {
              Serial.print("Message: ");
              Serial.println(incmessage.data());
        });
        //reset_connection = false;
        
    } else {
        Serial.print("Not able to obtain websocket connection: ");
        Serial.println(ws_server_host);
        Serial.print("On port: ");
        Serial.println(ws_server_port);
        delay(5000);
    }

    return conn;
   
}
