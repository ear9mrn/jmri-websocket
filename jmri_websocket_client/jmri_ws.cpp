#include "jmri_ws.h"
#include  "jmri_wifi.h"

void JMRI_Ws::connect_socket(jmriData * jmri_data  ){

 
    String clientid = "/" + jmri_data->client_id;
    const uint16_t port =  atoi( jmri_data->websockets_server_port.c_str() );
    
    Serial.println(" ");
    Serial.println("Trying to connect to Websocket...");

    if (jmri_data->connected){
        jmri_data->ws_client->close();
    }
    jmri_data->connected = jmri_data->ws_client->connect(jmri_data->websockets_server_host, port, clientid);
    
    if(jmri_data->connected) {
        Serial.print("Connected to Websocket server: ");
        Serial.println(jmri_data->websockets_server_host);
        Serial.print("On port: ");
        Serial.println(jmri_data->websockets_server_port);
     
        // run callback when messages are received
        jmri_data->ws_client->onMessage([&](WebsocketsMessage incmessage) {
              Serial.print("Message: ");
              Serial.println(incmessage.data());
        });
        
    } else {
        Serial.print("Not able to obtain websocket connection: ");
        Serial.println(jmri_data->websockets_server_host);
        Serial.print("On port: ");
        Serial.println(jmri_data->websockets_server_port);
        delay(5000);
    }


}

void JMRI_Ws::send_message(jmriData *jmri_data, String which_pin, String curr_state){

        String message;
        message = "jmri,";                  //target id i.e to... normally jmri websocket server
        message += jmri_data->client_id;               //client id i.e. 
        message += ","; 
        message += jmri_data->client_ip;               // sender ip address
        message += ",";
        message += which_pin;               //pin
        message += ",";
        message += curr_state;              //pin state
                
        //try sending our message
        Serial.println(message);
        delay(5);

        while(!jmri_data->ws_client->send(message)){

            Serial.println("Problem sending message, checking...");
            if (jmri_data->wifi_client.status() != WL_CONNECTED){
                  Serial.println("Lost wifi. Trying to re-connect...");
                  JMRI_Wifi::connect_wifi(jmri_data);
                  delay(500);
            } else {
                Serial.println("Wifi connection ok...Checking websocket...");
            } 
            
            jmri_data->connected = false;
          
            while (!jmri_data->connected) {
                JMRI_Ws::connect_socket(jmri_data); 
                delay(500);
            }
            Serial.println("re-connected...");
        }

        
}

void JMRI_Ws::send_heartbeat(jmriData *jmri_data ){
  
      JMRI_Ws::send_message(jmri_data, "x", "x");

}
