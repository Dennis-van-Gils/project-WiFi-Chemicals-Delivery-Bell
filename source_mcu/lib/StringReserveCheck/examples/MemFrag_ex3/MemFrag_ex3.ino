/*
  MemFrag_ex3.
  Example of Using StringReserveCheck to detect fragementation and out-of-memory
  
  by Matthew Ford
  Copyright(c)2021 Forward Computing and Control Pty. Ltd.
  This example code is in the public domain.

  www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html
*/

// Download the StringReserveCheck.zip file
// https://www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/StringReserveCheck.zip
// and install it using the IDE Sketch → Include Library → Add .ZIP library...
#include <StringReserveCheck.h>

String string1;
StringReserveCheck string1Check;
String string2;
StringReserveCheck string2Check;
String string3;
StringReserveCheck string3Check;

void setup() {
  Serial.begin(9600);
  for (int i = 10; i > 0; i--) {
    Serial.print(i); Serial.print(' ');
    delay(500);
  }
  Serial.println();
  Serial.println(F("Memory Fragementation Example 3"));
  string1.reserve(32);
  string1Check.init(string1); // checkReserve( ) will not print any msgs, you need to check its return value
  string2.reserve(32);
  string2Check.init(string2, Serial); // init( ) will print a msg if memory is low and checkReserve( ) will print a msg
  if (!string3.reserve(32)) { // check the last largest reserve
    while (1) { // stop here and print repeating msg
      Serial.println(F("Strings out-of-memory"));
      delay(3000); // repeat msg every 3 sec
    }
  }
  if (!string3Check.init(string3, Serial)) {    // check return for low memory
    Serial.println(F("Memory Low after reserves()"));
  }
  string1 = F("string1 initial value");
  string3 = F("string3 initial value");
}

void printDegC(float value, String& result) {
  String title = F("This is the temperature in the boiler room ");
  String units = F("degC");
  formatResult(title, units, value, result);
}
void formatResult(const String& title, const String& units, float value, String& result) {
  result = title;
  result += String(value, 1); //temp to 1 decimal only
  result += units;
}

void loop() {
  float temp = 27.35;
  printDegC(temp, string2);
  Serial.println(string2);
  string1Check.checkReserve(); // init(string1,Serial); was specified so msg printed
  if (!string2Check.checkReserve()) { // check return,  init(string2,Serial); was specified so msg also printed
    Serial.print(F("string2 reserve too small. Current length():")); Serial.println(string2.length());
  }
  if (!string3Check.checkReserve()) { // init(string3); // no output specified, check return
    Serial.print(F("string3 reserve too small. Current length():")); Serial.println(string3.length());
  }
  Serial.println();
}
