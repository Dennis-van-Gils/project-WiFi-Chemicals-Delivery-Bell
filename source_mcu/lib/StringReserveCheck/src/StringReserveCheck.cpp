/**
  StringReserveCheck.cpp
  by Matthew Ford
  (c)2021 Forward Computing and Control Pty. Ltd.
  This code is not warranted to be fit for any purpose. You may only use it at your own risk.
  This code may be freely used for both private and commercial use.
  Provide this copyright is maintained.
*/

#include "StringReserveCheck.h"

// use StringReserveCheck  strCheck(true); to detect any increase above the reserve
//  StringReserveCheck strCheck; or  StringReserveCheck strCheck(false); to only check if buffer moved leaving a 'hole' 
StringReserveCheck::StringReserveCheck(bool strict) {
  strPtr = NULL;
  initBufferPtr = NULL;
  outPtr = NULL;
  strictCheck = strict;
  checkPtr = NULL;
}

// initializes StringReserveCheck with the current String state and optionally assigns an output stream for messages
// returns false if memory low, ie < 25% free memory on UNO or MEGA2560 OR < ~2K free on all other boards, else returns true
bool StringReserveCheck::init(String & str) {
  return init(str, NULL);
}
bool StringReserveCheck::init(String & str, Print& out) {
  return init(str, &out);
}

bool StringReserveCheck::init(String & str, Print* _outPtr) {
  outPtr = _outPtr;
  bool rtn = true;
  if (str.c_str() == NULL) {
    if (outPtr) {
      outPtr->println(F("!! Out-Of-Memory when resizing String"));
    }
    rtn = false;
  }
  // check free memory after reserve
  // 330 for UNO ,  1024 for MEGA2650 and 1024 for all others
#if defined(ARDUINO_AVR_MEGA2560)
  void *mem = malloc(1024); // MEGA2650 
#elif defined(ARDUINO_ARCH_AVR)
  void *mem = malloc(330);
#else
  void *mem = malloc(1024); // others 1024
#endif
  if (mem == NULL) {
    if (outPtr) {
      outPtr->println(F("!! Low memory available, stability problems may occur."));
    }
    rtn = false;
  } else {
    free(mem); // rtn true above
  }
  if (strictCheck) {
  	  checkPtr = malloc(2); 
  }
  // else
  strPtr = &str;
  // outPtr set at top.
  initBufferPtr = str.c_str();
  return rtn;
}


// returns true if the String's buffer was not moved on the heap
// prints a message as well if an output stream was specified in init
bool StringReserveCheck::checkReserve() {
  bool rtn = true;
  bool notMovedMsg = false;
  if (strPtr && initBufferPtr) {
    const char* cstr = strPtr->c_str();
    if (cstr == NULL) {
      if (outPtr) {
        outPtr->println(F("!! Out-Of-Memory when resizing String"));
      }
      rtn = false;
    } else {
      if (cstr == initBufferPtr) {
      	if (strictCheck) {
          if (outPtr) {
            outPtr->print(F("reserve() large enough for String '"));
          }
        } else {
        	notMovedMsg = true;
        }
      } else {
        if (outPtr) {
          outPtr->print(F("!! reserve() NOT large enough for String '"));
        }
        rtn = false;
      }
      size_t len = strPtr->length();
      bool dots = false;
      if (len > 10) {
        len = 10;
        dots = true;
      }
      if (notMovedMsg) {
        if (outPtr) {         	  
            outPtr->print(F("?? reserve() for String '")); 
        }
      }
      if (outPtr) {
        outPtr->write(cstr, len);
        if (dots) {
          outPtr->print(F("..."));
        }
        outPtr->print(F("' Current len:"));  outPtr->print(strPtr->length());
      }
      if (notMovedMsg) {
        if (outPtr) {         	  
            outPtr->print(F(" not moved, but may have been enlarged. Use strict option to check.")); 
        }
      }      
      if (outPtr) {
      	outPtr->println();
      }
    }
  }
  return rtn;
}

// checkReserve and if an output stream was specified in init, prints a message, otherwise just cleans up
StringReserveCheck::~StringReserveCheck() {
  checkReserve();
  free(checkPtr);
}
