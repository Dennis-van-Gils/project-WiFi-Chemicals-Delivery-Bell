/*
  bufferOverflow_ex1.
  Example of Buffer Overflow on UNO / ESP8266 / ESP32 using char[] and strcat
  on UNO this appears to work, on ESP8266 str2 is cleared, on ESP32 it causes a reboot
  
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

void appendCharsTo(char* strIn) {
  // should really check the bounds here but..
  // i) cannot tell from char* how big the char[] is
  // ii) no one ever seems to add the bounds checking code anyway.
  strcat(strIn, " some more text");
  Serial.print(" appendCharsTo returns:");  Serial.println(strIn);
}
void loop() {
  Serial.println("--------- start of loop()");
  char str1[24] = "some str1"; // allow extra space for strcat 
  char str2[] = "some str2 other text";
  appendCharsTo(str1);
  Serial.print("str1:");   Serial.println(str1);
  Serial.print("str2:");   Serial.println(str2);
  Serial.println("--------- end of loop()");
}
