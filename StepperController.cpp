
#include <inttypes.h>
#include "StepperController.h"
#include "config.h"
#include "Timer.h"

#define STEPPER_HZ 100000

namespace {
	Stepper steppers[3] = {
		Stepper(&STEPPERX_PORT,_BV(STEPPERX_STEP_PIN),_BV(STEPPERX_DIR_PIN),_BV(STEPPERX_EN_PIN),_BV(STEPPERX_MIN_PIN),_BV(STEPPERX_MAX_PIN)),
		Stepper(&STEPPERY_PORT,_BV(STEPPERY_STEP_PIN),_BV(STEPPERY_DIR_PIN),_BV(STEPPERY_EN_PIN),_BV(STEPPERY_MIN_PIN),_BV(STEPPERY_MAX_PIN)),
		Stepper(&STEPPERZ_PORT,_BV(STEPPERZ_STEP_PIN),_BV(STEPPERZ_DIR_PIN),_BV(STEPPERZ_EN_PIN),_BV(STEPPERZ_MIN_PIN),_BV(STEPPERZ_MAX_PIN)),
	};
	
	
	Timer timer(5,STEPPER_HZ);
}

#define FORALL(x) { for (uint8_t _i=0;_i<3;_i++) steppers[i]->x; }


StepperController::StepperController()
{
	
	
}


void StepperController::setPoint(uint32_t x,uint32_t y,uint32_t z)
{
	steppers[0].setCurrentStep(x);
	steppers[1].setCurrentStep(y);
	steppers[2].setCurrentStep(z);

}


