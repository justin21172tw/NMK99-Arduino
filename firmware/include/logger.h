#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

namespace Logger {
enum class Level { INFO, WARNING, ERROR };

inline const char* ToText(Level level) {
  switch (level) {
    case Level::INFO:
      return "INFO";
    case Level::WARNING:
      return "WARNING";
    case Level::ERROR:
      return "ERROR";
  }
  return "INFO";
}

inline void Begin(unsigned long baud_rate) {
  Serial.begin(baud_rate);
}

inline void Log(Level level, const String& message) {
  Serial.printf("[%s] %s\n", ToText(level), message.c_str());
}

inline void Info(const String& message) { Log(Level::INFO, message); }
inline void Warning(const String& message) { Log(Level::WARNING, message); }
inline void Error(const String& message) { Log(Level::ERROR, message); }
}  // namespace Logger

#endif
