#ifndef STEPPERCONTROLLER_H_RTJFI7CF
#define STEPPERCONTROLLER_H_RTJFI7CF

#include "Stepper.h"
#include "owntypes.h"

class StepperController
{
public:
	StepperController();
	
	
	void setPoint(uint32_t x,uint32_t y,uint32_t z);
	void setPoint(const Point& rPoint) { setPoint(rPoint.x,rPoint.y,rPoint.z); }
	
protected:
	void doISR();
	
	static void timerCallback(void*udata) { StepperController* sc = reinterpret_cast<StepperController*>(udata); sc->doISR(); }
	
};


#endif /* end of include guard: STEPPERCONTROLLER_H_RTJFI7CF */
