/*******************************************************************************
 *
 * WaterRouter Wifi Firmware
 *
 * Author: Cyrill vW. < eni@e23.ch >
 *
 * This file needs to be compiled with Arduino-esp8266,
 * Website: https://github.com/esp8266/Arduino
 *
 ******************************************************************************/

#include <string.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Wire.h>


// wiring
#define GPIO_FLOWMETER    D1  // (D6)

// i2c port expander
#define PCF8574_SDA       D2
#define PCF8574_SCL       D3
#define PCF8574_I2CADDR   32
#define PCF8574_PORT_PUMP 0

// serial port baudrate
#define SERIAL_BAUD 115200
// maximal serial input line length
#define SERIAL_INPUT_BUFFER_SIZE 256
// configuration version (required to locate config in eeprom)
#define CONFIG_VERSION "003"
// start address for config-struct in eeprom
#define CONFIG_START 64


// settings struct stored in eeprom, default values only set if no config found
struct eeprom_config_struct {
  char version[4];
  char wifi_ssid[64];
  char wifi_pass[64];
  boolean static_ip;
  uint8_t static_ip_addr[4];
  uint8_t static_ip_netmask[4];
  uint8_t static_ip_gateway[4];
  uint8_t static_ip_dns[4];
  boolean debug_output_enabled;
} eeprom_config = {
  CONFIG_VERSION,
  "yourmom",
  "isfat",
  false,
  { 10,10,1,42  },
  { 255,255,0,0 },
  { 10,10,1,1   },
  { 8,8,8,8     },
  true
};


// if true, debug("foo") prints line on serial port

bool pump_is_running = false;
uint8_t current_active_valve = 0;
uint32_t pump_run_until = 0;
uint32_t flow_counter;

EspClass esp;
ESP8266WebServer *server;

/**
 * connect to wifi. if reconnect is true, connection is closed first
 **/
void wifi_connect( bool reconnect = false ) {
  if ( reconnect ) {
    debug( "auto: stop wifi" );
    WiFi.disconnect();
    delay( 100 );
  }
  debug( "auto: connecting to wifi" );
  WiFi.begin( eeprom_config.wifi_ssid, eeprom_config.wifi_pass );
  wifi_set_static_if_enabled();
}

void wifi_set_static_if_enabled(){
  if ( eeprom_config.static_ip ){
    debug("static ip enabled");
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


/**
 * split a string and get chuck at desired position
 * example:
 * data="foo,bar,baz" separator="," index=2  => baz
 * data="foo/bar" separator="/" index=0  => foo
 **/
String get_str_token( String data, char separator, int index ) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length()-1;
  for( int i = 0; i <= maxIndex && found <= index; i++ ){
    if( data.charAt( i ) == separator || i == maxIndex ){
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = ( i == maxIndex ) ? i + 1 : i;
    }
  }
  return found > index ? data.substring( strIndex[0], strIndex[1] ) : "";
}



/*******
 * EEPROM helpers
 * Simple load/save stuct-config into eeprom
 *******/

/**
 * load eeprom-struct if version check passes
 **/
void eeprom_load_config() {
  debug( "eeprom: load" );
  // If nothing is found we load default settings.
  if ( EEPROM.read( CONFIG_START + 0 ) == CONFIG_VERSION[0] &&
       EEPROM.read( CONFIG_START + 1 ) == CONFIG_VERSION[1] &&
       EEPROM.read( CONFIG_START + 2 ) == CONFIG_VERSION[2] ) {
    for ( unsigned int t = 0; t < sizeof( eeprom_config ); t++ ) {
      *( ( char* ) &eeprom_config + t ) = EEPROM.read( CONFIG_START + t );
    }
  }
}

/**
 * save eeprom-struct
 **/
void eeprom_save_config() {
  debug( "eeprom: save" );
  for ( unsigned int t = 0; t < sizeof( eeprom_config ); t++ ) {
    EEPROM.write( CONFIG_START + t, *( ( char* ) &eeprom_config + t ) );
  }
  EEPROM.commit();
}





/**
 * arduino-init function
 **/
void setup() {

  setup_pcf8574();
  pinMode( GPIO_FLOWMETER, INPUT_PULLUP );
  attachInterrupt(GPIO_FLOWMETER, inc_flowcount, RISING);
  sei();


  Serial.begin( SERIAL_BAUD );
  EEPROM.begin( sizeof( eeprom_config ) + CONFIG_START );
  delay( 50 );
  // load settings from eeprom to struct eeprom_config
  eeprom_load_config();
  // connect to wifi
  wifi_connect();
  delay(1000);
  server_setup();
}


/**
 * arduino-main entry point after setup()
 **/
uint32_t loop_ticks = 0;
uint32_t last_wifi_connection_try = 0;


void loop() {
  // do serial stuff first
  if ( Serial.available() ) {
    serial_char_event();
  }

  // if wifi isn't connected, reconnect. non-blocking, re-try every 20sec
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
  server->handleClient();
  delay(1);
}
