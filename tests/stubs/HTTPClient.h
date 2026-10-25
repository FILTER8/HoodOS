#pragma once
#include "Arduino.h"
#include "WiFiClientSecure.h"
#define HTTP_CODE_OK 200
struct HTTPClient {
 void setConnectTimeout(int){} void setTimeout(int){}
 bool begin(WiFiClientSecure&,const String&){return true;}
 void addHeader(const char*,const char*){} int GET(){return 200;}
 String getString(){return {};}
 void end(){}
};
