#ifndef JMRI_WS_h
#define JMRI_WS_h

#include  "vars.h"
#include <ArduinoWebsockets.h>

using namespace websockets;

struct JMRI_Ws {

  static void connect_socket(jmriData *jmri_data);
  static void send_message(jmriData *jmri_data, String which_pin, String curr_state);
  static void send_heartbeat(jmriData *jmri_data);
  
};

#endif
