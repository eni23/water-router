#include <Arduino.h>
#include "config.h"
#include "serial.h"
#include "water.h"



void setup() {
  Serial.begin(19200);
  Serial1.begin(SERIAL1_BAUD);
  Serial1.setTimeout(50);
  water_init();
}


void loop() {
  serial_loop();
}
