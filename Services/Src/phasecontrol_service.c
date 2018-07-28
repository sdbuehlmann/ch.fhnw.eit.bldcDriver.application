/*
 * phasecontrol_service.c
 *
 *  Created on: Dec 15, 2017
 *      Author: simon
 */
// =============== Includes ==============================================
#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"
#include "logger.h"
// =============== Defines ===============================================

// =============== Variables =============================================
static uint8_t activeState = STOP_SIN_APPROX;
static volatile uint8_t phaseSection = 6; // power off all
static uint32_t t60Deg = 0;
static uint32_t timestampStartSection;

// =============== Function pointers =====================================
void (*pSectionEndsListener)(uint8_t section, uint8_t nextSection);

// =============== Function declarations =================================
/** Changes the output channels for the pwm.
 Input:
 state = 0: A heavyside, C lowside
 state = 1: B heavyside, C lowside
 state = 2: B heavyside, A lowside
 state = 3: C heavyside, A lowside
 state = 4: C heavyside, B lowside
 state = 5: A heavyside, B lowside
 state > 5: power off all channels
 **/
void changePhaseState(uint8_t newPhaseSection);
void switchPhases(uint8_t forward_backward_selecter);
void timerCallback();

// =============== Functions =============================================
void initPhaseControllService() {
	logMsg_debug("phasencontrol service initialized.");
}

void changePhaseState(uint8_t newPhaseSection) {
	enable_PWM_phaseA_HS(0);
	enable_PWM_phaseB_HS(0);
	enable_PWM_phaseC_HS(0);
	enable_PWM_phaseA_LS(0);
	enable_PWM_phaseB_LS(0);
	enable_PWM_phaseC_LS(0);

	switch (newPhaseSection) {
	case 0:
		enable_PWM_phaseA_HS(1);
		enable_PWM_phaseC_LS(1);
		break;

	case 1:
		enable_PWM_phaseB_HS(1);
		enable_PWM_phaseC_LS(1);
		break;

	case 2:
		enable_PWM_phaseB_HS(1);
		enable_PWM_phaseA_LS(1);
		break;

	case 3:
		enable_PWM_phaseC_HS(1);
		enable_PWM_phaseA_LS(1);
		break;

	case 4:
		enable_PWM_phaseC_HS(1);
		enable_PWM_phaseB_LS(1);
		break;

	case 5:
		enable_PWM_phaseA_HS(1);
		enable_PWM_phaseB_LS(1);
		break;

	default:
		break;
	}

	phaseSection = newPhaseSection;
}

void switchPhases(uint8_t forward_backward_selecter) {
	// calc next section
	uint8_t nextSection;

	switch (forward_backward_selecter) {
	case START_SIN_APPROX_FORWARD:
		nextSection = (phaseSection + 1) % 6;
		break;

	case START_SIN_APPROX_BACKWARD:
		nextSection = (phaseSection - 1) % 6;
		break;

	default:
		break;
	}

	// inform listener
	if (pSectionEndsListener != 0) {
		pSectionEndsListener(phaseSection, nextSection);
	}

	changePhaseState(nextSection);
	timestampStartSection = getTimestamp();
}

void control3PhaseSinusApproximation(uint8_t start_stop_selecter) {
	if (start_stop_selecter != STOP_SIN_APPROX && t60Deg != 0) {
		//startAfterUs(t60Deg, &timerCallback);
		if (delayedCallback_D(t60Deg,
				&timerCallback) != DELAYED_CALLBACK_REGISTERED) {
			control3PhaseSinusApproximation(STOP_SIN_APPROX);
		}
	} else {
		enable_PWM_phaseA_HS(0);
		enable_PWM_phaseB_HS(0);
		enable_PWM_phaseC_HS(0);
		enable_PWM_phaseA_LS(0);
		enable_PWM_phaseB_LS(0);
		enable_PWM_phaseC_LS(0);

		phaseSection = 6;
	}
	activeState = start_stop_selecter;
}

void setSinusApproximation60DegTime(uint32_t t60DegParam) {
	t60Deg = t60DegParam;
}

void timerCallback() {
	if (activeState == START_SIN_APPROX_FORWARD
			|| activeState == START_SIN_APPROX_BACKWARD) {
		switchPhases(activeState);

		if (delayedCallback_D(t60Deg,
				&timerCallback) != DELAYED_CALLBACK_REGISTERED) {
			control3PhaseSinusApproximation(STOP_SIN_APPROX);
		}
	}
}

// getters
uint32_t getSinusApproximation60DegTime() {
	return t60Deg;
}

uint8_t getPhasecontrolState() {
	return activeState;
}

uint8_t getActiveSection() {
	return phaseSection;
}

// register listeners
void registerListener_sectionEnds_ISR(
		void (*pListener)(uint8_t section, uint8_t nextSection)) {
	pSectionEndsListener = pListener;
}
