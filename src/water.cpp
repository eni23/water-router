#include <Arduino.h>
#include "config.h"
#include "mic5891.cpp"
#include "term.h"
#include "water.h"


MIC5891 mic5891;

bool pump_is_running = false;
uint8_t current_active_valve = 0;
uint32_t pump_run_until = 0;
uint32_t flow_counter;

void water_init(){
  mic5891.begin(
    MIC5891_DATA,
    MIC5891_CLOCK,
    MIC5891_STROBE,
    MIC5891_OUTPUT_ENABLE
  );
}


void inc_flowcount() {
  flow_counter++;
  term.debug("inc_flowcount");
  if (flow_counter>=pump_run_until){
    stop_water();
  }
}


void stop_water(){

}

bool water(uint8_t valve, uint32_t amount_ml ){
  if ( pump_is_running ){
    term.debug("allready running");
    return false;
  }
  if ( valve < 1 || valve > 6 ){
    term.debug("valve number is not valid");
    return false;
  }

}
