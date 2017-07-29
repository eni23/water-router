#ifndef __CONFIG_H
  #define __CONFIG_H

  // wiring
  #define GPIO_FLOWMETER    D6  // (D6)

  // i2c port expander
  #define PCF8574_SDA       D2
  #define PCF8574_SCL       D1
  #define PCF8574_I2CADDR   32
  #define PCF8574_PORT_PUMP 0

  //mic5891
  #define MIC_OUTPUT_ENABLE 2
  #define MIC_STROBE        3
  #define MIC_CLOCK         4
  #define MIC_DATA          5


  // button & led
  #define GPIO_WS2811       D7
  #define GPIO_BUTTON       D8


  // serial port baudrate
  #define SERIAL_BAUD 115200
  // maximal serial input line length
  #define SERIAL_INPUT_BUFFER_SIZE 256
  // configuration version (required to locate config in eeprom)
  #define CONFIG_VERSION "003"
  // start address for config-struct in eeprom
  #define CONFIG_START 64

#endif
