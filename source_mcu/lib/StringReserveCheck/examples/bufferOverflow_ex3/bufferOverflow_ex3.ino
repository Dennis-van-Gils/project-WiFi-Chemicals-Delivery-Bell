/*
  bufferOverflow_ex3.
  Example of Avoiding Buffer Overflow on UNO / ESP8266 / ESP32 using SafeString
  
  by Matthew Ford
  Copyright(c)2021 Forward Computing and Control Pty. Ltd.
  This example code is in the public domain.

  www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html
*/

#include <SafeString.h>
// install the SafeString library V3.1.0+ from Arduino library manager or
// download the zip file from https://www.forward.com.au/pfod/ArduinoProgramming/SafeString/index.html

void setup() {
  Serial.begin(9600);
  for (int i = 10; i > 0; i--) {
    Serial.print(i); Serial.print(' ');
    delay(500);
  }
  Serial.println();
  // SafeString::setOutput(Serial); // uncomment this for detailed error msgs
}

void appendCharsTo(SafeString & strIn) {
  // pass strIn as a reference &
  strIn += " some more text"; // this does all the bounds checks
  Serial.print(" appendCharsTo returns:");  Serial.println(strIn);
}
void loop() {
  Serial.println("--------- start of loop()");
  char str1[24] = "some str1"; // allow extra space for appendCharsTo
  char str2[] = "some str2 other text";
  createSafeStringFromCharArray(sfStr1, str1); // or cSFA(sfStr1,str1); for short.  Wrap str1 in a SafeString
  appendCharsTo(sfStr1);
  if (SafeString::errorDetected()) { // set true if any SafeString has an error.  Use hasError() on each SafeString to narrow it down or use SafeString::setOutput(Serial) for error msgs
    Serial.println(F("Out of bounds error detected in appendCharsTo"));
  }
  Serial.print("str1:");   Serial.println(str1);
  Serial.print("str2:");   Serial.println(str2);
  Serial.println("--------- end of loop()");
}
