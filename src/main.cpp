#include <Arduino.h>
#include "config.h"
#include "water.h"
#include <TimerOne.h>



// struct serial_wr_message {
//   uint8_t addr;
//   uint8_t valve;
//   uint32_t amount_ml;
//   uint8_t crc;
// };
//
// void send_struct(const serial_wr_message* data){
//   Serial.write((const char*)data, sizeof(serial_wr_message));
// }
//
// bool receive_struct(serial_wr_message* data){
//   return (
//     Serial1.readBytes((char*)data,
//     sizeof(serial_wr_message)) == sizeof(serial_wr_message)
//   );
// }




String serial_data;



/**
* split a string and get chuck at desired position
* example:
* data="foo,bar,baz" separator="," index=2  => baz
* data="foo/bar" separator="/" index=0  => foo
**/
String str_token( String data, char separator, int index ) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length()-1;
  for( int i = 0; i <= maxIndex && found <= index; i++ ){
    if( data.charAt( i ) == separator || i == maxIndex ){
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = ( i == maxIndex ) ? i + 1 : i;
    }
  }
  return found > index ? data.substring( strIndex[0], strIndex[1] ) : "";
}

String strArg(int index){
  return str_token(serial_data, ' ', index);
}

int intArg(int index){
  String s_str = "";
  s_str = strArg( index );
  int s;
  s = s_str.toInt();
  return s;
}




void setup() {
  Serial.begin(19200);
  Serial1.begin(SERIAL1_BAUD);
  Serial1.setTimeout(400);
  water_init();
}


void loop() {
  if (Serial1.available()) {
    serial_data = Serial1.readStringUntil("\0");
    String cmd = str_token(serial_data, ' ', 0);
    if (cmd == "WTR1"){
      int s_valve = intArg(1);
      int s_amount = intArg(2);

      Serial.print("start water, valve=");
      Serial.print(s_valve);
      Serial.print(" amount=");
      Serial.println(s_amount);

      water(s_valve, s_amount);
    }

    if (cmd == "WTR0"){
      Serial.println("stop water");
      stop_water();
    }


  }
}
