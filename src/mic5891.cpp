#include "Arduino.h"

class MIC5891 {

  int pin_data;
  int pin_clock;
  int pin_strobe;
  int pin_enable;

public:

  uint8_t state=0;
  bool enabled;

  /**
  * Constructor
  **/
  MIC5891() {
  }

  // Setup ports and set state to enabled
  void begin(
    const int _pin_data,
    const int _pin_sck,
    const int _pin_str,
    const int _pin_en
  ){
    pin_data   = _pin_data;
    pin_clock  = _pin_sck;
    pin_strobe = _pin_str;
    pin_enable = _pin_en;
    pinMode(pin_enable, OUTPUT);
    disable();
    pinMode(pin_data, OUTPUT);
    pinMode(pin_clock, OUTPUT);
    pinMode(pin_strobe, OUTPUT);
    digitalWrite(pin_strobe, LOW );
    clear();
    update();
    enable();
  }

  // disable outputs
  void disable(){
    digitalWrite(pin_enable, HIGH);
    enabled = false;
  }

  // enable outputs
  void enable(){
    digitalWrite(pin_enable, LOW);
    enabled = true;
  }

  // Set single port
  void set_port(uint8_t portnum, bool state){
    if (state){
      bitSet(state, portnum);
    }
    else {
      bitClear(state, portnum);
    }
  }

  // set all ports
  void set_all(uint8_t in_states){
    state = in_states;
  }

  // Set all ports to disabled state
  void clear(){
    for (uint8_t i = 0; i<8;i++){
      set_port(i, false);
    }
  }

  // Update shift register (send current state)
  void update(){
    for (uint8_t i=0; i<8; i++){
      digitalWrite(pin_data, bitRead(state, i));
      digitalWrite(pin_clock, HIGH);
      digitalWrite(pin_clock, LOW);
      delay(1);
    }
    digitalWrite(pin_strobe, HIGH );
    delay(2);
    digitalWrite(pin_strobe, LOW );
  }




};
