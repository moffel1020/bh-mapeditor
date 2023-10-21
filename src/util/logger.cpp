#include "logger.h"
#include <raylib.h>

// extremely advanced logging
void Logger::info(std::string text) { std::cout << "[INFO] : " << text << std::endl; }

void Logger::error(std::string text) { std::cout << "[ERROR] : " << text << std::endl; }

void Logger::warn(std::string text) { std::cout << "[WARN] : " << text << std::endl; }

void Logger::rlLog(int msgType, const char* text, va_list args) {
    switch (msgType) {
    case LOG_INFO:
        std::cout << "[INFO] : ";
        break;
    case LOG_ERROR:
        std::cout << "[ERROR] : ";
        break;
    case LOG_WARNING:
        std::cout << "[WARN] : ";
        break;
    case LOG_DEBUG:
        std::cout << "[DEBUG] : ";
        break;
    default:
        break;
    }

    vprintf(text, args);
    printf("\n");
}