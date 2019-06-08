#include <Arduino.h>
#include "config.h"
#include "water.h"


const uint8_t VALVES[VALVE_COUNT] = { GPIO_VALVES };
void set_null();

bool pump_is_running = false;
uint8_t current_active_valve = 0;
uint32_t pump_run_until = 0;
uint32_t flow_counter;

void water_init(){
  pinMode( GPIO_FLOWMETER, INPUT_PULLUP );
  pinMode( GPIO_PUMP, OUTPUT );
  attachInterrupt(digitalPinToInterrupt(GPIO_FLOWMETER), inc_flowcount, RISING);
  for ( uint8_t i=0; i<VALVE_COUNT; i++){
    pinMode(VALVES[i], OUTPUT );
  }
  set_null();
}

void set_null(){
  for ( uint8_t i=0; i< VALVE_COUNT; i++){
    digitalWrite(VALVES[i], 1);
  }
  digitalWrite(GPIO_PUMP, 0);
}

void inc_flowcount() {
  flow_counter++;
  if (flow_counter>=pump_run_until){
    stop_water();
  }
}


void stop_water(){
  set_null();
  pump_is_running = false;
  flow_counter = 0;
}


bool water(uint8_t valve, uint32_t amount_ml ){
  if ( pump_is_running ){
    return false;
  }
  if ( valve < 1 || valve > (VALVE_COUNT+1) ){
    return false;
  }

  // 4550 impulses/liter
  //pump_run_until = (amount_ml * 4.55);

  // 1137 impulses/liter
  pump_run_until = (amount_ml * 1.11);
  if (amount_ml<25){
    pump_run_until+=3;
  } else if (amount_ml<50){
    pump_run_until+=2;
  } else if (amount_ml>100){
    int corr_min  = ((amount_ml/200)*10);
    Serial.print("corr minus=");
    Serial.println(corr_min);
    pump_run_until-=corr_min;
  }

  // valve mapped from port 2 to 8 (1=pump, 2=reserved)
  current_active_valve = valve;

  // reset all valves to zero
  set_null();
  // enable correct valve
  digitalWrite(VALVES[valve-1], 0);
  // turn on pump
  digitalWrite(GPIO_PUMP, 1);

  pump_is_running = true;

  return true;
}
