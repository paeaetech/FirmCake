#ifndef STEPPER_H_T5YBUFNN
#define STEPPER_H_T5YBUFNN

#include <inttypes.h>

class Stepper
{
public:
	Stepper(volatile uint8_t *port,uint8_t stepPin,uint8_t dirPin,uint8_t enPin,uint8_t minPin,uint8_t maxPin);

	void enable() { setEnabled(true); }
	void disable() { setEnabled(false); }

	
	void setDirectionPositive(bool v);
	void setInverted(bool v) { mbInverted = v; }
	void setEnabled(bool v);
	
	void doISR();
	
protected:
	const volatile uint8_t * mpPort;
	const uint8_t mStepPin;
	const uint8_t mDirPin;
	const uint8_t mEnPin;
	const uint8_t mMinPin;
	const uint8_t mMaxPin;
	bool mbInverted;
	bool mbDirectionPositive;
	
};


#endif /* end of include guard: STEPPER_H_T5YBUFNN */
