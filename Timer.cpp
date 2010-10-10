

#include <avr/io.h>
#include <util/atomic.h>
#include "Timer.h"
#include "utils.h"
namespace {

#define CS0 CS00
#define CS1 CS01
#define CS2 CS02
#define TOIE TOIE0
	
#if defined(__AVR_ATmega1280__) //mega
	volatile uint8_t* timerTCCRnA[] = { &TCCR0A, &TCCR1A, &TCCR2A,&TCCR3A, &TCCR4A, &TCCR5A };
	volatile uint8_t* timerMSK[] = {&TIMSK0, &TIMSK1, &TIMSK2, &TIMSK3, &TIMSK4, &TIMSK5 };

	const uint16_t prescalers[] = { 1,8,64,256,1024 };
	const uint8_t prescalerFlags[] = { _BV(CS0), _BV(CS1), _BV(CS0)|_BV(CS1), _BV(CS2), _BV(CS2)|_BV(CS0) };
	Timer* timers[6];
#elif defined(__AVR_ATmega644P__) //sanguino
	volatile uint8_t* timerTCCRnA[] = { &TCCR0A, &TCCR1A,&TCCR2A };
	volatile uint8_t* timerMSK[] = {&TIMSK0, &TIMSK1,&TIMSK2 };

	const uint16_t prescalers[] = { 1,8,64,256,1024 };
	const uint8_t prescalerFlags[] = { _BV(CS0), _BV(CS1), _BV(CS0)|_BV(CS1), _BV(CS2), _BV(CS2)|_BV(CS0) };
	Timer* timers[6];
#endif

}

#define TIMSK *((volatile uint8_t*)(timerMSK[mTimerNum]))





Timer::Timer(uint8_t timer,uint32_t hz)
 : mTimerNum(timer), mHZ(hz), mpCB(0), mTCNT(0), mPrescaler(0), mpUserdata(0)
{
	setIOMap();

	timers[mTimerNum] = this;
	disable();
	setupPrescalerAndTCNT();
	setPrescaler();
}

void Timer::setIOMap()
{
	mpIO = (TimerIOMap*)timerTCCRnA[mTimerNum];
	
}

void Timer::enable()
{
	TIMSK = _BV(TOIE);
}

void Timer::disable()
{
	TIMSK = 0;
}

void Timer::reset()
{
//	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	uint8_t s = SREG;
	cli();
	{
		mpIO->TCNTH = mTCNT>>8;
		mpIO->TCNTL = mTCNT;
	}
	SREG = s;
}

void Timer::setPrescaler()
{
	mpIO->TCCRB = prescalerFlags[mPrescaler];
}

void Timer::setupPrescalerAndTCNT()
{
	
	for (int i=0;i<sizeof(prescalers)/sizeof(uint16_t);i++)
	{
		uint32_t t = F_CPU/prescalers[i]/mHZ;
		if (t < 0xffff)
		{
			mTCNT = 0xffff-t;
			mPrescaler = i;
			return;
		}
	}
}

void Timer::doISR()
{
	if (mpCB)
		mpCB((void*)mpUserdata);
		
	mpIO->TCNTH = mTCNT>>8;
	mpIO->TCNTL = mTCNT;
}


//----------------------------------------------------------
//8 bit timer
//----------------------------------------------------------
Timer8::Timer8(uint8_t timer,uint32_t hz)
 : Timer(timer,hz)
{
}

void Timer8::setIOMap()
{
	mpIO8 = (TimerIOMap8*)timerTCCRnA[mTimerNum];
}


void Timer8::reset()
{
//	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	uint8_t s = SREG;
	cli();
	{
		mpIO8->TCNT = mTCNT;
	}
	SREG=s;
}

void Timer8::setPrescaler()
{
	mpIO8->TCCRB = prescalerFlags[mPrescaler];
}


void Timer8::setupPrescalerAndTCNT()
{
	for (int i=0;i<sizeof(prescalers)/sizeof(uint16_t);i++)
	{
		uint32_t t = F_CPU/prescalers[i]/mHZ;
		if (t < 0xff)
		{
			mTCNT = 0xff-t;
			mPrescaler = i;
			return;
		}
	}
}

void Timer8::doISR()
{
	if (mpCB)
		mpCB((void*)mpUserdata);
		
	mpIO8->TCNT = mTCNT;
}

//----------------------------------------
ISR(TIMER0_OVF_vect)
{
	timers[0]->doISR();
}

ISR(TIMER1_OVF_vect)
{
	timers[1]->doISR();
}

ISR(TIMER2_OVF_vect)
{
	timers[2]->doISR();
}

#if defined(__AVR_ATmega1280__) //mega
ISR(TIMER3_OVF_vect)
{
	timers[3]->doISR();
}

ISR(TIMER4_OVF_vect)
{
	timers[4]->doISR();
}
ISR(TIMER5_OVF_vect)
{
	timers[5]->doISR();
}
#endif


