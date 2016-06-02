/*******
 *
 */


// gets triggered when GPIO_FLOWMETER is rising
// 4550 pulses every liter
void inc_flowcount() {
  flow_counter++;
  debug("inc_flowcount");
  if (flow_counter>=pump_run_until){
    stop_water();
  }
}


void stop_water(){
  pcf8574_null();
  pcf8574_set_port(PCF8574_PORT_PUMP, false);
  pcf8574_set_port(current_active_valve, false);
  pcf8574_update();
  debug("watering done");
  pump_is_running = false;
  flow_counter = 0;
  led_blink_stop();
}

// valve: valve starting with 1, highest 6
// amount_ml: how much water in mililitres
boolean water(uint8_t valve, uint32_t amount_ml ){

  if ( pump_is_running ){
    debug("allready running");
    return false;
  }
  if ( valve < 1 || valve > 6 ){
    debug("valve number is not valid");
    return false;
  }


  // 4550 impulses/litre
  pump_run_until = (amount_ml * 4.55);
  // valve mapped from port 2 to 8 (1=pump, 2=reserved)
  current_active_valve = valve;

  if (eeprom_config.debug_output_enabled){
    Serial.print("start routing, valve=");
    Serial.print(valve);
    Serial.print(", mililitres=");
    Serial.print(amount_ml);
    Serial.print(", pulses=");
    Serial.println(pump_run_until);
  }

  // send command to port expander
  pcf8574_null();
  pcf8574_set_port(PCF8574_PORT_PUMP, true);
  pcf8574_set_port(current_active_valve, true);
  pcf8574_update();
  pump_is_running = true;

  led_blink(led_color_blue, 500);

  return true;
}
