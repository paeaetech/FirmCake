#ifndef UTILS_H_RFF2FPSB
#define UTILS_H_RFF2FPSB

#include <inttypes.h>
#include "config.h"
#include "portAccess.h"
#include "uart.h"

uint32_t millis();
void clock_init();
void reset();

void clock_callback();

#define DEBUG_ON() PIN(DEBUG_PORT) |= _BV(DEBUG_PIN);
#define DEBUG_OFF() PIN(DEBUG_PORT) &= ~_BV(DEBUG_PIN);

#ifdef DEBUG
extern UART uart3;

#define DEBUG_OUT(s) { for (uint8_t __x=0;s[__x];__x++) uart3.send(s[__x]); }
#define DEBUG_OUTB(b) { uart3.send(b); }
#else
#define DEBUG_OUT(s)
#define DEBUG_OUTB(b)
#endif

#endif /* end of include guard: UTILS_H_RFF2FPSB */
