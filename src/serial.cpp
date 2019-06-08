#include <Arduino.h>
#include "config.h"
#include "serial.h"
#include "water.h"



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

void parse_serial_msg(){
  String cmd = str_token(serial_data, ' ', 0);
  int s_addr = intArg(1);

  if (s_addr != ADDRESS){
      Serial.println("not my address");
      return;
  }

  if (cmd == "WTR1"){

    int s_valve = intArg(2);
    int s_amount = intArg(3);


    Serial.print("start water, valve=");
    Serial.print(s_valve);
    Serial.print(" amount=");
    Serial.println(s_amount);
    water(s_valve, s_amount);
    return;
  }

  if (cmd == "WTR0"){
    Serial.println("stop water");
    stop_water();
    return;
  }


}


void serial_loop(){
  if (Serial1.available()) {
    serial_data = Serial1.readStringUntil(";");
    parse_serial_msg();
  }
}
