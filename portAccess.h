#ifndef PORTACCESS_H_NI7H7SVU
#define PORTACCESS_H_NI7H7SVU


#include <inttypes.h>
#include <avr/io.h>

#define PORT(x) (x)
#define PIN(x) *((volatile uint8_t*)((&x)+1))
#define DDR(x) *((volatile uint8_t*)((&x)+2))


#endif /* end of include guard: PORTACCESS_H_NI7H7SVU */


