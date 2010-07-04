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

#define DEBUG_ON() PINB|=_BV(PB0)  //PIN(DEBUG_PORT) |= _BV(DEBUG_PIN);
#define DEBUG_OFF() PINB&=~_BV(PB0) //PIN(DEBUG_PORT) &= ~_BV(DEBUG_PIN);

#ifdef DEBUG
#include <stdio.h>
extern UART uart3;

#define DEBUG_OUT(s) { for (uint8_t __x=0;s[__x];__x++) uart3.send(s[__x]); }
#define DEBUG_OUTB(b) { uart3.send(b); }
#define DEBUG_OUTF(f,args...) { char _buf[256]; sprintf(_buf, f,## args); DEBUG_OUT(_buf); }
#else
#define DEBUG_OUT(s)
#define DEBUG_OUTB(b)
#define DEBUG_OUTF(f,args...)
#endif

#endif /* end of include guard: UTILS_H_RFF2FPSB */
