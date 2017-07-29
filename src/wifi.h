#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"


#ifndef __WIFI_H
#define __WIFI_H

extern EspClass esp;

extern uint32_t last_wifi_connection_try;

extern uint32_t loop_ticks;

extern void wifi_reconnect_loop();

extern void wifi_init();

extern void wifi_connect( bool reconnect = false );

extern void wifi_set_static_if_enabled();


#endif
