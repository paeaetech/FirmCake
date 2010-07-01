#ifndef STEPPER_H_T5YBUFNN
#define STEPPER_H_T5YBUFNN

#include <inttypes.h>

class Stepper
{
public:
	Stepper(volatile uint8_t *port,uint8_t stepPin,uint8_t dirPin,uint8_t enPin,uint8_t minPin,uint8_t maxPin);

	void enable() { setEnabled(true); }
	void disable() { setEnabled(false); }
	void reset() { mCurrentStep = 0; mTargetStep = 0; mDeltaSteps = 0; }
	
	uint32_t getCurrentStep() const { return mCurrentStep; }
	void setCurrentStep(uint32_t step) { mCurrentStep = step; }
	uint32_t getTargetStep() const { return mTargetStep; }
	void setTargetStep(uint32_t step); 
	int32_t getDeltaSteps() const { return mDeltaSteps; }
	
	void setDirectionPositive(bool v);
	void setInverted(bool v) { mbInverted = v; }
	void setEnabled(bool v);
	
	bool needStepping() { return mCurrentStep != mTargetStep; }

	void doStep();
	
protected:
	const volatile uint8_t * mpPort;
	const uint8_t mStepPin;
	const uint8_t mDirPin;
	const uint8_t mEnPin;
	const uint8_t mMinPin;
	const uint8_t mMaxPin;
	bool mbInverted;
	bool mbDirectionPositive;
	
	uint32_t mCurrentStep;
	uint32_t mTargetStep;
	int32_t mDeltaSteps;



};


#endif /* end of include guard: STEPPER_H_T5YBUFNN */
