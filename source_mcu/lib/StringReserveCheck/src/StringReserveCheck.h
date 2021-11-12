#ifndef STRING_RESERVE_CHECK_H
#define STRING_RESERVE_CHECK_H

/**
  StringReserveCheck.h
  by Matthew Ford
  (c)2021 Forward Computing and Control Pty. Ltd.
  This code is not warranted to be fit for any purpose. You may only use it at your own risk.
  This code may be freely used for both private and commercial use.
  Provide this copyright is maintained.
*/
/***
  Usage:
  NOTE: check for free memory will check that 25% free SRAM is available UNO and Mega2560 and ~2K bytes for all other boards.

  Ex1 ============================================
  String someGlobalString;
  StringReserveCheck someGlobalStringCheck;  // declare just after String so it has the same scope and life cycle
  . . .
  void setup() {
  . . .
  someGlobalString.reserve(100);
  someGlobalStringCheck.init(someGlobalString,Serial); // check available free memory and remember where the String buffer is. Messages will be printed to Serial
  . . .
  }

  void someMethod() {
    someGlobalStringCheck += " some extra text";
    // when you want to check if reserve was large enough
    someGlobalStringCheck.checkReserve();
  }

  void loop() {
  . .
  someMethod();
  . . .
  }

  Ex2 =============================================
  // to check a method local String
  void someLocalStringMethod() {
    String localString;
    StringReserveCheck localStringCheck; // same life cycle as localString
    localString.reserve(50);
    localStringCheck.init(localString,Serial);  // check available free memory and remember where the String buffer is. Messages will be printed to Serial when method returns
    . . .

   }  // here localString and localStringCheck are both discarded and localStringCheck will check if reserve was large enough and a msg printed
   
  Ex3 =============================================
  // to suppress output msgs use init(str)
  void someLocalStringMethod() {
    String localString;
    StringReserveCheck localStringCheck; // same life cycle as localString
    localString.reserve(50);
    if (!localStringCheck.init(localString)) {  // check available free memory and remember where the String buffer is. Suppress messages
      Serial.println("low free memory");
    }
    . . .
    if (!localStringCheck.checkReserve()) {
      Serial.println("localString was moved on the heap");
    }

   }  // here localString and localStringCheck are both discarded no msgs printed since a output stream was not specified in init( )
   
*/

#include <Arduino.h>

class StringReserveCheck {
  public:
  	//  StringReserveCheck strCheck; or  StringReserveCheck strCheck(true); to detect any increase above the reserve
  	// use StringReserveCheck  strCheck(false); to only check if buffer moved leaving a 'hole'  
    explicit StringReserveCheck(bool strict=true);

    // initializes StringReserveCheck with the current String state and optionally assigns an output stream for messages
    // returns false if memory low, ie < 25% free memory on UNO or MEGA2560 OR < ~2K free on all other boards, else returns true
    bool init(String & str);
    bool init(String & str, Print& output); // assign output here in init so it can be used in ~StringReserveCheck()

    // returns true if the String's buffer was not moved on the heap
    // prints a message as well if an output stream was specified in init
    bool checkReserve();

    // checkReserve and if an output stream was specified in init, prints a message, otherwise just cleans up
    ~StringReserveCheck();
  private:
  	bool strictCheck;
  	void *checkPtr;
    bool init(String & str, Print* _outPtr);
    String *strPtr;
    const char *initBufferPtr;
    Print *outPtr;
};

#endif
