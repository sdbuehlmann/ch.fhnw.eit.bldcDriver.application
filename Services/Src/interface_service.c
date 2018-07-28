/*
 * interface_service.c
 *
 *  Created on: Nov 30, 2017
 *      Author: simon
 */
// =============== Includes ==============================================
#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"

// =============== Defines ===============================================
#define DEBOUNCE_HYSTERESIS 10
#define PWR_LED_BLINKING_TIME 100000

// =============== Variables =============================================
uint8_t mainSwitchState = 0;
uint8_t stateSwitchState = 0;
uint8_t calibrateJumperState = 0;
uint8_t enableEncoderJumperState = 0;

uint32_t userInValue = 0;

uint8_t pwrLEDinBlinkingMode = 0;

// =============== Function pointers =====================================

// =============== Function declarations =================================

// =============== Functions =============================================
void debounceMainSwitch() {
	static int8_t cnt = 0;

	if (read_MainButton()) {
		cnt++;
	} else {
		cnt--;
	}

	if (cnt <= 0) {
		cnt = 0;
		mainSwitchState = 0;
	} else if (cnt >= DEBOUNCE_HYSTERESIS) {
		cnt = DEBOUNCE_HYSTERESIS;
		mainSwitchState = 1;
	}
}

void debounceStateSwitch() {
	static int8_t cnt = 0;

	if (read_StateButton()) {
		cnt++;
	} else {
		cnt--;
	}

	if (cnt <= 0) {
		cnt = 0;
		stateSwitchState = 0;
	} else if (cnt >= DEBOUNCE_HYSTERESIS) {
		cnt = DEBOUNCE_HYSTERESIS;
		stateSwitchState = 1;
	}
}

void debounceCalibrationJumper() {
	static int8_t cnt = 0;

	if (read_encoderCalibrate()) {
		cnt++;
	} else {
		cnt--;
	}

	if (cnt <= 0) {
		cnt = 0;
		calibrateJumperState = 0;
	} else if (cnt >= DEBOUNCE_HYSTERESIS) {
		cnt = DEBOUNCE_HYSTERESIS;
		calibrateJumperState = 1;
	}
}

void debounceEncoderJumper() {
	static int8_t cnt = 0;

	if (read_encoderEnable()) {
		cnt++;
	} else {
		cnt--;
	}

	if (cnt <= 0) {
		cnt = 0;
		enableEncoderJumperState = 0;
	} else if (cnt >= DEBOUNCE_HYSTERESIS) {
		cnt = DEBOUNCE_HYSTERESIS;
		enableEncoderJumperState = 1;
	}
}

void pollAnalogUserInput() {
	if (isMeasReady_userVolatgeMeas) {
		userInValue = getLastMeas_userVolatgeMeas();
		start_userVolatgeMeas();
	}
}

void readOutBridgeDriverPins() {
	// fault report indicator
	uint8_t nfault = read_NFault_BridgeDriver();

	// overcurrent and over temperature warning
	uint8_t noctw = read_NOCTW_BridgeDriver();

	// buck output voltage is low
	uint8_t pwrgd = read_PWRGD_BridgeDriver();
}

void handle_togglePwrLED() {
	static uint8_t lastState = 0;

	lastState = !lastState;
	switch_PowerLED(lastState);

	if (pwrLEDinBlinkingMode) {
		delayedCallback_B(PWR_LED_BLINKING_TIME, &handle_togglePwrLED);
	}
}

// =============== h-File Functions =======================================
void proceedInterfaceService() {
	debounceMainSwitch();
	debounceStateSwitch();
	debounceCalibrationJumper();
	debounceEncoderJumper();

	pollAnalogUserInput();
}

void initInterfaceService() {
	uint8_t cnt;
	for(cnt = 0; cnt<=DEBOUNCE_HYSTERESIS; cnt++){
		proceedInterfaceService();
	}
}

uint8_t getDebouncedMainSwitchState() {
	return mainSwitchState;
}
uint8_t getDebouncedStateSwitchState() {
	return stateSwitchState;
}
uint8_t getDebouncedEnableEncoderJumper(){
	return enableEncoderJumperState;
}
uint8_t getDebouncedCalibrateJumper(){
	return calibrateJumperState;
}

uint32_t getUserInValue() {
	return userInValue;
}

uint32_t getReferencePositionEncoder() {
	uint32_t poti = measAnalog_encoderCalibrationPoti_BLOCKING();

	uint32_t value = poti * 360;
	/* check overflow:
	 * max. size of poti: 12bit
	 * 360 = 9bit
	 * max. size of value: 12 + 9 = 21bit --> no overflow
	 */
	value = value / MAX_CALIBRATION_POTI_VALUE;
	return value;
}

int32_t getTorqueSetPoint(){
	return 100;
}

void setPowerLED_blinkingMode() {
	pwrLEDinBlinkingMode = 1;
	handle_togglePwrLED();
}

void setPowerLED_continiousMode() {
	pwrLEDinBlinkingMode = 0;
	switch_PowerLED(1);
}
