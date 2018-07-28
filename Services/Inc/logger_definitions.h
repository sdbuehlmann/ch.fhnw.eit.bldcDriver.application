/*
 * logger_definitions.h
 *
 *  Created on: Jul 18, 2018
 *      Author: simon
 */

#ifndef INC_LOGGER_DEFINITIONS_H_
#define INC_LOGGER_DEFINITIONS_H_

// control bytes
#define SYMBOL_START							1
#define SYMBOL_END								2

// events symbols
#define SYMBOL_MAIN_RESET 						3
#define SYMBOL_COMP_A_IRQ 						4
#define SYMBOL_COMP_B_IRQ						5
#define SYMBOL_COMP_C_IRQ 						6
#define SYMBOL_LOG_BUFFER_OVERFLOW				7

// strings headers
#define SYMBOL_MSG_DEBUG 						8
#define SYMBOL_MSG_INFO 						9
#define SYMBOL_MSG_ERROR 						10

// data headers
#define SYMBOL_CURRENT_A 						11
#define SYMBOL_CURRENT_B						12
#define SYMBOL_CURRENT_A_RANGE 					13
#define SYMBOL_CURRENT_B_RANGE 					14
#define SYMBOL_ABS_PHASECURRENT_SETPOINT		15
#define SYMBOL_ABS_PHASECURRENT					16
#define SYMBOL_DUTYCYCLE						17
#define SYMBOL_CURRENT_CONTROLER_OUT			18

#define SYMBOL_ROTORPOS_SETPOINT				19
#define SYMBOL_ROTORPOS_ENCODER_ABS				20
#define SYMBOL_ROTORPOS_ENCODER					21
#define SYMBOL_ROTORPOS_SENSORLESS				22
#define SYMBOL_TIME_60DEG						23
#define SYMBOL_ROTORPOS_CONTROLLER_OUT			24

#define SYMBOL_CYCLE_TIME						25
#define SYMBOL_SYS_TIME							26
#define SYMBOL_ENTRY_STATE						27

#endif /* INC_LOGGER_DEFINITIONS_H_ */
