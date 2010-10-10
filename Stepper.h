#ifndef STEPPER_H_T5YBUFNN
#define STEPPER_H_T5YBUFNN

#include <inttypes.h>
#include "config.h"

class Stepper
{
public:
	Stepper(volatile uint8_t *stepPort,uint8_t stepPin,
			volatile uint8_t *dirPort,uint8_t dirPin,
			volatile uint8_t *enPort,uint8_t enPin,
			volatile uint8_t *minPort,uint8_t minPin,
			volatile uint8_t *maxPort,uint8_t maxPin);

	void enable() { setEnabled(true); }
	void disable() { setEnabled(false); }
	
	virtual void reset() { mCurrentStep = 0; mTargetStep = 0; mDeltaSteps = 0; }

#if STEPPER_TYPE == STEPPER_TYPE_MICROSTEP	
	virtual int32_t getCurrentStep() { return mCurrentStep / STEPPER_MICROSTEPS; }
	virtual int32_t getTargetStep() { return mTargetStep / STEPPER_MICROSTEPS; }
#else
	virtual int32_t getCurrentStep() { return mCurrentStep; }
	virtual int32_t getTargetStep() { return mTargetStep; }
#endif
	virtual void setCurrentStep(int32_t step);
	virtual void setTargetStep(int32_t step); 

	int32_t getDeltaSteps() const { return mDeltaSteps; }

	
	virtual void setDirectionPositive(bool v);
	virtual void setEnabled(bool v);

	virtual void setInverted(bool v) { mbInverted = v; }
	virtual void setMicrosteps(uint8_t steps) {}
	
	bool needStepping() { return mCurrentStep != mTargetStep; }

	virtual void doStep();
	
protected:
	const volatile uint8_t * mpStepPort;
	const volatile uint8_t * mpDirPort;
	const volatile uint8_t * mpEnPort;
	const volatile uint8_t * mpMinPort;
	const volatile uint8_t * mpMaxPort;

	const uint8_t mStepPin;
	const uint8_t mDirPin;
	const uint8_t mEnPin;
	const uint8_t mMinPin;
	const uint8_t mMaxPin;
	bool mbInverted;
	bool mbDirectionPositive;
	
	volatile int32_t mCurrentStep;
	volatile int32_t mTargetStep;
	volatile int32_t mDeltaSteps;

}; 


#endif /* end of include guard: STEPPER_H_T5YBUFNN */
