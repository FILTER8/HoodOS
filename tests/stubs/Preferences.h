#pragma once
#include "Arduino.h"
struct Preferences {
 bool begin(const char*,bool){return true;}
 unsigned short getUShort(const char*,unsigned short v){return v;}
 size_t putUShort(const char*,unsigned short){return 2;}
 void end(){}
};
