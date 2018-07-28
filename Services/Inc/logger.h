/*
 * logger.h
 *
 *  Created on: Jul 18, 2018
 *      Author: simon
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

// general
#define LOG

// messages
#define LOG_MSG_DEBUG
#define LOG_MSG_INFO
#define LOG_MSG_ERROR

// events
#define LOG_STATUS_EVENTS
//#define LOG_COMP_IR_EVENTS

// data
#define LOG_CURRENT_DATA
#define LOG_CURRENT_CONTROLLER_DATA


//#define LOG_TIME60DEG
//#define LOG_CYCLETIME
//#define LOG_MEAS_ZEROCROSSING

void initLogger();

void logMsg_debug(char msg[]);
void logMsg_info(char msg[]);
void logMsg_error(char msg[]);

void logEvent_mainReset();
void logEvent_compA_IR();
void logEvent_compB_IR();
void logEvent_compC_IR();
void logEvent_logBufferOverflow();

void logData_current(int32_t currentA, int32_t currentB);

void logData_currentController(uint32_t abs_current, uint32_t abs_current_setpoint, uint32_t dutycycle);

void logData_rotorpos(int32_t pos_encoder, int32_t pos_sensorless);
void logData_rotorposController(uint32_t t60Deg, uint32_t rotorpos, uint32_t rotorpos_setpoint);
void logData_time60Deg(uint32_t t60Deg);
void logData_encoderPos(uint32_t pos);

void logData_cycleTime(uint32_t cycleTime);
void logData_systime();
void logData_entryNewState(uint32_t state);


void logger_writeBuffered();

#endif /* INC_LOGGER_H_ */
