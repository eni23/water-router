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
#include <Arduino.h>
#include "led.h"
#include "eeprom.h"
#include "term.h"
#include "wifi.h"



void push_button(){
  led_off();
  led_blink_stop();
  term.debug("BTN pushed");
  led_blink_times(RgbColor(0,100,0), 100, 2);
}


/**
 * arduino-init function
 **/
void setup() {

  //pinMode( GPIO_FLOWMETER, INPUT_PULLUP );
  //attachInterrupt(GPIO_FLOWMETER, inc_flowcount, RISING);

  pinMode( GPIO_BUTTON, INPUT_PULLUP);
  attachInterrupt( GPIO_BUTTON, push_button, RISING);
  sei();

  led_init();

  eeprom_init();
  delay( 50 );
  // load settings from eeprom to struct eeprom_config
  eeprom_load_config();
  init_term();

  // connect to wifi
  wifi_init();
  //delay(1000);
  //server_setup();

}


/**
 * arduino-main entry point after setup()
 **/


void loop() {
  // if wifi isn't connected, reconnect. non-blocking, re-try every 20sec
  wifi_reconnect_loop();

  //server->handleClient();
  delay(1);
  timer.run();
  term.run();
}
