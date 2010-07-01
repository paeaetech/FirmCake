
#include <inttypes.h>
#include "StepperController.h"
#include "config.h"
#include "Timer.h"

#define STEPPER_HZ 100000

#define MAX(x,y) x > y ? x : y
#define MIN(x,y) x < y ? x : y


namespace {
	Stepper steppers[3] = {
		Stepper(&STEPPERX_PORT,_BV(STEPPERX_STEP_PIN),_BV(STEPPERX_DIR_PIN),_BV(STEPPERX_EN_PIN),_BV(STEPPERX_MIN_PIN),_BV(STEPPERX_MAX_PIN)),
		Stepper(&STEPPERY_PORT,_BV(STEPPERY_STEP_PIN),_BV(STEPPERY_DIR_PIN),_BV(STEPPERY_EN_PIN),_BV(STEPPERY_MIN_PIN),_BV(STEPPERY_MAX_PIN)),
		Stepper(&STEPPERZ_PORT,_BV(STEPPERZ_STEP_PIN),_BV(STEPPERZ_DIR_PIN),_BV(STEPPERZ_EN_PIN),_BV(STEPPERZ_MIN_PIN),_BV(STEPPERZ_MAX_PIN)),
	};
	
	uint32_t stepDelay[3];
	uint32_t currentStepTime[3];
	
	Timer timer(5,STEPPER_HZ);
}


StepperController::StepperController()
{
	timer.setCallback(timerCallback);
	
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


void StepperController::setPoint(uint32_t x,uint32_t y,uint32_t z)
{
	steppers[0].setCurrentStep(x);
	steppers[1].setCurrentStep(y);
	steppers[2].setCurrentStep(z);

}

void StepperController::moveTo(uint32_t x,uint32_t y,uint32_t z,uint32_t feedRate)
{
	steppers[0].setTargetStep(x);
	steppers[1].setTargetStep(y);
	steppers[2].setTargetStep(z);
	
	uint32_t moveTime=0;
	uint8_t i;
	for (i=0;i<3;i++)
	{
		int32_t delta = steppers[i].getDeltaSteps();
		moveTime = MAX(moveTime,delta / (feedRate/STEPPER_HZ));
	}
	
	for (i=0;i<3;i++)
	{
		stepDelay[i] = moveTime / steppers[i].getDeltaSteps();
		currentStepTime[i] = stepDelay[i];
	}
}

void StepperController::doISR()
{
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
}

