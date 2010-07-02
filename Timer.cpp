

#include <avr/io.h>
#include <util/atomic.h>
#include "Timer.h"

namespace {
#define CS0 CS00
#define CS1 CS01
#define CS2 CS02
#define TOIE TOIE0
	
	
	volatile uint8_t* timerTCCRnA[6] = { 0, &TCCR1A, 0,&TCCR3A, &TCCR4A, &TCCR5A };
	volatile uint8_t* timerMSK[6] = {0, &TIMSK1, 0, &TIMSK3, &TIMSK4, &TIMSK5 };

	const uint16_t prescalers[] = { 1,8,64,256,1024 };
	const uint8_t prescalerFlags[] = { _BV(CS0), _BV(CS1), _BV(CS0)|_BV(CS1), _BV(CS2), _BV(CS2)|_BV(CS0) };
	Timer* timers[6];
}

#define TIMSK *((volatile uint8_t*)(timerMSK[mTimerNum]))





Timer::Timer(uint8_t timer,uint32_t hz)
 : mTimerNum(timer), mHZ(hz), mpCB(0), mTCNT(0), mPrescaler(0), mpUserdata(0)
{
	mpIO = (TimerIOMap*)timerTCCRnA[mTimerNum];
	timers[mTimerNum] = this;

	disable();
	setupPrescalerAndTCNT();
	setPrescaler();
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
	{
		mpIO->TCNTH = mTCNT>>8;
		mpIO->TCNTL = mTCNT;
	}
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
		mpCB(mpUserdata);
		
	mpIO->TCNTH = mTCNT>>8;
	mpIO->TCNTL = mTCNT;
}


ISR(TIMER1_OVF_vect)
{
	timers[1]->doISR();
}

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


