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
  pinMode( GPIO_BUTTON, INPUT_PULLUP );
  pinMode( GPIO_PUMP, OUTPUT );
  attachInterrupt(digitalPinToInterrupt(GPIO_FLOWMETER), inc_flowcount, RISING);
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON), btn_isr, FALLING);

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

void btn_isr(){
  if (!pump_is_running){
    return;
  }
  Serial.print("pulses was=");
  Serial.println(flow_counter);
  stop_water();
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

  // 1.098 impulses/liter
  // plus calib value (extra steps to run for hose looses)
  pump_run_until = (amount_ml * 1.098)+PUMP_CAL;

  Serial.print("num_pulses=");
  Serial.println(pump_run_until);

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
