

#include "Stepper.h"
#include "portAccess.h"


Stepper::Stepper(volatile uint8_t *port,uint8_t stepPin,uint8_t dirPin,uint8_t enPin,uint8_t minPin,uint8_t maxPin)
: mpPort(port), 
  mStepPin(stepPin),
  mDirPin(dirPin),
  mEnPin(enPin),
  mMinPin(minPin),
  mMaxPin(maxPin)
  
{
	DDRP(mpPort) = mStepPin | mDirPin | mEnPin;
	
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
