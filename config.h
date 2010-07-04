#ifndef CONFIG_H_5N8HUBME
#define CONFIG_H_5N8HUBME

#include <avr/io.h>

//generic definitions
#define STEPPER_TYPE_GEN3 0
#define STEPPER_TYPE_MICROSTEP 1

#define MACHINE_TYPE_GEN3 0
#define MACHINE_TYPE_PAEAE 1

#if defined(__AVR_ATmega1280__)
#define MACHINE_TYPE 1 //paeae
#elif defined(__AVR_ATmega644P__) //sanguino
#define MACHINE_TYPE 0
#else
#error Machine type unsupported
#endif

#define DEBUG
#define DEBUG_FAKE_SLAVE


#define HOST_BAUDRATE 38400
#define HOST_RECEIVE_BUFFER_SIZE 32
#define HOST_REPLY_BUFFER_SIZE 32
#define HOST_SLAVE_BUFFER_SIZE 32
#define HOST_SLAVE_REPLY_BUFFER_SIZE 32
#define COMMAND_BUFFER_SIZE 256

#define HOST_TIMEOUT 1000
#define SLAVE_TIMEOUT 1000

#define CLOCK_TIMER 1


#if MACHINE_TYPE == 1

	#define CLOCK_TIMER 1
	#define STEPPER_TIMER 5
	#define NUM_UARTS 4
	#define HOST_UART 0
	//feature definitions
	#define USE_SDCARD
	#define USE_LCD
	#define USE_PSU

	//port/pin  definitions go here
	#define DEBUG_PORT PORTB
	#define DEBUG_PIN PB0

	#define RS485_PORT_IE PORTD
	#define RS485_PORT_OE PORTD
	#define RS485_PIN_IE PD0
	#define RS485_PIN_OE PD1
	#define RS485_UART 1
	#define RS485_BAUDRATE 38400



	#define STEPPER_TYPE STEPPER_TYPE_GEN3

	//#define STEPPERX_DISABLE_INACTIVE
	//#define STEPPERY_DISABLE_INACTIVE
	#define STEPPERZ_DISABLE_INACTIVE


	#define STEPPERX_STEP_PORT PORTE
	#define STEPPERX_STEP_PIN PE2
	#define STEPPERX_DIR_PORT PORTE
	#define STEPPERX_DIR_PIN PE3
	#define STEPPERX_EN_PORT PORTE
	#define STEPPERX_EN_PIN PE4
	#define STEPPERX_MIN_PORT PORTE
	#define STEPPERX_MIN_PIN PE5
	#define STEPPERX_MAX_PORT PORTE
	#define STEPPERX_MAX_PIN PE6

	#define STEPPERY_STEP_PORT PORTK
	#define STEPPERY_STEP_PIN PK0
	#define STEPPERY_DIR_PORT PORTK
	#define STEPPERY_DIR_PIN PK1
	#define STEPPERY_EN_PORT PORTK
	#define STEPPERY_EN_PIN PK2
	#define STEPPERY_MIN_PORT PORTK
	#define STEPPERY_MIN_PIN PK3
	#define STEPPERY_MAX_PORT PORTK
	#define STEPPERY_MAX_PIN PK4

	#define STEPPERZ_STEP_PORT PORTA
	#define STEPPERZ_STEP_PIN PA0
	#define STEPPERZ_DIR_PORT PORTA
	#define STEPPERZ_DIR_PIN PA1
	#define STEPPERZ_EN_PORT PORTA
	#define STEPPERZ_EN_PIN PA2
	#define STEPPERZ_MIN_PORT PORTA
	#define STEPPERZ_MIN_PIN PA3
	#define STEPPERZ_MAX_PORT PORTA
	#define STEPPERZ_MAX_PIN PA4


	#ifdef USE_SDCARD

	#endif

	#ifdef USE_LCD
	#define LCD_UART 2
	#endif


	#ifdef USE_PSU
	#define PSU_PORT PORTC
	#define PSU_PIN PC0
	#endif


#elif MACHINE_TYPE == 0 //gen3 sanguino

	#define CLOCK_TIMER 1
	#define STEPPER_TIMER 5
	#define NUM_UARTS 2
	#define HOST_UART 0
	
	//feature definitions
	#define USE_SDCARD
//	#define USE_LCD
	#define USE_PSU

	//port/pin  definitions go here
	#define DEBUG_PORT PORTB
	#define DEBUG_PIN PB0


	#define RS485_PORT_IE PORTD
	#define RS485_PORT_OE PORTD
	#define RS485_PIN_IE PD5
	#define RS485_PIN_OE PD4
	#define RS485_UART 1
	#define RS485_BAUDRATE 38400


	#define STEPPER_TYPE STEPPER_TYPE_GEN3

	//#define STEPPERX_DISABLE_INACTIVE
	//#define STEPPERY_DISABLE_INACTIVE
	#define STEPPERZ_DISABLE_INACTIVE

	//currently stepper has all pins in same port
	#define STEPPERX_STEP_PORT PORTD
	#define STEPPERX_STEP_PIN PD7
	#define STEPPERX_DIR_PORT PORTC
	#define STEPPERX_DIR_PIN PC2
	#define STEPPERX_EN_PORT PORTC
	#define STEPPERX_EN_PIN PC3
	#define STEPPERX_MIN_PORT PORTC
	#define STEPPERX_MIN_PIN PC4
	#define STEPPERX_MAX_PORT PORTC
	#define STEPPERX_MAX_PIN PC5

	#define STEPPERY_STEP_PORT PORTC
	#define STEPPERY_STEP_PIN PC7
	#define STEPPERY_DIR_PORT PORTC
	#define STEPPERY_DIR_PIN PC6
	#define STEPPERY_EN_PORT PORTA
	#define STEPPERY_EN_PIN PA7
	#define STEPPERY_MIN_PORT PORTA
	#define STEPPERY_MIN_PIN PA6
	#define STEPPERY_MAX_PORT PORTA
	#define STEPPERY_MAX_PIN PA5

	#define STEPPERZ_STEP_PORT PORTA
	#define STEPPERZ_STEP_PIN PA4
	#define STEPPERZ_DIR_PORT PORTA
	#define STEPPERZ_DIR_PIN PA3
	#define STEPPERZ_EN_PORT PORTA
	#define STEPPERZ_EN_PIN PA2
	#define STEPPERZ_MIN_PORT PORTA
	#define STEPPERZ_MIN_PIN PA1
	#define STEPPERZ_MAX_PORT PORTA
	#define STEPPERZ_MAX_PIN PA0


	#ifdef USE_SDCARD
	#define SDCARD_WRITE_PORT PORTB
	#define SDCARD_WRITE_PIN PB2
	#define SDCARD_DETECT_PORT PORTB
	#define SDCARD_DETECT_PIN PB3
	#define SDCARD_SELECT_PORT PORTB
	#define SDCARD_SELECT_PIN PB4
	#endif

	#ifdef USE_LCD
	#define LCD_UART 2
	#endif


	#ifdef USE_PSU
	#define PSU_PORT PORTD
	#define PSU_PIN PD6
	#endif



#else

#error Unknown CPU

#endif

#endif /* end of include guard: CONFIG_H_5N8HUBME */
