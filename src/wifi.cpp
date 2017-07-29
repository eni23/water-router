#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include "wifi.h"
#include "term.h"
#include "eeprom.h"
#include "led.h"


EspClass esp;

uint32_t last_wifi_connection_try = 0;
uint32_t loop_ticks = 0;


/**
 * init wifi (connect)
 **/
void wifi_init(){
  // since this is during startup, timer will run for aprox. 1000ms
  led_timeout_ui(RgbColor(150,0,0),60);
  term.debug( "wifi init" );
  WiFi.begin( eeprom_config.wifi_ssid, eeprom_config.wifi_pass );
  wifi_set_static_if_enabled();
}

/**
 * connect to wifi. if reconnect is true, connection is closed first
 **/
void wifi_connect( bool reconnect ) {
  if ( reconnect ) {
    term.debug( "auto: stop wifi" );
    WiFi.disconnect();
    delay( 100 );
  }
  led_timeout_ui(RgbColor(0,120,80),60);
  term.debug( "auto: connecting to wifi" );
  WiFi.begin( eeprom_config.wifi_ssid, eeprom_config.wifi_pass );
  wifi_set_static_if_enabled();
}

void wifi_set_static_if_enabled(){
  if ( eeprom_config.static_ip ){
    term.debug("static ip enabled");
    IPAddress static_ip(
      eeprom_config.static_ip_addr[0],
      eeprom_config.static_ip_addr[1],
      eeprom_config.static_ip_addr[2],
      eeprom_config.static_ip_addr[3]
    );
    IPAddress static_nm(
      eeprom_config.static_ip_netmask[0],
      eeprom_config.static_ip_netmask[1],
      eeprom_config.static_ip_netmask[2],
      eeprom_config.static_ip_netmask[3]
    );
    IPAddress static_gw(
      eeprom_config.static_ip_gateway[0],
      eeprom_config.static_ip_gateway[1],
      eeprom_config.static_ip_gateway[2],
      eeprom_config.static_ip_gateway[3]
    );
    WiFi.config(static_ip, static_gw, static_nm);
  }
}

void wifi_reconnect_loop(){
  if ( ( (loop_ticks + 1) ) > 4294967290 ) {
    loop_ticks = last_wifi_connection_try = 0;
  }
  if (  WiFi.status() != WL_CONNECTED ) {
    if ( loop_ticks > ( last_wifi_connection_try + 20000 ) ) {
      last_wifi_connection_try = loop_ticks;
      wifi_connect();
    }
  }

  loop_ticks++;
}
