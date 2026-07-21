#ifndef TIME_UTILS_H
#define TIME_UTILS_H

#include <Arduino.h>
#include <time.h>

inline void beginTimeSync() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

inline String currentIsoTimestamp() {
  time_t now = time(nullptr);
  if (now < 8 * 3600 * 2) {
    return "";
  }
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(buf);
}

#endif // TIME_UTILS_H