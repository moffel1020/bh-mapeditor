#pragma once
#include <iostream>
#include <string>

class Logger {
  public:
    static void rlLog(int msgType, const char* text, va_list args);
    static void info(std::string text);
    static void error(std::string text);
    static void warn(std::string text);
    static inline bool disableRlLog = false;
};
