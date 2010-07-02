#ifndef TIMER_H_E2HKPL15
#define TIMER_H_E2HKPL15

#include <inttypes.h>

typedef void (*TimerCallback)(void*);


struct TimerIOMap {
	volatile uint8_t TCCRA;
	volatile uint8_t TCCRB;
	volatile uint8_t TCCRC;
	volatile uint8_t fill1;
	volatile uint8_t TCNTL;
	volatile uint8_t TCNTH;
	volatile uint8_t ICRL;
	volatile uint8_t ICRH;
	volatile uint8_t OCRAL;
	volatile uint8_t OCRAH;
	volatile uint8_t OCRBL;
	volatile uint8_t OCRBH;
	volatile uint8_t OCRCL;
	volatile uint8_t OCRCH;
};


//this is only for 16-bit timers
class Timer
{
public:
	Timer(uint8_t timer,uint32_t hz);
	
	void enable();
	void disable();
	void reset();
	
	void setFrequency(uint32_t hz);
	void setCallback(TimerCallback cb,void* userData=0) { mpCB = cb; mpUserdata = userData; }
	

	void doISR();
	
protected:
	
	void setupPrescalerAndTCNT();
	void setPrescaler();
	
	uint8_t mTimerNum;
	uint32_t mHZ;
	uint16_t mTCNT;
	uint8_t mPrescaler;
	
	void* mpUserdata;
	TimerCallback mpCB;
	TimerIOMap* mpIO;
	
};



#endif /* end of include guard: TIMER_H_E2HKPL15 */
