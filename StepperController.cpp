
#include <inttypes.h>
#include "StepperController.h"
#include "config.h"
#include "Timer.h"
#include "utils.h"

#define STEPPER_HZ 100000

#define MAX(x,y) x > y ? x : y
#define MIN(x,y) x < y ? x : y
#define ABS(x) (x < 0 ? -x : x)


StepperController stepperController;

namespace {
	
	struct STEPPER {
		const volatile uint8_t * stepPort;
		const volatile uint8_t stepPin;

		const volatile uint8_t * dirPort;
		const volatile uint8_t dirPin;

		const volatile uint8_t * enPort;
		const volatile uint8_t enPin;

		const volatile uint8_t * minPort;
		const volatile uint8_t minPin;

		const volatile uint8_t * maxPort;
		const volatile uint8_t maxPin;

		bool bInverted;
		volatile bool bDirectionPositive;

		volatile int32_t currentStep;
		volatile int32_t targetStep;
		volatile int32_t deltaSteps;
		volatile uint32_t stepDelay;
		volatile uint32_t currentStepTime;
	};
	
	volatile STEPPER steppers[3] = {
		{
			&STEPPERX_STEP_PORT,_BV(STEPPERX_STEP_PIN),
			&STEPPERX_DIR_PORT,_BV(STEPPERX_DIR_PIN),
			&STEPPERX_EN_PORT,_BV(STEPPERX_EN_PIN),
			&STEPPERX_MIN_PORT,_BV(STEPPERX_MIN_PIN),
			&STEPPERX_MAX_PORT,_BV(STEPPERX_MAX_PIN),
			false,
			false,
			0,
			0,
			0,
			0,
			0,
		},
		{
			&STEPPERY_STEP_PORT,_BV(STEPPERY_STEP_PIN),
			&STEPPERY_DIR_PORT,_BV(STEPPERY_DIR_PIN),
			&STEPPERY_EN_PORT,_BV(STEPPERY_EN_PIN),
			&STEPPERY_MIN_PORT,_BV(STEPPERY_MIN_PIN),
			&STEPPERY_MAX_PORT,_BV(STEPPERY_MAX_PIN),
			false,
			false,
			0,
			0,
			0,
			0,
			0,
		},
		{
			&STEPPERZ_DIR_PORT,_BV(STEPPERZ_DIR_PIN),
			&STEPPERZ_EN_PORT,_BV(STEPPERZ_EN_PIN),
			&STEPPERZ_MIN_PORT,_BV(STEPPERZ_MIN_PIN),
			&STEPPERZ_MAX_PORT,_BV(STEPPERZ_MAX_PIN),
			false,
			false,
			0,
			0,
			0,
			0,
			0,			
		},
	};
/*		
	volatile Stepper stepperX(&STEPPERX_STEP_PORT,_BV(STEPPERX_STEP_PIN),
				&STEPPERX_DIR_PORT,_BV(STEPPERX_DIR_PIN),
				&STEPPERX_EN_PORT,_BV(STEPPERX_EN_PIN),
				&STEPPERX_MIN_PORT,_BV(STEPPERX_MIN_PIN),
				&STEPPERX_MAX_PORT,_BV(STEPPERX_MAX_PIN));

	volatile Stepper	stepperY(&STEPPERY_STEP_PORT,_BV(STEPPERY_STEP_PIN),
				&STEPPERY_DIR_PORT,_BV(STEPPERY_DIR_PIN),
				&STEPPERY_EN_PORT,_BV(STEPPERY_EN_PIN),
				&STEPPERY_MIN_PORT,_BV(STEPPERY_MIN_PIN),
				&STEPPERY_MAX_PORT,_BV(STEPPERY_MAX_PIN));
	volatile Stepper	stepperZ(&STEPPERZ_STEP_PORT,_BV(STEPPERZ_STEP_PIN),
				&STEPPERZ_DIR_PORT,_BV(STEPPERZ_DIR_PIN),
				&STEPPERZ_EN_PORT,_BV(STEPPERZ_EN_PIN),
				&STEPPERZ_MIN_PORT,_BV(STEPPERZ_MIN_PIN),
				&STEPPERZ_MAX_PORT,_BV(STEPPERZ_MAX_PIN));

	Stepper* steppers[3] = { (Stepper*)&stepperX, (Stepper*)&stepperY, (Stepper*)&stepperZ };
*/

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
	rPoint.x = steppers[0].currentStep;
	rPoint.y = steppers[1].currentStep;
	rPoint.z = steppers[2].currentStep;
}

bool StepperController::isMoving()
{
	bool ret = false;
	for (int i=0;i<3;i++)
		ret |= steppers[i].currentStep != steppers[i].targetStep;
	
	return ret;
}

void StepperController::disableSteppers(bool x,bool y,bool z)
{
	if (x)
		PORTP(steppers[0].enPort) &= ~steppers[0].enPin;
	if (y)
		PORTP(steppers[1].enPort) &= ~steppers[1].enPin;
	if (z)
		PORTP(steppers[2].enPort) &= ~steppers[2].enPin;

}

void StepperController::enableSteppers(bool x, bool y, bool z)
{
	if (x)
		PORTP(steppers[0].enPort) |= steppers[0].enPin;
	if (y)
		PORTP(steppers[1].enPort) |= steppers[1].enPin;
	if (z)
		PORTP(steppers[2].enPort) |= steppers[2].enPin;
}


void StepperController::setPoint(int32_t x,int32_t y,int32_t z)
{
	steppers[0].currentStep=x;
	steppers[1].currentStep=y;
	steppers[2].currentStep=z;
	

}

void StepperController::moveTo(int32_t x,int32_t y,int32_t z,int32_t feedRate)
{
	
	uint32_t moveTime;
	uint8_t i;
	
	for (i=0;i<3;i++)
	{
		steppers[i].targetStep = x;
		int32_t deltaSteps = x - steppers[i].currentStep;
		steppers[i].bDirectionPositive = deltaSteps >= 0;
		steppers[i].deltaSteps = ABS(deltaSteps);

		if (steppers[i].deltaSteps > 0)
		{
			PORTP(steppers[i].enPort) |= steppers[i].enPin;
			
			if (steppers[i].bDirectionPositive)
				PORTP(steppers[i].dirPort) |= steppers[i].dirPin;
			else
				PORTP(steppers[i].dirPort) &= ~steppers[i].dirPin;
				
		}
		moveTime = MAX(moveTime,steppers[i].deltaSteps / feedRate);
	}
	
	for (i=0;i<3;i++)
	{
		if (steppers[i].deltaSteps > 0)
		{
			steppers[i].stepDelay = STEPPER_HZ / (steppers[i].deltaSteps / moveTime);
			steppers[i].currentStepTime = steppers[i].stepDelay;
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
		if (!steppers[0].deltaSteps)
			PORTP(steppers[0].enPort) &= ~steppers[0].enPin;
#endif
#ifdef STEPPERY_DISABLE_INACTIVE
		if (!steppers[1].deltaSteps)
			PORTP(steppers[1].enPort) &= ~steppers[1].enPin;
#endif
#ifdef STEPPERZ_DISABLE_INACTIVE
		if (!steppers[2].deltaSteps)
			PORTP(steppers[2].enPort) &= ~steppers[2].enPin;
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
	if (steppers[3].deltaSteps > 0)
	{
		steppers[3].currentStepTime--;
		if (!steppers[3].currentStepTime)
		{
			PORTP(steppers[3].stepPort) |= steppers[3].stepPin;
			PORTP(steppers[3].stepPort) &= ~steppers[3].stepPin;
			steppers[3].currentStepTime = steppers[3].stepDelay;
			steppers[3].deltaSteps--;
			steppers[3].currentStep += steppers[3].bDirectionPositive ? 1 : -1;
		}
	}
	else
	{
		for (int i=0;i<2;i++)
		{
			steppers[i].currentStepTime--;
			if (!steppers[i].currentStepTime)
			{
				PORTP(steppers[i].stepPort) |= steppers[i].stepPin;
				PORTP(steppers[i].stepPort) &= ~steppers[i].stepPin;
				steppers[i].currentStepTime = steppers[i].stepDelay;
				steppers[i].deltaSteps--;
				steppers[i].currentStep += steppers[i].bDirectionPositive ? 1 : -1;
			}

		}
	}
/*
	if (steppers[3]->needStepping())
	{
		currentStepTime[3]--;
		if (currentStepTime[3] == 0)
		{
			steppers[3]->doStep();
			currentStepTime[3] = stepDelay[3];
		}
	}
	else
	{
		for (int i=0;i<2;i++)
		{
			if (steppers[i]->needStepping())
			{
				currentStepTime[i]--;
				if (currentStepTime[i] == 0)
				{
					steppers[i]->doStep();
					currentStepTime[i] = stepDelay[i];
				}
			}
		}
	}
*/
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

