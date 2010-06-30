
#include <inttypes.h>
#include "StepperController.h"
#include "config.h"

namespace {
	Stepper steppers[3] = {
		Stepper(&STEPPERX_PORT,_BV(STEPPERX_STEP_PIN),_BV(STEPPERX_DIR_PIN),_BV(STEPPERX_EN_PIN),_BV(STEPPERX_MIN_PIN),_BV(STEPPERX_MAX_PIN)),
		Stepper(&STEPPERY_PORT,_BV(STEPPERY_STEP_PIN),_BV(STEPPERY_DIR_PIN),_BV(STEPPERY_EN_PIN),_BV(STEPPERY_MIN_PIN),_BV(STEPPERY_MAX_PIN)),
		Stepper(&STEPPERZ_PORT,_BV(STEPPERZ_STEP_PIN),_BV(STEPPERZ_DIR_PIN),_BV(STEPPERZ_EN_PIN),_BV(STEPPERZ_MIN_PIN),_BV(STEPPERZ_MAX_PIN)),
	};
	
}

StepperController::StepperController()
{

}
