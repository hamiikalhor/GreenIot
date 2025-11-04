/**
 * @file Arduino_stub.h
 * @brief Minimal Arduino API Stubs for Native Testing
 * 
 * Provides minimal Arduino function stubs for native (PC) testing
 * 
 * @author GreenIoT Vertical Farming Project
 * @date 2025-11-04
 */

#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#ifdef NATIVE_BUILD

#include <stdint.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Arduino time functions
static inline uint32_t millis() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

static inline uint32_t micros() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000000) + tv.tv_usec);
}

static inline void delay(uint32_t ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

static inline void delayMicroseconds(uint32_t us) {
    struct timespec ts;
    ts.tv_sec = us / 1000000;
    ts.tv_nsec = (us % 1000000) * 1000;
    nanosleep(&ts, NULL);
}

#ifdef __cplusplus
}
#endif

#endif // NATIVE_BUILD

#endif // ARDUINO_STUB_H

