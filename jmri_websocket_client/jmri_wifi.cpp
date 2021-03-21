#include "jmri_wifi.h"

 
void JMRI_Wifi::connect_wifi(jmriData *jmri_data){


    // Connect to wifi
    WiFi.begin(jmri_data->ssid, jmri_data->password);
    jmri_data->wifi_client = WiFi;
    
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
        jmri_data->client_ip = WiFi.localIP().toString();
        jmri_data->client_mac = WiFi.macAddress();
        Serial.println("");
        Serial.println("Connected to Wifi: " + jmri_data->ssid + "...");
        Serial.print("Client IP: ");
        Serial.println(WiFi.localIP().toString());
        Serial.print("MAC: ");
        Serial.println(WiFi.macAddress());
        Serial.println("");
    }

}
