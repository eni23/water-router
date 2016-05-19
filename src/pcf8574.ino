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

uint8_t pcf8574_state = 0;


// init module
void setup_pcf8574(){
  Wire.begin(PCF8574_SDA, PCF8574_SCL);
  Wire.setClock(1000);
  Wire.beginTransmission(PCF8574_I2CADDR);
  pcf8574_null();
  Wire.write(~pcf8574_state);
  Wire.endTransmission();
}

// send current state to module
void pcf8574_update(){
  Wire.beginTransmission(PCF8574_I2CADDR);
  Wire.write(~pcf8574_state);
  Wire.endTransmission();
}

// update state variable
void pcf8574_set_port(uint8_t portnum, boolean state){
  if (state){
    bitSet(pcf8574_state, portnum);
  }
  else {
    bitClear(pcf8574_state, portnum);
  }
}

// turn off all ports
void pcf8574_null(){
  for (uint8_t i = 0; i<8;i++){
    pcf8574_set_port(i, false);
  }

  //pcf8574_state=0;
}

// read state
boolean pcf8574_get_port( uint8_t portnum ){
  uint8_t state = bitRead(pcf8574_state, portnum);
  if (state){
    return true;
  }
  else {
    return false;
  }
}
