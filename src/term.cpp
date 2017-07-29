#include "Arduino.h"
#include "config.h"
#include "serial-term/SerialTerm.cpp"
#include "term.h"
#include "led.h"
#include "wifi.h"
#include "eeprom.h"

SerialTerm term;

void init_term(){

  term.begin(SERIAL_BAUD);

  term.on("led-blink-on", [](){
    led_blink(led_color_green, 300 );
  });

  term.on("led-off", [](){
    led_off();
    led_blink_stop();
  });


  term.on("enable-static-ip", [](){
    eeprom_config.static_ip = true;
    eeprom_save_config();
    term.debug("static ip enabled");
  });

  term.on("disable-static-ip", [](){
    eeprom_config.static_ip = false;
    eeprom_save_config();
    term.debug("static ip disabled");
  });

  // reconnect wifi
  term.on("reconnect-wifi" , [](){
    term_wifi_reconnect();
    return;
  });

  // turn on printing debug messages
  term.on("show-debug", [](){
    eeprom_config.debug_output_enabled = true;
    term.debug_enabled = true;
    eeprom_save_config();
    term.debug("debug output enabled");
    return;
  });

  // turn off printing debug messages
  term.on("hide-debug", [](){
    eeprom_config.debug_output_enabled = false;
    term.debug_enabled = false;
    eeprom_save_config();
    return;
  });


  /****************
   * wifi & ipconfig
   ***************/
   term.on("update-wifi", [](){

     String wifi_ssid = term.strArg(0);
     String wifi_pass = term.strArg(1);

     if ( eeprom_config.debug_output_enabled ) {
       Serial.println( "updating wifi-configuration in eeprom" );
       Serial.print( "new wifi_ssid: " );
       Serial.println( wifi_ssid );
       Serial.print( "new wifi_pass: " );
       Serial.println( wifi_pass );
     }

     wifi_ssid.toCharArray( eeprom_config.wifi_ssid, 64 );
     wifi_pass.toCharArray( eeprom_config.wifi_pass, 64 );
     eeprom_save_config();

     Serial.println("ok");
     return;

   });


   term.on("ipconfig", [](){

     String ipaddr_str = term.strArg(0);
     if (ipaddr_str.length()<7){
       serial_print_ipconfig();
       //debug("Usage: ipconfig [ipaddr] [netmask] [gateway] [dns-server]");
       return;
     }
     uint8_t conf_ip[4];
     get_ip_from_string(ipaddr_str, conf_ip);
     ipcpy(conf_ip, eeprom_config.static_ip_addr);
     Serial.print("ipaddr: ");
     serial_print_ip(conf_ip, true);


     String netmask_str = term.strArg(1);
     if (netmask_str.length()<7){
       eeprom_save_config();
       return;
     }
     uint8_t conf_nm[4];
     get_ip_from_string(netmask_str, conf_nm);
     ipcpy(conf_nm, eeprom_config.static_ip_netmask);
     Serial.print("netmask: ");
     serial_print_ip(conf_nm, true);

     String gw_str = term.strArg(2);
     if (gw_str.length()<7){
       eeprom_save_config();
       return;
     }
     uint8_t conf_gw[4];
     get_ip_from_string(gw_str, conf_gw);
     ipcpy(conf_gw, eeprom_config.static_ip_gateway);
     Serial.print("gateway: ");
     serial_print_ip(conf_gw, true);

     String dns_str = term.strArg(3);
     if (dns_str.length()<7){
       eeprom_save_config();
       return;
     }
     uint8_t conf_dns[4];
     get_ip_from_string(dns_str, conf_dns);
     ipcpy(conf_dns, eeprom_config.static_ip_dns);
     Serial.print("dns-server: ");
     serial_print_ip(conf_dns, true);

     eeprom_save_config();
     return;
   });

  /************
   * Get SysInfo
   ***********/
  term.on("info", [](){
    eeprom_load_config();
    delay(10);

    serial_print_ipconfig();

    // eeprom-dump
    Serial.print( "eeprom.wifi_ssid = " );
    Serial.println( eeprom_config.wifi_ssid );
    Serial.print( "eeprom.wifi_pass = " );
    Serial.println( eeprom_config.wifi_pass );


    Serial.print( "eeprom.static_ip = " );
    if (eeprom_config.static_ip){
      Serial.println( "enabled" );
    }
    else {
      Serial.println( "disabled" );
    }
    Serial.print( "eeprom.static_ip_addr = " );
    serial_print_ip( eeprom_config.static_ip_addr, true );
    Serial.print( "eeprom.static_ip_netmask = " );
    serial_print_ip(  eeprom_config.static_ip_netmask, true );
    Serial.print( "eeprom.static_ip_gateway = " );
    serial_print_ip(  eeprom_config.static_ip_gateway, true );
    Serial.print( "eeprom.static_ip_dns = " );
    serial_print_ip(  eeprom_config.static_ip_dns, true );
    Serial.print( "eeprom.debug_output_enabled = " );
    if ( eeprom_config.debug_output_enabled ) Serial.println( "true" );
    else Serial.println( "false" );

    // version & esp-internals
    Serial.println( "firmware.configversion = "  CONFIG_VERSION);

    Serial.print( "esp8266.free_heap = " );
    Serial.println( ESP.getFreeHeap() );
    Serial.print( "esp8266.chip_id = " );
    Serial.println( ESP.getChipId() );
    Serial.print( "esp8266.flash_size = " );
    Serial.println( ESP.getFlashChipSize() );
    Serial.print( "esp8266.flash_speed = " );
    Serial.println( ESP.getFlashChipSpeed() );
    Serial.print( "esp8266.cycle_count = " );
    Serial.println( ESP.getCycleCount() );
  });

}


void serial_print_ipconfig(){
    // wifi status
    String wifi_status_human="";
    switch ( WiFi.status() ) {
      case WL_NO_SHIELD:
        wifi_status_human="no shield";
        break;
      case WL_IDLE_STATUS:
        wifi_status_human="idle";
        break;
      case WL_NO_SSID_AVAIL:
        wifi_status_human="requested ssid not aviable";
        break;
      case WL_SCAN_COMPLETED:
        wifi_status_human="ssid scan completed";
        break;
      case WL_CONNECTED:
        wifi_status_human="connected";
        break;
      case WL_CONNECT_FAILED:
        wifi_status_human="connection failed";
        break;
      case WL_CONNECTION_LOST:
        wifi_status_human="connection lost";
        break;
      case WL_DISCONNECTED:
        wifi_status_human="disconnected";
        break;
    }
    // wifi info
    Serial.print( "wifi.status = " );
    Serial.println( wifi_status_human );
    Serial.print( "wifi.mac = " );
    byte mac[6];
    WiFi.macAddress(mac);
    Serial.print(mac[5],HEX);
    Serial.print(":");
    Serial.print(mac[4],HEX);
    Serial.print(":");
    Serial.print(mac[3],HEX);
    Serial.print(":");
    Serial.print(mac[2],HEX);
    Serial.print(":");
    Serial.print(mac[1],HEX);
    Serial.print(":");
    Serial.println(mac[0],HEX);
    if ( WiFi.status() == WL_CONNECTED ) {
      Serial.print( "wifi.ip = " );
      Serial.println( WiFi.localIP() );
      Serial.print( "wifi.netmask = " );
      Serial.println( WiFi.subnetMask() );
      Serial.print( "wifi.gateway = " );
      Serial.println( WiFi.gatewayIP() );
    }
}

void serial_print_ip( uint8_t *addr, boolean nl ){
  for (int i=0; i<4; i++){
    Serial.print(addr[i]);
    if (i<3){
      Serial.print(".");
    }
  }
  if (nl) {
    Serial.println("");
  }
}

void ipcpy( uint8_t* ipaddr, uint8_t* eepc ){
  for (int i=0; i<4; i++){
    eepc[i] = ipaddr[i];
  }
}

// helper for extracting ip addresses
void get_ip_from_string(String ipstr, uint8_t *result ){
  for (int i=0; i<4; i++){
    String num_str = term.str_token(ipstr, '.', i);
    result[i] = num_str.toInt();
  }
}

void term_wifi_reconnect( ) {
  term.debug("do a wifi-reconnect");
  term.debug( "disconnecting from current network (100ms)" );
  WiFi.disconnect();
  delay( 100 );
  term.debug( "connecting (10sec)" );
  WiFi.begin( eeprom_config.wifi_ssid, eeprom_config.wifi_pass );
  wifi_set_static_if_enabled();
  int rounds = 0;
  while ( WiFi.status() != WL_CONNECTED ) {
    delay( 50 );
    Serial.print( "." );
    rounds++;
    // delay=50ms, rounds=200, time=10sec
    if (rounds > 200 ){
      if (  WiFi.status() != WL_CONNECTED ) {
        Serial.println( "." );
        Serial.println( "fail" );
        term.debug( "failed to connect in given timeframe. connection open in background" );
        return;
      }
      break;
    }
    rounds++;
  }
  Serial.println( "." );
  if (  WiFi.status() == WL_CONNECTED ) {
    Serial.println( "ok" );
  }
}
