#pragma once

class Logger {
  public:
  Logger(const String &tag) { init(tag); }

  ~Logger() { Serial.println(mBuffer); }

  Logger &operator<<(const String &s) { mBuffer += " " + s; return *this; }
  Logger &operator<<(const int number) { mBuffer += " " + String(number); return *this; }
  Logger &operator<<(void *p) { mBuffer += " " + String((uint16_t)(p), 16); return *this; }
  
private:
  void init(const String &tag) { mBuffer = tag + ": "; }
  String mBuffer;
};