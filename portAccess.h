#ifndef PORTACCESS_H_NI7H7SVU
#define PORTACCESS_H_NI7H7SVU


#include <inttypes.h>
#include <avr/io.h>

#define PORT(x) (x)
#define PIN(x) *((volatile uint8_t*)((&x)+1))
#define DDR(x) *((volatile uint8_t*)((&x)+2))

//handles pointer parameter
#define PORTP(x) *(volatile uint8_t*)&PORT(x)
#define PINP(x) *(volatile uint8_t*)&PIN(x)
#define DDRP(x) *(volatile uint8_t*)&DDR(x)


#endif /* end of include guard: PORTACCESS_H_NI7H7SVU */


