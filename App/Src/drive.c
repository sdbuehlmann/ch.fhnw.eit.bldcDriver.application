/*
 * drive.c
 *
 *  Created on: Dec 25, 2017
 *      Author: simon
 */

#include "drive.h"

#include "drive_state.h"
#include "logger.h"
#include "measurement.h"
#include "pwrControl.h"

#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"

// =============== Defines =============================================
#define GRADIENT_DIVIDER 128
#define TIME_60DEG_SPEED_UP_START 10000
#define TIME_60DEG_SPEED_UP_END 1500

#define TIMING 0
#define MAX_T_60_DEG 5000

// _controller options____________________________________________________
#define P_DIVIDER 32
#define I_DIVIDER 256
//#define D_DIVIDER 32768

// =============== Variables =============================================
DriveState activeState = off;
uint8_t started = 0;

static uint32_t time60deg;
static uint32_t timestamp_entryState;

// =============== Function Pointers =====================================

// =============== Function Declarations =================================
void changeTiming(uint32_t timing);

// =============== Functions =============================================
void initDrive() {
	logMsg_debug("init drive...");
	changeState(off);

	if (getDebouncedCalibrateJumper()) {
		// calibrate encoder mode
		changeState(calibrate_encoder);
	} else if (getDebouncedEnableEncoderJumper()) {
		// running mode with encoder

	} else {
		// running mode without encoder
		uint32_t poti = getReferencePositionEncoder();
		logData_encoderPos(poti);
		setReferencePosition(poti);

		changeState(start_up);


		startCurrentMeasurement();
	}
}

void proceedRotorPosController(volatile uint32_t rotorpos_deg) {
	volatile uint32_t target_deg = 30 - TIMING;

	volatile int32_t fault = (target_deg - rotorpos_deg);

	volatile int32_t controllerOut = (fault / P_DIVIDER);

#ifdef I_DIVIDER
	// integrator active
	volatile static int32_t fault_I;
	fault_I = fault_I + fault;
	controllerOut += fault_I / I_DIVIDER;
#endif
#ifdef D_DIVIDER
	// differentiator active
	static int32_t last_fault;
	controllerOut += (fault-last_fault)/D_DIVIDER;
#endif

	time60deg = time60deg - controllerOut;
	setSinusApproximation60DegTime(time60deg);

	if (time60deg > MAX_T_60_DEG) {
		// too slow
		control3PhaseSinusApproximation(STOP_SIN_APPROX);
		changeState(off);
	}
}

void proceedController(volatile uint32_t rotorpos) {
	volatile uint32_t targetTime = (time60deg * (30 - TIMING)) / 60;

	volatile int32_t fault = (targetTime - rotorpos);

	volatile int32_t controllerOut = (fault / P_DIVIDER);

#ifdef I_DIVIDER
	// integrator active
	volatile static int32_t fault_I;
	fault_I = fault_I + fault;
	controllerOut += fault_I / I_DIVIDER;
#endif
#ifdef D_DIVIDER
	// differentiator active
	static int32_t last_fault;
	controllerOut += (fault-last_fault)/D_DIVIDER;
#endif

	time60deg = time60deg - controllerOut;

	setSinusApproximation60DegTime(time60deg);

	if (time60deg > MAX_T_60_DEG) {
		// too slow
		control3PhaseSinusApproximation(STOP_SIN_APPROX);
		changeState(off);
	}

	//log_controllerParameterTuple_mr(5, time60deg, rotorpos, targetTime, controllerOut);
	//log_controllerParameterTuple(time60deg, rotorpos, targetTime,controllerOut);
}

// statemachine & transitions
void entryState(DriveState state) {
	logData_entryNewState(state);

	switch (state) {
	case off:
		setPowerLED_continiousMode();

		switch_Enable_BridgeDriver(0);
		enableMeasurement(0);

		setPowerlevelImmediately(OFF_POWER_LEVEL);
		control3PhaseSinusApproximation(STOP_SIN_APPROX);
		break;

	case stopped:
		break;

	case start_up:
		time60deg = TIME_60DEG_SPEED_UP_START; //us
		setSinusApproximation60DegTime(time60deg);

		switch_Enable_BridgeDriver(1);
		setPowerlevelImmediately(70);
		control3PhaseSinusApproximation(START_SIN_APPROX_FORWARD);
		break;

	case synchronized:
		break;

	case controlled_negative_torque:
		break;

	case controlled_positive_torque:
		setPowerlevelImmediately(70);
		enableMeasurement(1);
		break;

	case calibrate_encoder:
		changeToCalibrationMode();
		setPowerLED_blinkingMode();
		break;
	}
}

void exitState(DriveState state) {
	switch (state) {
	case off:
		break;
	case stopped:
		break;
	case start_up:
		break;
	case synchronized:
		break;
	case controlled_negative_torque:
		break;
	case controlled_positive_torque:
		enableMeasurement(0);
		break;
	case calibrate_encoder:

		break;
	}
}

void changeState(DriveState newState) {
	exitState(activeState);

	timestamp_entryState = getTimestamp();
	entryState(newState);

	activeState = newState;
}

// callbacks
void inform_newRotorPos(uint32_t time) {
	switch (activeState) {
	case controlled_positive_torque: {
		// convert time to angle
		/*uint8_t activeSection = getActiveSection();


		uint32_t rotorPos_backEMF = (time * 60) / time60deg;
		uint32_t rotorPos_encoder = getRotorPosition();

		// calculate setpoint (with timing)
		int32_t rotorPos_setPoint = getRotorPositionSetPointOfSectionInZeroCrossing(activeSection) + TIMING;
#if (TIMING != 0)
		if(rotorPos_setPoint < 0){
			rotorPos_setPoint += 360;
		}
#endif

		// calculate error angle


		//proceedRotorPosController(rotorPos_encoder);

		log_controllerParameterTuple(time60deg, rotorPos_backEMF,
				rotorPos_encoder);*/
	}
		break;
	default:
		// do nothing
		break;
	}
}
void informRotorTooEarly() {
	switch (activeState) {
	case controlled_positive_torque:
		;
		uint32_t rotorPos_backEMF = 0;
		uint32_t rotorPos_encoder = getRotorPosition();

		//proceedRotorPosController(rotorPos_encoder);

		//log_controllerParameterTuple(time60deg, rotorPos_backEMF, rotorPos_encoder);
		break;
	default:
		// do nothing
		break;
	}
}
void informRotorTooLate() {
	switch (activeState) {
	case controlled_positive_torque:
		;
		uint32_t rotorPos_backEMF = 60;
		uint32_t rotorPos_encoder = getRotorPosition();

		//proceedRotorPosController(rotorPos_encoder);

		//log_controllerParameterTuple(time60deg, rotorPos_backEMF,rotorPos_encoder);
		break;
	default:
		// do nothing
		break;
	}
}
void inform_newRotationDirection(uint8_t direction) {

}

void inform_tooManyZeroCrossings() {
	switch (activeState) {
	case controlled_positive_torque:
		//changeState(off);
		break;
	default:
		// do nothing
		break;
	}
}

void startup() {
	/*if (!read_MainButton()) {
	 // no real startup
	 return;
	 };*/
	started = 1;

	//switch_PowerLED(1);

	// init hardware
	initUART();
	initLogger();

	logMsg_debug("Startup BLDC driver...");

	initSystime();
	initAnalog();

	// __init services__
	initInterfaceService();
	initZeroCrossingService();
	initPhaseControllService();
	initCurrentServiceService();


	// init rest of software
	initDrive();
	initEncoderService();

	initMeasurement();
	register_rotorPosMeas_listener_ISR(&inform_newRotorPos,
			&informRotorTooEarly, &informRotorTooLate);

	setPowerLED_blinkingMode();
}

void proceed() {
	// handle invalid startup
	if (!started) {
		return;
	}

	// proceed all services
	/*proceedInterfaceService();

	switch (activeState) {
	case off: {
		int32_t torqueSP = getTorqueSetPoint();

		if (torqueSP > 0) {
			changeState(start_up);
		} else if (torqueSP < 0) {

		}
	}
		break;
	case stopped: {

	}
		break;
	case start_up:
		;
		uint32_t deltaTime = calculateDeltaTime(timestamp_entryState);
		time60deg = (TIME_60DEG_SPEED_UP_START - deltaTime / GRADIENT_DIVIDER);
		setSinusApproximation60DegTime(time60deg);

		logData_time60Deg(time60deg);

		if (time60deg < TIME_60DEG_SPEED_UP_END) {
			changeState(controlled_positive_torque);
		}
		break;
	case synchronized:
		break;
	case controlled_negative_torque:
		break;
	case controlled_positive_torque:

		break;
	case calibrate_encoder: {
		uint32_t poti = getReferencePositionEncoder();
		setReferencePosition(poti);
	}
		break;
	}*/

	//logger_writeBuffered();
}
