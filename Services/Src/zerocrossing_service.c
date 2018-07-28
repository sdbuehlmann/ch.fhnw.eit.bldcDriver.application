/*
 * zerocrossing_service.c
 *
 *  Created on: Dec 22, 2017
 *      Author: simon
 */
// =============== Defines ===============================================
#include "bldc_driver_functions.h"
#include "bldc_driver_HAL.h"
#include "logger.h"

// =============== Defines ===============================================

// =============== Variables =============================================
uint8_t lastEdge_A = 0;
uint8_t lastEdge_B = 0;
uint8_t lastEdge_C = 0;

// =============== Function pointers =====================================
static void (*pListener_zeroCrossing_phaseA)(volatile uint8_t);
static void (*pListener_zeroCrossing_phaseB)(volatile uint8_t);
static void (*pListener_zeroCrossing_phaseC)(volatile uint8_t);

// =============== Function declarations =================================
static void handle_comparator_A(volatile uint8_t edge);
static void handle_comparator_B(volatile uint8_t edge);
static void handle_comparator_C(volatile uint8_t edge);

static void handle_delayedCallback_A();
static void handle_delayedCallback_B();
static void handle_delayedCallback_C();

// =============== Functions =============================================
void initZeroCrossingService() {
	logMsg_debug("zero crossing service initialized.");

	register_comperatorListener_phaseA(&handle_comparator_A);
	register_comperatorListener_phaseB(&handle_comparator_B);
	register_comperatorListener_phaseC(&handle_comparator_C);
}

void registerListener_zeroCrossing_phaseA(void (*listener)(volatile uint8_t)){
	pListener_zeroCrossing_phaseA = listener;
}
void registerListener_zeroCrossing_phaseB(void (*listener)(volatile uint8_t)){
	pListener_zeroCrossing_phaseB = listener;
}
void registerListener_zeroCrossing_phaseC(void (*listener)(volatile uint8_t)){
	pListener_zeroCrossing_phaseC = listener;
}

void registerListener_zeroCrossing(
				void (*listener_phaseA)(volatile uint8_t),
				void (*listener_phaseB)(volatile uint8_t),
				void (*listener_phaseC)(volatile uint8_t)){
	registerListener_zeroCrossing_phaseA(listener_phaseA);
	registerListener_zeroCrossing_phaseB(listener_phaseB);
	registerListener_zeroCrossing_phaseC(listener_phaseC);
}

void enableZeroCrossingIRQ(uint8_t phase, uint8_t enable) {
	switch (phase) {
	case PHASE_A:
		enableCompA(enable);
		break;
	case PHASE_B:
		enableCompB(enable);
		break;
	case PHASE_C:
		enableCompC(enable);
		break;
	}
}

uint8_t readStatusOfZeroCrossingSignal(uint8_t phase) {
	switch (phase) {
	case PHASE_A:
		return read_signal_compA();
	case PHASE_B:
		return read_signal_compB();
	case PHASE_C:
		return read_signal_compC();
	}

	return 0;
}

// zero crossing ISR handler
void handle_comparator_A(volatile uint8_t edge) {
	if(isBusy_delayedCallback_A() == DELAYED_CALLBACK_IS_READY){
		delayedCallback_A(TIME_DELAY_INTERRUPT_FILTER, &handle_delayedCallback_A);
		lastEdge_A = edge;
	}

}
void handle_delayedCallback_A(){
	if(read_signal_compA() == lastEdge_A){
		// stable signal --> valid zero crossing
		pListener_zeroCrossing_phaseA(lastEdge_A);
	}
}

void handle_comparator_B(volatile uint8_t edge) {
	if(isBusy_delayedCallback_B() == DELAYED_CALLBACK_IS_READY){
		delayedCallback_B(TIME_DELAY_INTERRUPT_FILTER, &handle_delayedCallback_B);
		lastEdge_B = edge;
	}
}
void handle_delayedCallback_B(){
	if(read_signal_compB() == lastEdge_B){
		// stable signal --> valid zero crossing
		pListener_zeroCrossing_phaseB(lastEdge_B);
	}
}

void handle_comparator_C(volatile uint8_t edge) {
	if(isBusy_delayedCallback_C() == DELAYED_CALLBACK_IS_READY){
		delayedCallback_C(TIME_DELAY_INTERRUPT_FILTER, &handle_delayedCallback_C);
		lastEdge_C = edge;
	}
}
void handle_delayedCallback_C(){
	if(read_signal_compC() == lastEdge_C){
		// stable signal --> valid zero crossing
		pListener_zeroCrossing_phaseC(lastEdge_C);
	}
}
