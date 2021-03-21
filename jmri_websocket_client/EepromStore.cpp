
#include "EepromStore.h"
#include <EEPROM.h>


jmriData EEStore::init(){

    EEPROM.begin(512);
    Serial.begin(115200);
    Serial.println("Size of EEPROM" + EEPROM.length() );
    Serial.println("");
    Serial.println("Getting configuration data from EEPROM:");
    Serial.println("");


    eeStore=(EEStore *)calloc(1,sizeof(EEStore));
    EEPROM.get(0,eeStore->data);
    
    if ( strncmp(eeStore->data.id,EESTORE_ID,sizeof(EESTORE_ID))!=0 ) {
        Serial.println("EEPROM is not configured. Setting up EEPROM...");
        Serial.println("Please visit client ip using a browser to configure...");
        Serial.println("");

        configeeprom();
        
    }
    
    jmriData jmri_data;


    Serial.println("Websocket server address: " + String(jmri_data.websockets_server_host) );  
    Serial.println("Websocket server port: "    + String(jmri_data.websockets_server_port) );
    Serial.println("Client address: "           + jmri_data.client_id); 
    Serial.println("");
    
    config_active_pins  (&jmri_data, eeStore->data.active_pins);
    config_pwm_pins     (&jmri_data, eeStore->data.pwm_pins );
    config_sensor_pins  (&jmri_data, eeStore->data.sensor_pins );

    jmri_data.websockets_server_host = eeStore->data.websockets_server_host;
    jmri_data.websockets_server_port = eeStore->data.websockets_server_port;
    jmri_data.client_id              = String(eeStore->data.client_id);
    
    return jmri_data;
    
}

void EEStore::configeeprom(){

        //clear the eeprom
        for (int i = 0 ; i < EEPROM.length() ; i++) {
              EEPROM.write(i, 0);
        }

        //write default data to eeprom
        sprintf(eeStore->data.id,EESTORE_ID);
        sprintf(eeStore->data.websockets_server_host,DEFAULTWSSERVER);
        sprintf(eeStore->data.websockets_server_port,DEFAULTWSPORT);
        sprintf(eeStore->data.client_id, DEFAULTWSCLIENTID);
      
        for (int i=0; i<PINS; i++){
            eeStore->data.active_pins[i] = MYNULL;
            eeStore->data.pwm_pins[i]    = MYNULL;
            eeStore->data.sensor_pins[i] = MYNULL;
        }
        
        EEPROM.put(0,eeStore->data); 
        EEPROM.commit();  
         
     reset(); 

} 

void EEStore::config_pwm_pins(jmriData *jmri_datax, int *pins){

      jmri_datax->pwm_count = 0;

      Serial.print("xPWM (Servo) Pins: ");
      for (int i=0; i<PINS; i++){
               jmri_datax->pwm_pins[i] = pins[i];
               if (pins[i] != MYNULL && pins[i] != 99) {
                  jmri_datax->servos[i].attach (pins[i], 700, 2500);
                  Serial.print(pins[i]);
                  Serial.print(",");
                  jmri_datax->pwm_count += 1;
               } else {
                  jmri_datax->pwm_pins[i] = MYNULL;
               }
      } Serial.println(" ");
}

void EEStore::config_active_pins(jmriData *jmri_data, int *pins){

      jmri_data->active_count = 0;

      Serial.print("Active Pins: ");
      for (int i=0; i<PINS; i++){
               jmri_data->active_pins[i] = pins[i];
               if (pins[i] != MYNULL && pins[i] != 99) {
                  pinMode(pins[i],OUTPUT);
                  Serial.print(pins[i]);
                  Serial.print(",");
                  jmri_data->active_count += 1;
               } else {
                  jmri_data->active_pins[i] = MYNULL;
               }
      } Serial.println(" ");
}


void EEStore::config_sensor_pins(jmriData *jmri_data, int *pins){

      jmri_data->sensor_count = 0;
                    
      Serial.print("Sensor Pins: ");
      for (int i=0; i<PINS; i++){
               jmri_data->sensor_pins[i] = pins[i];
               if (pins[i] != MYNULL && pins[i] != 99) {
                  pinMode(pins[i],INPUT);
                  Serial.print(pins[i]);
                  Serial.print(",");
                  jmri_data->sensor_count += 1;
               } else {
                  jmri_data->sensor_pins[i] = MYNULL;
               }
      } Serial.println(" ");
}


void EEStore::updateWebSockHost(String websockets_server_host)
{
      sprintf(eeStore->data.websockets_server_host,websockets_server_host.c_str());
      EEPROM.put(0,eeStore->data);
      EEPROM.commit();   
}

void EEStore::updateWebSockPort(String websockets_server_port)
{
      sprintf(eeStore->data.websockets_server_port,websockets_server_port.c_str());
      EEPROM.put(0,eeStore->data);   
      EEPROM.commit(); 
}

void EEStore::updateClientId(String  client_id)
{
      sprintf(eeStore->data.client_id,client_id.c_str());
      EEPROM.put(0,eeStore->data);
      EEPROM.commit();
}

void EEStore::updateActivePins(int *active_pins)
{
      for (int i = 0; i<PINS; i++){
        eeStore->data.active_pins[i] = active_pins[i];
      }
      EEPROM.put(0,eeStore->data);
      EEPROM.commit();
}

void EEStore::updatePWMPins(int *pwm_pins)
{
      for (int i = 0; i<PINS; i++){
        eeStore->data.pwm_pins[i] = pwm_pins[i];
      }
      EEPROM.put(0,eeStore->data);
      EEPROM.commit();
}

void EEStore::updateSensorPins(int *sensor_pins)
{
      for (int i = 0; i<PINS; i++){
        eeStore->data.sensor_pins[i] = sensor_pins[i];
      }
      EEPROM.put(0,eeStore->data);
      EEPROM.commit();
}

        
void EEStore::reset(){
  eeAddress=sizeof(EEStore);
}

int EEStore::pointer(){
  return(eeAddress);
}

EEStore *EEStore::eeStore=NULL;
int EEStore::eeAddress=0;
