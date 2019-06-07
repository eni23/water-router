#include "mic5891.cpp"

#ifndef __WATER_H
  #define __WATER_H


extern MIC5891 mic5891;

extern bool pump_is_running;
extern uint8_t current_active_valve;
extern uint32_t pump_run_until;
extern uint32_t flow_counter;

extern void water_init();

extern void inc_flowcount();

extern void stop_water();

extern bool water(uint8_t valve, uint32_t amount_ml );


#endif
