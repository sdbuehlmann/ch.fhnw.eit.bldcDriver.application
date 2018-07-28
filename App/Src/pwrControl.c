/*
 * pwrControl.c
 *
 *  Created on: Apr 27, 2018
 *      Author: simon
 */

#include "pwrControl.h"
#include "logger.h"

#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"

#define MIN_DELTA_TIME_US

uint32_t dutyCycle_sp = 0;
uint32_t dutyCycle = 0;

void setPowerlevelImmediately(uint8_t powerLevel){
	if(powerLevel > MAX_POWER_LEVEL){
		powerLevel = MAX_POWER_LEVEL;
	}
	dutyCycle_sp = (powerLevel * MAX_PWM_DUTYCYCLE) / MAX_POWER_LEVEL;
	dutyCycle = dutyCycle_sp;

	set_PWM_DutyCycle(dutyCycle);
}

void setPowerlevel(uint8_t powerLevel){
	if(powerLevel > MAX_POWER_LEVEL){
		powerLevel = MAX_POWER_LEVEL;
	}
	dutyCycle_sp = (powerLevel * MAX_PWM_DUTYCYCLE) / MAX_POWER_LEVEL;
	//set_PWM_DutyCycle(dutycycle);
}

void proceedPowerControl(){
	static uint32_t timestamp;

	getTimestamp();


}

