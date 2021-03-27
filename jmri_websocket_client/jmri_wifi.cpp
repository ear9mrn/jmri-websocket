#include "jmri_wifi.h"

 
void JMRI_Wifi::connect_wifi(jmriData *jmri_data){

    //start fresh if already connected.
    if (WiFi.status() == WL_CONNECTED){
        WiFi.disconnect();
    }

    WiFi.begin(jmri_data->ssid, jmri_data->password);
    
    // Wait some time (10sec) to connect to wifi
    Serial.print("Waiting for Wifi to come up...");
    for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
        Serial.print(".");
        delay(1000);
    }

    
    //keep trying...
    if (WiFi.status() != WL_CONNECTED){
        Serial.println(" ");
        Serial.print("Problem connecting to wifi. Trying again.");
    }
    while ( WiFi.status() != WL_CONNECTED) {   
          WiFi.begin(jmri_data->ssid, jmri_data->password); 
          Serial.print(".");
          delay(10000);
          //need a timeout here...
    }


     jmri_data->wifi_client = WiFi;
     jmri_data->client_ip = WiFi.localIP().toString();
     jmri_data->client_mac = WiFi.macAddress();
     Serial.println("");
     Serial.println("Connected to Wifi: " + String(jmri_data->ssid));
     Serial.print("Client IP: ");
     Serial.println(WiFi.localIP().toString());
     Serial.print("MAC: ");
     Serial.println(WiFi.macAddress());
     Serial.println("");
    

}
