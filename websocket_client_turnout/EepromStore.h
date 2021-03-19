#ifndef EEStore_h
#define EEStore_h


#include  "Arduino.h"
#define   EESTORE_ID "JMRI"
#define   PINS 18
#define   MYNULL  9999
#define   DEFAULTWSSERVER "123.123.123.123"
#define   DEFAULTWSPORT "8000"
#define   DEFAULTWSCLIENTID "1001"

struct EEStoreData{
  char id[sizeof(EESTORE_ID)]; 
  char websockets_server_host[sizeof(DEFAULTWSSERVER)];                    
  char websockets_server_port[sizeof(DEFAULTWSPORT)];
  char client_addr[sizeof(DEFAULTWSCLIENTID)];         
  int active_pins[PINS];
  int servo_pins[PINS] ;  
  int sensor_pins[PINS];
};

struct EEStore{
  static EEStore *eeStore;
  EEStoreData data;
  static void updateWebSockHost(String websockets_server_host);
  static void updateWebSockPort(String websockets_server_port);
  static void updateClientAddr (String client_addr);
  static void updateStore      ();
  static int eeAddress;
  static void init();
  static void reset();
  static int pointer();
  static void configeeprom();
  //static void advance(int);
  //static void store();
  //static void clear();
};

#endif
