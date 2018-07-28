/*
 * drive_state.h
 *
 *  Created on: Dec 22, 2017
 *      Author: simon
 */

#ifndef INC_DRIVE_STATE_H_
#define INC_DRIVE_STATE_H_

#include <stdint.h>

void startFreeRunning(void (*freeRunningStopedCallback)(void));
void stopFreeRunning(void);

void startStartUp(void (*startupFinishedCallback)(uint32_t));
void proceedStartUp();
void stopStartUp();

void startControlled(uint32_t time60deg_init, void (*tooSlowCallback)(void));
void stopControlled(void);
void informRotorPos_controlled(uint32_t rotorpos);
void informRotorTooEarly_controlled();
void informRotorTooLate_controlled();

#endif /* INC_DRIVE_STATE_H_ */
