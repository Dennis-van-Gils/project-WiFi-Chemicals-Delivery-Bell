/*
  bufferOverflow_ex2.
  Example of Avoiding Buffer Overflow on UNO / ESP8266 / ESP32 using Arduino Strings
  
  by Matthew Ford
  Copyright(c)2021 Forward Computing and Control Pty. Ltd.
  This example code is in the public domain.

  www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html
*/

void setup() {
  Serial.begin(9600);
  for (int i = 10; i > 0; i--) {
    Serial.print(i); Serial.print(' ');
    delay(500);
  }
  Serial.println();
}
void appendCharsTo(String & strIn) {
  strIn += " some more text";
  Serial.print(" appendCharsTo returns:");  Serial.println(strIn);
}
void loop() {
  Serial.println("--------- start of loop()");
  String str1 = "some str1";  // Note: declaring Strings in the loop() is not recommended as it can lead to memory fragmentation
  String str2 = "some str2 other text";  
  appendCharsTo(str1);
  Serial.print("str1:");   Serial.println(str1);
  Serial.print("str2:");   Serial.println(str2);
  Serial.println("--------- end of loop()");
}
