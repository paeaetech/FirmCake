#ifndef STEPPERCONTROLLER_H_RTJFI7CF
#define STEPPERCONTROLLER_H_RTJFI7CF

#include "config.h"
#include "Stepper.h"
#include "owntypes.h"
#include "RingBuffer.h"

class StepperController
{
public:
	StepperController();
	
	
	void disableSteppers(bool x=true,bool y=true,bool z=true);
	void enableSteppers(bool x=true, bool y=true, bool z=true);
	
	void setPoint(int32_t x,int32_t y,int32_t z);
	void setPoint(const Point& rPoint) { setPoint(rPoint.x,rPoint.y,rPoint.z); }

	void moveTo(int32_t x,int32_t y,int32_t z,int32_t feedRate);
	void getPoint(Point& rPoint);
	
	bool isMoving();
	
	void update();
protected:
	void doISR();

	bool mbMoving;
	
	static void timerCallback(void*udata) { StepperController* sc = reinterpret_cast<StepperController*>(udata); sc->doISR(); }
	
};

#ifdef USE_STEPPERS
extern StepperController stepperController;
#endif



#endif /* end of include guard: STEPPERCONTROLLER_H_RTJFI7CF */
