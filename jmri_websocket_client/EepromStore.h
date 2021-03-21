#ifndef EEStore_h
#define EEStore_h

#include  "Arduino.h"
#include  "vars.h"
#include  <Servo.h>

#define   EESTORE_ID "JMRI"

struct EEStoreData{
  char id[sizeof(EESTORE_ID)]; 
  char websockets_server_host[sizeof(DEFAULTWSSERVER)];                    
  char websockets_server_port[sizeof(DEFAULTWSPORT)];
  char client_id[sizeof(DEFAULTWSCLIENTID)]; 
  String client_ip;       
  int active_pins[PINS];
  int pwm_pins[PINS] ;  
  int sensor_pins[PINS];
};


struct EEStore{
  static EEStore *eeStore;
  EEStoreData data;
  static void updateWebSockHost(String websockets_server_host);
  static void updateWebSockPort(String websockets_server_port);
  static void updateClientId   (String client_id);
  static void updateActivePins (int *active_pins);
  static void updatePWMPins    (int *pwm_pins);
  static void updateSensorPins (int *sensor_pins);
  static void config_active_pins (jmriData *jmri_data, int *pins);
  static void config_pwm_pins    (jmriData *jmri_datax, int *pins);
  static void config_sensor_pins (jmriData *jmri_data, int *pins);
  static int eeAddress;
  static jmriData init();
  static void reset();
  static int pointer();
  static void configeeprom();

};

#endif
