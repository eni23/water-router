#include <Arduino.h>

/*******
 * I2C port expander driver
 *
 * Example:
 *
 * #define PCF8574_SDA     D2 // i2c data gpio pin
 * #define PCF8574_SCL     D1 // i2c clock gpio pin
 * #define PCF8574_I2CADDR 32 // i2c address of module
 * setup_pcf8574()            // init module
 * pcf8574_set_port(0, true)  // set port 1 on
 * pcf8574_set_port(5, false) // set port 5 off
 * pcf8574_update()           // send to the device
 ****/

uint8_t mic5891_state = 0;

void mic_send(uint8_t states){
  for (uint8_t i=0; i<8; i++){
    digitalWrite(MIC_DATA,bitRead(states, i));
    digitalWrite(MIC_CLOCK, HIGH);
    digitalWrite(MIC_CLOCK, LOW);
    delay(1);
  }
  digitalWrite(MIC_STROBE, LOW);
  delay(2);
  digitalWrite(MIC_STROBE, HIGH);
}


// init module
void setup_mic5891(){
  pinMode(MIC_OUTPUT_ENABLE, OUTPUT);
  digitalWrite(MIC_OUTPUT_ENABLE, HIGH);

  pinMode(MIC_STROBE, OUTPUT);
  pinMode(MIC_CLOCK, OUTPUT);
  pinMode(MIC_DATA, OUTPUT);

  // set all to 0
  mic_send(0);
  digitalWrite(MIC_OUTPUT_ENABLE, LOW);

}

// send current state to module
void mic5891_update(){
  for (uint8_t i=0; i<8; i++){
    digitalWrite(MIC_DATA,bitRead(mic5891_state, i));
    digitalWrite(MIC_CLOCK, HIGH);
    digitalWrite(MIC_CLOCK, LOW);
    delay(1);
  }
  digitalWrite(MIC_STROBE, LOW);
  delay(2);
  digitalWrite(MIC_STROBE, HIGH);
}

// update state variable
void mic5891_set_port(uint8_t portnum, boolean state){
  if (state){
    bitSet(mic5891_state, portnum);
  }
  else {
    bitClear(mic5891_state, portnum);
  }
}


// turn off all ports
void mic5891_null(){
  for (uint8_t i = 0; i<8;i++){
    mic5891_set_port(i, false);
  }
}

// read state
boolean mic5891_get_port( uint8_t portnum ){
  uint8_t state = bitRead(mic5891_state, portnum);
  if (state){
    return true;
  }
  else {
    return false;
  }
}
