#include "config.h"
#include <EEPROM.h>

#ifndef __EEPROM_H
#define __EEPROM_H

extern struct eeprom_config_struct {
  char version[4];
  char wifi_ssid[64];
  char wifi_pass[64];
  bool static_ip;
  uint8_t static_ip_addr[4];
  uint8_t static_ip_netmask[4];
  uint8_t static_ip_gateway[4];
  uint8_t static_ip_dns[4];
  bool debug_output_enabled;
} eeprom_config;

extern void eeprom_init();

extern void eeprom_load_config();

extern void eeprom_save_config();

#endif
