#ifndef CONFIG_H_5N8HUBME
#define CONFIG_H_5N8HUBME

#include <avr/io.h>


//generic definitions
#define STEPPER_TYPE_GEN3 0
#define EXTRUDER_TYPE_GEN3 0


#if defined(__AVR_ATmega1280__)


//feature definitions
#define USE_RS485
#define USE_STEPPERS
#define USE_EXTRUDER_TOOL
#define USE_SDCARD
#define USE_LCD
#define USE_PSU

//port/pin  definitions go here


#ifdef USE_RS485
#define RS485_PORT_IE PORTD
#define RS485_PORT_OE PORTD
#define RS485_PIN_IE PD0
#define RS485_PIN_OE PD1
#define RS485_UART 1
#define RS485_BAUDRATE 38400
#endif

#ifdef USE_EXTRUDER_TOOL
#define EXTRUDER_TYPE EXTRUDER_TYPE_GEN3
#endif


#ifdef USE_STEPPERS
#define STEPPER_TYPE STEPPER_TYPE_GEN3

//#define STEPPERX_DISABLE_INACTIVE
//#define STEPPERY_DISABLE_INACTIVE
#define STEPPERZ_DISABLE_INACTIVE

//currently stepper has all pins in same port
#define STEPPERX_PORT PORTE
#define STEPPERY_PORT PORTK
#define STEPPERZ_PORT PORTA

#define STEPPERX_STEP_PIN PE2
#define STEPPERX_DIR_PIN PE3
#define STEPPERX_EN_PIN PE4
#define STEPPERX_MIN_PIN PE5
#define STEPPERX_MAX_PIN PE6

#define STEPPERY_STEP_PIN PK0
#define STEPPERY_DIR_PIN PK1
#define STEPPERY_EN_PIN PK2
#define STEPPERY_MIN_PIN PK3
#define STEPPERY_MAX_PIN PK4

#define STEPPERZ_STEP_PIN PA0
#define STEPPERZ_DIR_PIN PA1
#define STEPPERZ_EN_PIN PA2
#define STEPPERZ_MIN_PIN PA3
#define STEPPERZ_MAX_PIN PA4

#endif

#ifdef USE_SDCARD

#endif

#ifdef USE_LCD
#define LCD_UART 2
#endif


#ifdef USE_PSU
#define PSU_PORT PORTC
#define PSU_PIN PC0
#endif


#else

#error Unknown CPU

#endif

#endif /* end of include guard: CONFIG_H_5N8HUBME */
