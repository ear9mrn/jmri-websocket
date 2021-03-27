
#include  "vars.h"
#include  "EepromStore.h"
#include  "jmri_wifi.h"
#include  "jmri_ws.h"
#include  "jmri_web.h"

WebsocketsClient client;
jmriData jmri_data;
AsyncWebServer server(80);
               
long previousMillis = 0;
long currentMillis = 0;

void setup() {

      //wait for serial port to get warmed up! 
      Serial.begin(115200);
      while (! Serial);  
      delay(random(1000));

      Serial.println("JMRI ESP2866 accessory client.");
      Serial.println("");
      
      //initiate/read eeprom
      jmri_data = EEStore::init(); 
      jmri_data.jmri_ptr = &jmri_data;
   
      //connect to wifi
      JMRI_Wifi::connect_wifi(&jmri_data);

      //start client config web server
      JMRI_WEB::jmri_web(&server,&jmri_data);

      //connect to jmri websocket server
      jmri_data.connected = false;
      jmri_data.ws_client = &client;
      
      while(!jmri_data.connected){
        JMRI_Ws::connect_socket(&jmri_data);
        delay(5000);
      }


}

void pin_activate(int thepin, int thestate){
                  
         for (int i = 0; i < PINS; i++){
              //Serial.println("PWM Pin: " + String(jmri_data.pwm_pins[i]));
              //Serial.println("Active Pin: " + String(jmri_data.active_pins[i]));
              if (jmri_data.active_pins[i] == MYNULL){
                break;
              }
              if (thepin == jmri_data.active_pins[i] ){
                  if (jmri_data.pwm_pins[i] != MYNULL){
                      jmri_data.servos[i].write(180 * thestate);
                  }
                  digitalWrite( thepin, thestate );
                  Serial.print("Pin: " + String(thepin) + " ");
                  Serial.print("PWM Pin: " + String(jmri_data.pwm_pins[i]) + ", ");
                  Serial.println("State: " + String(thestate));
                  client.send("ack:" + jmri_data.client_id); //send ack
              }
          }     
}

void loop() {

    currentMillis = millis();

    //let the server know state we are still connected...
    //doubles up as re-connect user changes IP port port number of ws server.
    if (currentMillis - previousMillis >= OnTime  ) {
      JMRI_Ws::send_heartbeat(&jmri_data);  
      previousMillis = currentMillis;
    }

    //check to see if there any any requests
    if(client.available()) {
        client.poll();
    }
    delay(5);

    client.onMessage([](WebsocketsMessage msg){
           
            String inmesg[10];
            int count;
            
            count = JMRI_WEB::split(msg.data(), ',', inmesg);
            Serial.println(msg.data());
            pin_activate (inmesg[4].toInt(), inmesg[5].toInt());
            
    });    

    //check for any changes in sensor state
    for (int i=0; i < jmri_data.sensor_count; i++){

       if ( jmri_data.sensor_state[i] != digitalRead(jmri_data.sensor_pins[i]) ){
            jmri_data.sensor_state[i] =  digitalRead(jmri_data.sensor_pins[i]);
            JMRI_Ws::send_message(&jmri_data, String(jmri_data.sensor_pins[i]), String(jmri_data.sensor_state[i]) );
       }
    }
  

}
