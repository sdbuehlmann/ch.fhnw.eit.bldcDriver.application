/*
 * bldc_driver_functions.h
 *
 *  Created on: Nov 29, 2017
 *      Author: simon
 */

#ifndef INC_BLDC_DRIVER_FUNCTIONS_H_
#define INC_BLDC_DRIVER_FUNCTIONS_H_

#include <stdint.h>

//======================= INTERFACE SERVICE ==================================
void initInterfaceService();
void proceedInterfaceService();

uint8_t getDebouncedMainSwitchState();
uint8_t getDebouncedStateSwitchState();
uint8_t getDebouncedEnableEncoderJumper();
uint8_t getDebouncedCalibrateJumper();

int32_t getTorqueSetPoint();

uint32_t getUserInValue();

void setPowerLED_blinkingMode();
void setPowerLED_continiousMode();

/** Returns the calibrated reference position of the encoder.
 *
 * @return Value in interval  [0-360]
 */
uint32_t getReferencePositionEncoder();

//========================= TIME ===================================
void initTimeService();

uint32_t getTimestamp();
uint32_t calculateDeltaTime(uint32_t start_timestamp);

uint8_t unblockingDelay(uint32_t time_us, void (*listener)(void));

//========================= PHASE CONTROLL ===================================
void initPhaseControllService();

#define START_SIN_APPROX_FORWARD 0
#define START_SIN_APPROX_BACKWARD 1
#define STOP_SIN_APPROX 2
void control3PhaseSinusApproximation(uint8_t start_stop_selecter);
void setSinusApproximation60DegTime(uint32_t t60Deg);
uint32_t getSinusApproximation60DegTime();

#define SECTION_0_ACTIVE 0
#define SECTION_1_ACTIVE 1
#define SECTION_2_ACTIVE 2
#define SECTION_3_ACTIVE 3
#define SECTION_4_ACTIVE 4
#define SECTION_5_ACTIVE 5
#define NO_SECTION_ACTIVE 6
/** Returns the active section.
 *  Return:
    state = 0: A heavyside, C lowside
    state = 1: B heavyside, C lowside
    state = 2: B heavyside, A lowside
    state = 3: C heavyside, A lowside
    state = 4: C heavyside, B lowside
    state = 5: A heavyside, B lowside
    state > 5: power off all channels
 */
uint8_t getActiveSection();

/** Returns the active phase control state.
 *  	Return = 0: sinus approximation is running forward
 *  	Return = 1: sinus approximation is running backward
 *  	Return = 2: sinus approximation is stopped
 */
uint8_t getPhasecontrolState();


void registerListener_sectionEnds_ISR(void (*pListener)(uint8_t section, uint8_t nextSection));

//========================= ZERO CROSSING ===================================
#define FALLING_EDGE ZERO_CROSSING_SIGNAL_LOW
#define RISING_EDGE ZERO_CROSSING_SIGNAL_HIGH
#define ZERO_CROSSING_SIGNAL_LOW 0
#define ZERO_CROSSING_SIGNAL_HIGH 1

#define TIME_DELAY_INTERRUPT_FILTER 100

#define PHASE_A 'A'
#define PHASE_B 'B'
#define PHASE_C 'C'

void initZeroCrossingService();

void registerListener_zeroCrossing_phaseA(void (*listener)(volatile uint8_t));
void registerListener_zeroCrossing_phaseB(void (*listener)(volatile uint8_t));
void registerListener_zeroCrossing_phaseC(void (*listener)(volatile uint8_t));

void registerListener_zeroCrossing(
				void (*listener_phaseA)(volatile uint8_t),
				void (*listener_phaseB)(volatile uint8_t),
				void (*listener_phaseC)(volatile uint8_t));

void enableZeroCrossingIRQ(uint8_t phase, uint8_t enable);
uint8_t readStatusOfZeroCrossingSignal(uint8_t phase);

//========================= ENCODER ===================================
void initEncoderService();
void setReferencePosition(uint32_t position);

void changeToCalibrationMode();

#define ENCODER_UNREFERENCED -1
/* Returns the electrical position of the rotor in degrees.
 *
 * @return  Value in interval  [0-357] or ENCODER_UNREFERENCED when encoder is unreferenced.
 */
int32_t getRotorPosition();
int32_t getRotorPositionSetPointOfSectionInZeroCrossing(uint8_t section);

//========================= CURRENT ===================================
void initCurrentServiceService();

void startCurrentMeasurement();

//======================= SYNC ========================================
void startUninterruptibleSection();
void finishUninterruptibleSection();

#endif /* INC_BLDC_DRIVER_FUNCTIONS_H_ */
