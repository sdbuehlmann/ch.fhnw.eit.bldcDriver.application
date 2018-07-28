/*
 * measurement.c
 *
 *  Created on: Dec 24, 2017
 *      Author: simon
 */
// =============== Includes ===============================================
#include "measurement.h"

#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"
#include "logger.h"

// =============== Defines ===============================================
#define MAX_ZERO_CROSSINGS 5

// =============== Variables =============================================
static uint8_t enabledFlag = 0;

static uint8_t zeroCrossingCounter = 0;
static uint32_t zeroCrossingTimestamp = 0;

// =============== Function pointers =====================================
void (*pNewRotorPositionMeasurement_listener_ISR)(uint32_t);
void (*pRotorTooEarlyForMeas_listener_ISR)(void);
void (*pRotorTooLateForMeas_listener_ISR)(void);
void (*pTooManyZeroCrossings_listener_ISR)(void);

// =============== Function declarations =================================
void handle_sectionEnds(uint8_t section, uint8_t nextSection);

void handle_zeroCrossing_phaseA(volatile uint8_t edge);
void handle_zeroCrossing_phaseB(volatile uint8_t edge);
void handle_zeroCrossing_phaseC(volatile uint8_t edge);

// =============== Functions =============================================
void initMeasurement() {
	registerListener_sectionEnds_ISR(&handle_sectionEnds);

	registerListener_zeroCrossing(
			&handle_zeroCrossing_phaseA,
			&handle_zeroCrossing_phaseB,
			&handle_zeroCrossing_phaseC);
}

void enableMeasurement(uint8_t enable) {
	if (enable) {
		enabledFlag = 1;

	} else {
		enableZeroCrossingIRQ(PHASE_A, 0);
		enableZeroCrossingIRQ(PHASE_B, 0);
		enableZeroCrossingIRQ(PHASE_C, 0);

		enabledFlag = 0;
	}
}

void register_rotorPosMeas_listener_ISR(void (*pNewMeasListener)(uint32_t),
		void (*pRotorTooEarlyListener)(void),
		void (*pRotorTooLateListener)(void)) {
	pNewRotorPositionMeasurement_listener_ISR = pNewMeasListener;
	pRotorTooEarlyForMeas_listener_ISR = pRotorTooEarlyListener;
	pRotorTooLateForMeas_listener_ISR = pRotorTooLateListener;
}

void register_tooManyZeroCrossings_listener_ISR(void (*listener)(void)) {
	pTooManyZeroCrossings_listener_ISR = listener;
}

void classifyMeasurement(uint8_t phase, uint8_t rightSignalStatus) {
	uint8_t statusSignal = readStatusOfZeroCrossingSignal(phase);
	if (statusSignal != rightSignalStatus) {
		// wrong status -> no valid zero crossing
		pRotorTooLateForMeas_listener_ISR();
	} else if (zeroCrossingTimestamp == 0) {
		// no valid meas time -> no valid zero crossing
		pRotorTooEarlyForMeas_listener_ISR();
	} else {
		// valid measurement
		uint32_t deltaTime = calculateDeltaTime(zeroCrossingTimestamp);
		pNewRotorPositionMeasurement_listener_ISR(
				getSinusApproximation60DegTime() - deltaTime);
	}
}

// listeners
void handle_zeroCrossing_phaseA(volatile uint8_t edge) {
	volatile uint8_t section = getActiveSection();

	uint32_t tempTimestamp = getTimestamp();
	logEvent_compA_IR();

	switch (section) {
	case SECTION_1_ACTIVE:
		if (edge == FALLING_EDGE) {
			zeroCrossingTimestamp = tempTimestamp;
			zeroCrossingCounter++;
		}
		break;
	case SECTION_4_ACTIVE:
		if (edge == RISING_EDGE) {
			zeroCrossingTimestamp = tempTimestamp;
			zeroCrossingCounter++;
		}
		break;
	default:
		// invalid zerocrossing --> do nothing
		break;
	}
}
void handle_zeroCrossing_phaseB(volatile uint8_t edge) {
	volatile uint8_t section = getActiveSection();

	uint32_t tempTimestamp = getTimestamp();
	logEvent_compB_IR();

	switch (section) {
	case SECTION_0_ACTIVE:
		if (edge == RISING_EDGE) {
			zeroCrossingTimestamp = tempTimestamp;
			zeroCrossingCounter++;
		}
		break;
	case SECTION_3_ACTIVE:
		if (edge == FALLING_EDGE) {
			zeroCrossingTimestamp = tempTimestamp;
			zeroCrossingCounter++;
		}
		break;
	default:
		// invalid zerocrossing --> do nothing
		break;
	}
}
void handle_zeroCrossing_phaseC(volatile uint8_t edge) {
	volatile uint8_t section = getActiveSection();

	uint32_t tempTimestamp = getTimestamp();
	logEvent_compC_IR();

	switch (section) {
	case SECTION_2_ACTIVE:
		if (edge == RISING_EDGE) {
			zeroCrossingTimestamp = tempTimestamp;
			zeroCrossingCounter++;
		}
		break;
	case SECTION_5_ACTIVE:
		if (edge == FALLING_EDGE) {
			zeroCrossingTimestamp = tempTimestamp;
			zeroCrossingCounter++;
		}
		break;
	default:
		// invalid zerocrossing --> do nothing
		break;
	}
}

void handle_sectionEnds(uint8_t section, uint8_t nextSection) {
	if (!enabledFlag) {
		return;
	}

	/*if (zeroCrossingCounter > MAX_ZERO_CROSSINGS) {
		// error!
		pTooManyZeroCrossings_listener_ISR();
	}*/

	switch (section) {
	case SECTION_0_ACTIVE:
		classifyMeasurement(PHASE_B, ZERO_CROSSING_SIGNAL_HIGH);
		break;
	case SECTION_1_ACTIVE:
		classifyMeasurement(PHASE_A, ZERO_CROSSING_SIGNAL_LOW);
		break;
	case SECTION_2_ACTIVE:
		classifyMeasurement(PHASE_C, ZERO_CROSSING_SIGNAL_HIGH);
		break;
	case SECTION_3_ACTIVE:
		classifyMeasurement(PHASE_B, ZERO_CROSSING_SIGNAL_LOW);
		break;
	case SECTION_4_ACTIVE:
		classifyMeasurement(PHASE_A, ZERO_CROSSING_SIGNAL_HIGH);
		break;
	case SECTION_5_ACTIVE:
		classifyMeasurement(PHASE_C, ZERO_CROSSING_SIGNAL_LOW);
		break;
	default:
		// run synchronisation
		break;
	}

	// reset
	zeroCrossingCounter = 0;
	zeroCrossingTimestamp = 0;

	// enable disable zero crossing interrupts
	enableZeroCrossingIRQ(PHASE_A, 0);
	enableZeroCrossingIRQ(PHASE_B, 0);
	enableZeroCrossingIRQ(PHASE_C, 0);

	switch (nextSection) {
	case SECTION_0_ACTIVE:
	case SECTION_3_ACTIVE:
		enableZeroCrossingIRQ(PHASE_B, 1);
		break;
	case SECTION_1_ACTIVE:
	case SECTION_4_ACTIVE:
		enableZeroCrossingIRQ(PHASE_A, 1);
		break;
	case SECTION_2_ACTIVE:
	case SECTION_5_ACTIVE:
		enableZeroCrossingIRQ(PHASE_C, 1);
		break;
	default:
		// run synchronisation
		break;
	}
}
