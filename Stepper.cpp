

#include "Stepper.h"
#include "portAccess.h"


#define ABS(x) (x < 0 ? -x : x)

Stepper::Stepper(volatile uint8_t *port,uint8_t stepPin,uint8_t dirPin,uint8_t enPin,uint8_t minPin,uint8_t maxPin)
: mpPort(port), 
  mStepPin(stepPin),
  mDirPin(dirPin),
  mEnPin(enPin),
  mMinPin(minPin),
  mMaxPin(maxPin)
{
	DDRP(mpPort) = mStepPin | mDirPin | mEnPin;
	reset();
}

void Stepper::setEnabled(bool v)
{
	if (v)
		PINP(mpPort) |= mEnPin;
	else
		PINP(mpPort) &= ~mEnPin;
}

void Stepper::setDirectionPositive(bool v)
{
	mbDirectionPositive = mbInverted ? !v : v;

	if (mbDirectionPositive)
		PINP(mpPort) |= mDirPin;
	else
		PINP(mpPort) &= ~mDirPin;
}

void Stepper::setTargetStep(uint32_t step)
{
	mTargetStep = step;
	mDeltaSteps = mTargetStep - mCurrentStep;
	setDirectionPositive(mDeltaSteps >= 0);
	mDeltaSteps = ABS(mDeltaSteps);

	if (mDeltaSteps > 0)
		enable();
		
}

void Stepper::doStep()
{
	mCurrentStep += mbDirectionPositive ? 1 : -1;
	PINP(mpPort) |= mStepPin;
	PINP(mpPort) &= ~mStepPin;
}

void Stepper::doISR()
{
	//add endstop checking
	
	if (needStepping())
		doStep();
}


