#include "config.h"
#include "eeprom.h"
#include <EEPROM.h>

eeprom_config_struct eeprom_config = {
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


void eeprom_init(){
  EEPROM.begin( sizeof( eeprom_config ) + CONFIG_START );
}

/**
 * load eeprom-struct if version check passes
 **/
void eeprom_load_config() {
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
  for ( unsigned int t = 0; t < sizeof( eeprom_config ); t++ ) {
    EEPROM.write( CONFIG_START + t, *( ( char* ) &eeprom_config + t ) );
  }
  EEPROM.commit();
}
