#ifndef __WATER_H
  #define __WATER_H


extern void water_init();

extern void inc_flowcount();

extern void stop_water();

extern bool water(uint8_t valve, uint32_t amount_ml );


#endif
