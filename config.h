#ifndef CONFIG_H_5N8HUBME
#define CONFIG_H_5N8HUBME


//generic definitions
#define STEPPER_TYPE_GEN3 0
#define EXTRUDER_TYPE_GEN3 0


#if defined(__AVR_ATmega1280__)


//feature definitions
#define USE_RS485
#define USE_STEPPERS
#define USE_EXTRUDER


//port/pin  definitions go here


#ifdef USE_RS485
#define RS485_PORT_IE
#define RS485_PORT_IE
#define RS485_PIN_IE
#define RS485_PIN_OE
#endif

#ifdef USE_EXTRUDER
#define EXTRUDER_TYPE EXTRUDER_TYPE_GEN3
#endif


#ifdef USE_STEPPERS
#define STEPPER_TYPE STEPPER_TYPE_GEN3

#define STEPPERX_DIR_PORT
#define STEPPERX_EN_PORT
#define STEPPERX_STEP_PORT
#define STEPPERX_MIN_PORT
#define STEPPERX_MAX_PORT

#define STEPPERX_DIR_PIN
#define STEPPERX_EN_PIN
#define STEPPERX_STEP_PIN
#define STEPPERX_MIN_PIN
#define STEPPERX_MAX_PIN

#define STEPPERY_DIR_PORT
#define STEPPERY_EN_PORT
#define STEPPERY_STEP_PORT
#define STEPPERY_MIN_PORT
#define STEPPERY_MAX_PORT

#define STEPPERY_DIR_PIN
#define STEPPERY_EN_PIN
#define STEPPERY_STEP_PIN
#define STEPPERY_MIN_PIN
#define STEPPERY_MAX_PIN

#define STEPPERZ_DIR_PORT
#define STEPPERZ_EN_PORT
#define STEPPERZ_STEP_PORT
#define STEPPERZ_MIN_PORT
#define STEPPERZ_MAX_PORT

#define STEPPERZ_DIR_PIN
#define STEPPERZ_EN_PIN
#define STEPPERZ_STEP_PIN
#define STEPPERZ_MIN_PIN
#define STEPPERZ_MAX_PIN

#endif




#else

#error Unknown CPU

#endif

#endif /* end of include guard: CONFIG_H_5N8HUBME */
