#pragma once
#include "Arduino.h"

#include "globals.h"
#include "timeoutInterval.h"
#include "timeoutIntervalTC.h"
#include "timeoutIntervalTCC.h"
namespace TimerTimeout
{
	void setupGenericClock(int gcIdx, int divideBy){

		REG_GCLK_GENDIV = GCLK_GENDIV_DIV(divideBy) | // Set division to divideBy
		                  GCLK_GENDIV_ID(gcIdx); // For this clock

		while (GCLK->STATUS.bit.SYNCBUSY);

		REG_GCLK_GENCTRL = GCLK_GENCTRL_IDC | // Make duty cycle 50/50
		                   GCLK_GENCTRL_GENEN | // Enable
		                   GCLK_GENCTRL_SRC_DFLL48M | // Use 48 MHz clock source
		                   GCLK_GENCTRL_ID(gcIdx); // For this clock

		while (GCLK->STATUS.bit.SYNCBUSY);
	}

	void setClockForTCPair(int tcPairIdx, int gcIdx){
		REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |  //Enable
		                   CLKCTRL_GEN_GCLKs[gcIdx] | //Use this clock
		                   GCLK_CLKCTRL_ID(GCM_TC_Pairs[tcPairIdx]); // For those TC's

		while (GCLK->STATUS.bit.SYNCBUSY);
	}

	void begin(){
		setupGenericClock(4, 187);
		for(int i=0;i<3;i++){
			if(availableTCPairs[i]) setClockForTCPair(i, 4);
		}
		for(int i=0;i<6;i++){
			if(timeoutIntervals[i]){
				timeoutIntervals[i]->enable();
			}
		}
	}

	TimeoutInterval* setTimeout(void (*isr)(TimeoutInterval*),int timeout){
		for(int i=0;i<6;i++){
			if(timeoutIntervals[i]){
				if(timeoutIntervals[i]->isr==nullptr){
					timeoutIntervals[i]->setISR(isr);
					timeoutIntervals[i]->start(true, timeout);
					break;
				}
			}
		}
		return(nullptr);
	}

	TimeoutInterval* setInterval(void (*isr)(TimeoutInterval*),int interval){
		for(int i=0;i<6;i++){
			if(timeoutIntervals[i]){
				if(timeoutIntervals[i]->isr==nullptr){
					timeoutIntervals[i]->setISR(isr);
					timeoutIntervals[i]->start(false, interval);
					return(timeoutIntervals[i]);
				}
			}
		}
		return(nullptr);
	}
};

#define CONCAT(a, b) a ## b

#define CONCAT(a, b, c) a ## b ## c
#define DEFINE_TC_ISR_HANDLER(tcIdx) \
void CONCAT(TC, tcIdx, _Handler) () { \
    TimerTimeout::timeoutIntervals[ tcIdx ]->runISR(); \
} \

#define DEFINE_TCC_ISR_HANDLER(tcIdx) \
void CONCAT(TCC, tcIdx, _Handler) () { \
    TimerTimeout::timeoutIntervals[ tcIdx ]->runISR(); \
} \

DEFINE_TCC_ISR_HANDLER(0)
DEFINE_TCC_ISR_HANDLER(1)
DEFINE_TCC_ISR_HANDLER(2)

DEFINE_TC_ISR_HANDLER(3)
