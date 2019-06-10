#ifndef __CONFIG_H
  #define __CONFIG_H

  #define ADDRESS               12

  // wiring
  #define GPIO_BUTTON           7
  #define GPIO_FLOWMETER        3
  #define GPIO_PUMP             5
  #define VALVE_COUNT           4
  #define GPIO_VALVES           15, 14, 16, 10

  // serial baud
  // common values are: 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000 and 256000
  #define SERIAL1_BAUD          57600

  // steps to run extra
  #define PUMP_CAL 2.75

  #define DEGUB_ENABLED         1

#endif
