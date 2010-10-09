

#include "Stepper.h"
#include "portAccess.h"
#include "config.h"

#define ABS(x) (x < 0 ? -x : x)

Stepper::Stepper(volatile uint8_t *stepPort,uint8_t stepPin,
		volatile uint8_t *dirPort,uint8_t dirPin,
		volatile uint8_t *enPort,uint8_t enPin,
		volatile uint8_t *minPort,uint8_t minPin,
		volatile uint8_t *maxPort,uint8_t maxPin)
: mpStepPort(stepPort), 
  mStepPin(stepPin),
  mpDirPort(dirPort),
  mDirPin(dirPin),
  mpEnPort(enPort),
  mEnPin(enPin),
  mpMinPort(minPort),
  mMinPin(minPin),
  mpMaxPort(maxPort),
  mMaxPin(maxPin)
{
	DDRP(mpStepPort) |= mStepPin;
	DDRP(mpDirPort) |= mDirPin;
	DDRP(mpEnPort) |= mEnPin;

	reset();
}

void Stepper::setEnabled(bool v)
{
	if (v)
		PINP(mpEnPort) |= mEnPin;
	else
		PINP(mpEnPort) &= ~mEnPin;
}

void Stepper::setDirectionPositive(bool v)
{
	mbDirectionPositive = mbInverted ? !v : v;

	if (mbDirectionPositive)
		PINP(mpDirPort) |= mDirPin;
	else
		PINP(mpDirPort) &= ~mDirPin;
}

void Stepper::setCurrentStep(int32_t step)
{
#if STEPPER_TYPE == STEPPER_TYPE_MICROSTEP
	mCurrentStep = step * STEPPER_MICROSTEPS;
#else
	mTargetStep = step;
#endif
}

void Stepper::setTargetStep(int32_t step)
{
#if STEPPER_TYPE == STEPPER_TYPE_MICROSTEP
	mTargetStep = step * STEPPER_MICROSTEPS;
#else
	mTargetStep = step;
#endif

	mDeltaSteps = mTargetStep - mCurrentStep;
	setDirectionPositive(mDeltaSteps >= 0);
	mDeltaSteps = ABS(mDeltaSteps);

	if (mDeltaSteps > 0)
		enable();
		
}

void Stepper::doStep()
{
	mCurrentStep += mbDirectionPositive ? 1 : -1;
	PINP(mpStepPort) |= mStepPin;
	PINP(mpStepPort) &= ~mStepPin;
}


