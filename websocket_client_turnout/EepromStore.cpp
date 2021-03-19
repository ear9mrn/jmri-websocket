//#include "config.h"
#include "EepromStore.h"
#include <EEPROM.h>

void EEStore::init(){

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
        sprintf(eeStore->data.client_addr, DEFAULTWSCLIENTID);
      
        for (int i=0; i<PINS; i++){
            eeStore->data.active_pins[i] = MYNULL;
            eeStore->data.servo_pins[i] = MYNULL;
            eeStore->data.sensor_pins[i] = MYNULL;
        }
        
        EEPROM.put(0,eeStore->data); 
        EEPROM.commit();  
         
     reset(); 

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
void EEStore::updateClientAddr(String  client_addr)
{

      sprintf(eeStore->data.client_addr,client_addr.c_str());
      EEPROM.put(0,eeStore->data);
      EEPROM.commit();

}

void EEStore::updateStore()
{

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
