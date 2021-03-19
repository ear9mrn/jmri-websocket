#include "web_config.h"


//this is currently not used. Idea is to use a url to test sensors.
// e.g.  ip/?pin=1&state=0

void test(AsyncWebServerRequest * request, int * active_pins, int * servo_pins, Servo *servos){

             int paramsNr = request->params();
             int testPin = 999;
             int pinState = 999;
             for(int i=0;i<paramsNr;i++){
                      AsyncWebParameter* p = request->getParam(i);
                      //Serial.println(p->name());
                      //Serial.println(p->value());
                      if ( p->name() == "pin" ){
                         testPin = p->value().toInt(); 
                     }
                      if ( p->name() == "state" ){
                         pinState = p->value().toInt();
                      }
             }
             if (testPin != 999 && pinState != 999 ){
                  pin_activate(testPin, pinState, active_pins, servo_pins, servos);
             } else {
                  Serial.println("Problem running test...");
             }

}




void pin_activate(int thepin, int thestate, int * active_pins, int * servo_pins, Servo *servos){
int PINS =18;
int MYNULL = 9999;

         for (int i = 0; i < PINS; i++){
              if (active_pins[i] == MYNULL){
                break;
              }
              if (thepin == active_pins[i]){
                
                  servos[i].write(180 * thestate);
                  digitalWrite( thepin, thestate );
                  Serial.print("Pin: " + String(thepin) + " ");
                  Serial.print("PWM Pin: " + String(servo_pins[i]) + ", ");
                  Serial.println("State: " + String(thestate));
                  //client.send("ack:" + client_addr); //send ack
                  
              }
          }
            
}


void notFound(AsyncWebServerRequest *request) {
          request->send(404, "text/plain", "Not found");
}
