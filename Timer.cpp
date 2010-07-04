

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
	volatile uint8_t* timerTCCRnA[] = { 0, &TCCR1A, 0,&TCCR3A, &TCCR4A, &TCCR5A };
	volatile uint8_t* timerMSK[] = {0, &TIMSK1, 0, &TIMSK3, &TIMSK4, &TIMSK5 };

	const uint16_t prescalers[] = { 1,8,64,256,1024 };
	const uint8_t prescalerFlags[] = { _BV(CS0), _BV(CS1), _BV(CS0)|_BV(CS1), _BV(CS2), _BV(CS2)|_BV(CS0) };
	Timer* timers[6];
#elif defined(__AVR_ATmega644P__) //sanguino
	volatile uint8_t* timerTCCRnA[] = { 0, &TCCR1A };
	volatile uint8_t* timerMSK[] = {0, &TIMSK1 };

	const uint16_t prescalers[] = { 1,8,64,256,1024 };
	const uint8_t prescalerFlags[] = { _BV(CS0), _BV(CS1), _BV(CS0)|_BV(CS1), _BV(CS2), _BV(CS2)|_BV(CS0) };
	Timer* timers[6];
#endif

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
#if defined(__AVR_ATmega644P__) //sanguino
	TIMSK = _BV(TOIE); //always on on sanguino because we use this for clock also
#endif
}

void Timer::enable()
{
#if defined(__AVR_ATmega644P__) //sanguino
	doCallback=true;
#else
	TIMSK = _BV(TOIE);
#endif
}

void Timer::disable()
{
#if defined(__AVR_ATmega644P__) //sanguino
	doCallback=false;
#else	
	TIMSK = 0;
#endif
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
#if defined(__AVR_ATmega644P__) //sanguino
	clock_callback();
	
	if (mpCB && doCallback)
#else
	if (mpCB)
#endif
		mpCB((void*)mpUserdata);
		
	mpIO->TCNTH = mTCNT>>8;
	mpIO->TCNTL = mTCNT;
}


ISR(TIMER1_OVF_vect)
{
	timers[1]->doISR();
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


