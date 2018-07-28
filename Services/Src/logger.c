/*
 * logger.c
 *
 *  Created on: Jul 18, 2018
 *      Author: simon
 */


// =============== Includes ==============================================
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"

#include "ringbuffer.h"
#include "logger.h"
#include "logger_definitions.h"

// =============== Defines ===============================================

// =============== Variables =============================================
Ringbuffer *pRingbuffer;

// =============== Function pointers =====================================

// =============== Function declarations =================================
void addToRingbuffer_unsigned(uint32_t number);
void addToRingbuffer_signed(int32_t number);
void addToRingbuffer_msg(char *pMsg);
void addToRingbuffer_byte(char data);
void addToRingbuffer_deltaSysTime();

// =============== Functions =============================================
void initLogger() {
	pRingbuffer = allocRingbuffer(2000);
}

// messages
void logMsg_debug(char msg[]){
#ifdef LOG_MSG_DEBUG
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_MSG_DEBUG);
	addToRingbuffer_msg(msg);
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}
void logMsg_info(char msg[]){
#ifdef LOG_MSG_INFO
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_MSG_INFO);
	addToRingbuffer_msg(msg);
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}
void logMsg_error(char msg[]){
#ifdef LOG_MSG_ERROR
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_MSG_ERROR);
	addToRingbuffer_msg(msg);
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}

// events
void logEvent_mainReset(){
#ifdef LOG_STATUS_EVENTS
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_MAIN_RESET);
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}
void logEvent_logBufferOverflow(){
#ifdef LOG_STATUS_EVENTS
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_LOG_BUFFER_OVERFLOW);
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}
void logEvent_compA_IR(){
#ifdef LOG_COMP_IR_EVENTS
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_COMP_A_IRQ);
	addToRingbuffer_byte(SYMBOL_END);

	endUninterruptibleSection();
#endif
}
void logEvent_compB_IR(){
#ifdef LOG_COMP_IR_EVENTS
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_COMP_B_IRQ);
	addToRingbuffer_byte(SYMBOL_END);

	endUninterruptibleSection();
#endif
}
void logEvent_compC_IR(){
#ifdef LOG_COMP_IR_EVENTS
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_COMP_C_IRQ);
	addToRingbuffer_byte(SYMBOL_END);

	endUninterruptibleSection();
#endif
}

void logData_current(int32_t currentA, int32_t currentB){
#ifdef LOG_CURRENT_DATA
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();

	addToRingbuffer_byte(SYMBOL_CURRENT_A);
	addToRingbuffer_signed(currentA);
	addToRingbuffer_byte(SYMBOL_CURRENT_B);
	addToRingbuffer_signed(currentB);

	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}
void logData_currentController(uint32_t abs_current, uint32_t abs_current_setpoint, uint32_t dutycycle){
#ifdef LOG_CURRENT_CONTROLLER_DATA
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();

	addToRingbuffer_byte(SYMBOL_ABS_PHASECURRENT);
	addToRingbuffer_unsigned(abs_current);
	addToRingbuffer_byte(SYMBOL_ABS_PHASECURRENT_SETPOINT);
	addToRingbuffer_unsigned(abs_current_setpoint);
	addToRingbuffer_byte(SYMBOL_DUTYCYCLE);
	addToRingbuffer_unsigned(dutycycle);

	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}

void logData_rotorpos(int32_t pos_encoder, int32_t pos_sensorless){
	/* ToDo Implementieren*/
}
void logData_rotorposController(uint32_t t60Deg, uint32_t rotorpos, uint32_t rotorpos_setpoint){
	/* ToDo Implementieren*/
}
void logData_time60Deg(uint32_t t60Deg){
	/* ToDo Implementieren*/
}
void logData_encoderPos(uint32_t pos){
	/* ToDo Implementieren*/
}

void logData_cycleTime(uint32_t cycleTime){
	/* ToDo Implementieren*/
}
void logData_systime(){
#ifdef LOG
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_SYS_TIME);
	addToRingbuffer_unsigned(getTimestamp());
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}
void logData_entryNewState(uint32_t state){
#ifdef LOG
	startUninterruptibleSection();

	addToRingbuffer_byte(SYMBOL_START);
	addToRingbuffer_deltaSysTime();
	addToRingbuffer_byte(SYMBOL_ENTRY_STATE);
	addToRingbuffer_unsigned(state);
	addToRingbuffer_byte(SYMBOL_END);

	finishUninterruptibleSection();
#endif
}

// handling of the logger
void logger_writeBuffered() {
	int32_t data;
	int32_t *pData = &data;

	while (1) {
		if (bufferOut(pRingbuffer, pData) == BUFFER_EMPTY) {
			return;
		}
		sendByteOverUART((char) *pData);
	}
}

void addToRingbuffer_unsigned(uint32_t number) {
	for (uint32_t cnt = 0; cnt < 5; cnt++) {
		uint8_t temp = number & 0b01111111; // store last 7 bits
		temp = temp | 0b10000000; // set first bit

		addToRingbuffer_byte(temp);

		number = number >> 7; // unsigned --> zeros added
		if(number == 0){
			return;
		}
	}
}
void addToRingbuffer_signed(int32_t number){
	addToRingbuffer_unsigned((uint32_t)number);
}
void addToRingbuffer_msg(char *pMsg) {
	while (1) {
		if (*pMsg == 0) {
			return;
		}

		addToRingbuffer_byte(*pMsg);

		pMsg++;
	}
}
void addToRingbuffer_byte(char data) {
	if (bufferIn(pRingbuffer, (uint32_t) data) == BUFFER_OVERFLOW) {
		// reset and log
		bufferReset(pRingbuffer);
		logEvent_logBufferOverflow();
	}
}
void addToRingbuffer_deltaSysTime(){
	static uint32_t lastTimestamp;
	uint32_t timestamp = getTimestamp();

	addToRingbuffer_unsigned(timestamp-lastTimestamp);
	lastTimestamp = timestamp;
}
