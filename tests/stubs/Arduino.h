#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <algorithm>
#include <chrono>
class String {
public:
  std::string s;
  String():s(){}
  String(const char *v):s(v?v:""){}
  String(char c):s(1,c){}
  String(const std::string &v):s(v){}
  String(int n):s(std::to_string(n)){}
  String(unsigned int n):s(std::to_string(n)){}
  String(uint64_t n):s(std::to_string(n)){}
  size_t length() const{return s.size();}
  int indexOf(const String& sub,size_t pos=0) const {auto n=s.find(sub.s,pos);return n==std::string::npos?-1:int(n);}
  int indexOf(char c,size_t pos=0) const {auto n=s.find(c,pos);return n==std::string::npos?-1:int(n);}
  String substring(size_t a,size_t b)const{return String(s.substr(a,b-a));}
  int toInt() const{return atoi(s.c_str());}
  char operator[](size_t i) const{return s[i];}
  const char *c_str() const{return s.c_str();}
  operator std::string() const{return s;}
};
inline String operator+(String a,const String &b){return String(a.s+b.s);}
inline String operator+(String a,const char *b){return String(a.s+b);}
inline String operator+(String a,char c){return String(a.s+std::string(1,c));}
inline bool operator==(const String&a,const String&b){return a.s==b.s;}
inline bool operator!=(const String&a,const String&b){return a.s!=b.s;}
inline bool operator==(const String&a,const char *b){return a.s==b;}
inline bool operator!=(const String&a,const char *b){return a.s!=b;}
template<typename T> inline T max(T a,T b){return std::max(a,b);}
template<typename T> inline T min(T a,T b){return std::min(a,b);}
struct SerialProxy { template<typename ...T> void printf(const char*,T...){} void println(const char*){} };
inline SerialProxy Serial;
struct ESPProxy { unsigned getFreePsram()const{return 500000;} unsigned getFreeHeap()const{return 190000;} };
inline ESPProxy ESP;
inline bool psramFound(){return true;}
inline unsigned millis(){return 10000;}
#define PROGMEM
