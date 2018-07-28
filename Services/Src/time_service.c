/*
 * time_service.c
 *
 *  Created on: Dec 15, 2017
 *      Author: simon
 */

#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"

typedef struct {
	uint8_t (*callback)(uint32_t time_us, void (*listener)(void));
	uint8_t (*isBusy)(uint32_t time_us, void (*listener)(void));
	uint8_t (*abort)(uint32_t time_us, void (*listener)(void));
} UnblockingDelay;

UnblockingDelay pDelays[4];

void initTimeService() {
	/*pDelays[0]->callback = &delayedCallback_A;
	pDelays[0]->isBusy = &isBusy_delayedCallback_A;
	pDelays[0]->abort = &abort_delayedCallback_A;

	pDelays[1]->callback = &delayedCallback_B;
	pDelays[1]->isBusy = &isBusy_delayedCallback_B;
	pDelays[1]->abort = &abort_delayedCallback_B;

	pDelays[2]->callback = &delayedCallback_C;
	pDelays[2]->isBusy = &isBusy_delayedCallback_C;
	pDelays[2]->abort = &abort_delayedCallback_C;

	pDelays[3]->callback = &delayedCallback_D;
	pDelays[3]->isBusy = &isBusy_delayedCallback_D;
	pDelays[3]->abort = &abort_delayedCallback_D;*/
}

uint32_t getTimestamp() {
	return getSystimeUs();
}

uint32_t calculateDeltaTime(uint32_t start_timestamp) {
	return getSystimeUs() - start_timestamp;
}

uint8_t unblockingDelay(uint32_t time_us, void (*listener)(void)) {
	// search unused delayed callback function
	for (uint8_t cnt = 0; cnt < 4; cnt++) {
	/*if(pDelays[cnt]->isBusy == DELAYED_CALLBACK_IS_READY){

	}*/
}
}

/*uint8_t delayedCallback_A(uint32_t time_us, void (*listener)(void));
 uint8_t delayedCallback_B(uint32_t time_us, void (*listener)(void));
 uint8_t delayedCallback_C(uint32_t time_us, void (*listener)(void));
 uint8_t delayedCallback_D(uint32_t time_us, void (*listener)(void));

 uint8_t isBusy_delayedCallback_A();
 uint8_t isBusy_delayedCallback_B();
 uint8_t isBusy_delayedCallback_C();
 uint8_t isBusy_delayedCallback_D();

 void abort_delayedCallback_A();*/
