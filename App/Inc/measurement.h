/*
 * measurement.h
 *
 *  Created on: Dec 25, 2017
 *      Author: simon
 */

#ifndef INC_MEASUREMENT_H_
#define INC_MEASUREMENT_H_

#include <stdint.h>

void initMeasurement();
void enableMeasurement(uint8_t enable);

void register_rotorPosMeas_listener_ISR(
		void (*pNewMeasListener)(uint32_t),
		void (*pRotorTooEarlyListener)(void),
		void (*pRotorTooLateListener)(void));

void register_tooManyZeroCrossings_listener_ISR(void (*listener)(void));

#endif /* INC_MEASUREMENT_H_ */
