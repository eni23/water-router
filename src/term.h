#include "config.h"
#include "serial-term/SerialTerm.cpp"

#ifndef __TERM_H
#define __TERM_H


extern SerialTerm term;
extern void init_term();
extern void serial_print_ipconfig();
extern void serial_print_ip( uint8_t *addr, boolean nl );
extern void ipcpy( uint8_t* ipaddr, uint8_t* eepc );
extern void get_ip_from_string(String ipstr, uint8_t *result );
extern void term_wifi_reconnect();

#endif
