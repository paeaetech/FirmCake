
#include <inttypes.h>
#include "StepperController.h"
#include "config.h"
#include "Timer.h"
#include "utils.h"

#define STEPPER_HZ 100000

#define MAX(x,y) x > y ? x : y
#define MIN(x,y) x < y ? x : y


StepperController stepperController;

namespace {
	Stepper steppers[3] = {
		Stepper(&STEPPERX_STEP_PORT,_BV(STEPPERX_STEP_PIN),
				&STEPPERX_DIR_PORT,_BV(STEPPERX_DIR_PIN),
				&STEPPERX_EN_PORT,_BV(STEPPERX_EN_PIN),
				&STEPPERX_MIN_PORT,_BV(STEPPERX_MIN_PIN),
				&STEPPERX_MAX_PORT,_BV(STEPPERX_MAX_PIN)),
		Stepper(&STEPPERY_STEP_PORT,_BV(STEPPERY_STEP_PIN),
				&STEPPERY_DIR_PORT,_BV(STEPPERY_DIR_PIN),
				&STEPPERY_EN_PORT,_BV(STEPPERY_EN_PIN),
				&STEPPERY_MIN_PORT,_BV(STEPPERY_MIN_PIN),
				&STEPPERY_MAX_PORT,_BV(STEPPERY_MAX_PIN)),
		Stepper(&STEPPERZ_STEP_PORT,_BV(STEPPERZ_STEP_PIN),
				&STEPPERZ_DIR_PORT,_BV(STEPPERZ_DIR_PIN),
				&STEPPERZ_EN_PORT,_BV(STEPPERZ_EN_PIN),
				&STEPPERZ_MIN_PORT,_BV(STEPPERZ_MIN_PIN),
				&STEPPERZ_MAX_PORT,_BV(STEPPERZ_MAX_PIN)),
	};
	
	volatile uint32_t stepDelay[3];
	volatile uint32_t currentStepTime[3];

#ifdef STEPPER_TIMER_8BIT
	Timer8 timer(STEPPER_TIMER,STEPPER_HZ);
#else
	Timer timer(STEPPER_TIMER,STEPPER_HZ);
#endif
}


StepperController::StepperController()
{
	timer.setCallback(timerCallback);
	
}

void StepperController::getPoint(Point& rPoint)
{
	rPoint.x = steppers[0].getCurrentStep();
	rPoint.y = steppers[1].getCurrentStep();
	rPoint.z = steppers[2].getCurrentStep();
}

bool StepperController::isMoving()
{
	bool ret = false;
	for (int i=0;i<3;i++)
		ret |= steppers[i].needStepping();
	
	return ret;
}

void StepperController::disableSteppers(bool x,bool y,bool z)
{
	if (x)
		steppers[0].disable();
	if (y)
		steppers[1].disable();
	if (z)
		steppers[2].disable();

}

void StepperController::enableSteppers(bool x, bool y, bool z)
{
	if (x)
		steppers[0].enable();
	if (y)
		steppers[1].enable();
	if (z)
		steppers[2].enable();
}


void StepperController::setPoint(int32_t x,int32_t y,int32_t z)
{
	steppers[0].setCurrentStep(x);
	steppers[1].setCurrentStep(y);
	steppers[2].setCurrentStep(z);

}

void StepperController::moveTo(int32_t x,int32_t y,int32_t z,int32_t feedRate)
{
	DEBUG_OUTF("moveTo %d %d %d %d\r\n",x,y,z,feedRate);
	steppers[0].setTargetStep(x);
	steppers[1].setTargetStep(y);
	steppers[2].setTargetStep(z);
	
	uint32_t moveTime;
	uint8_t i;
	
	for (i=0;i<3;i++)
	{
		moveTime = MAX(moveTime,steppers[i].getDeltaSteps() / feedRate);
	}
	DEBUG_OUTF("movetime %d\r\n",moveTime);
	for (i=0;i<3;i++)
	{
		if (steppers[i].needStepping())
		{
			stepDelay[i] = STEPPER_HZ / (steppers[i].getDeltaSteps() / moveTime);
			currentStepTime[i] = stepDelay[i];
			DEBUG_OUTF("stepdelay[%d]: %d\r\n",i,stepDelay[i]);
		}
	}
	
	mbMoving = isMoving();
	
	if (mbMoving)
		timer.enable();
		
}

void StepperController::update()
{
	if (mbMoving)
	{
#ifdef STEPPERX_DISABLE_INACTIVE
		if (!steppers[0].needStepping())
			steppers[0].disable();
#endif
#ifdef STEPPERY_DISABLE_INACTIVE
		if (!steppers[1].needStepping())
			steppers[1].disable();
#endif
#ifdef STEPPERZ_DISABLE_INACTIVE
		if (!steppers[2].needStepping())
			steppers[2].disable();
#endif

		mbMoving = isMoving();
		if (!mbMoving)
			timer.disable();
	}
}

void StepperController::doISR()
{
	//HACK: move z axis first if it needs to be moved to the head does not try to go through the object
	//essentially the host software should split the move into two commands, move z first and move xy after
	if (steppers[3].needStepping())
	{
		currentStepTime[3]--;
		if (currentStepTime[3] == 0)
		{
			steppers[3].doStep();
			currentStepTime[3] = stepDelay[3];
		}
	}
	else
	{
		for (int i=0;i<2;i++)
		{
			if (steppers[i].needStepping())
			{
				currentStepTime[i]--;
				if (currentStepTime[i] == 0)
				{
					steppers[i].doStep();
					currentStepTime[i] = stepDelay[i];
				}
			}
		}
	}
#if 0
	for (uint8_t i=0;i<3;i++)
	{
		if (steppers[i].needStepping())
		{
			currentStepTime[i]--;
			if (currentStepTime[i] == 0)
			{
				steppers[i].doStep();
				currentStepTime[i] = stepDelay[i];
			}
		}
	}
#endif
}

