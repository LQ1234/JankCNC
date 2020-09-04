#include "Arduino.h"

#include "globals.h"
#include "timeoutIntervalTCC.h"

namespace TimerTimeout
{

    TimeoutInterval_TCC::TimeoutInterval_TCC(int _tcIdx){
        tcIdx=_tcIdx;
        tc = (Tcc*) TCs[_tcIdx];
    }
    void TimeoutInterval_TCC::enable(){
    	tc->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ; // Set counter to normal freq mode
        while (tc->SYNCBUSY.bit.WAVE); // wait for sync

    	tc->CTRLA.reg |= TCC_CTRLA_PRESCALER_DIV256; // Set prescaler to 256

    	tc->INTENSET.bit.OVF = 1; // Detect overflow/underflow
    	tc->INTENSET.bit.MC0 = 0; // Don't detect match/capture for CC0
    }

    void TimeoutInterval_TCC::start(bool oneshot, int top){
        if(oneshot)tc->CTRLBSET.reg |= TCC_CTRLBSET_ONESHOT; // make oneshot
        else tc->CTRLBCLR.reg |= TCC_CTRLBCLR_ONESHOT; // remove oneshot
        while (tc->SYNCBUSY.bit.CTRLB);

        isOneShot=oneshot;

        tc->CTRLBSET.reg |= TCC_CTRLBSET_DIR; // Set direction to down
        while (tc->SYNCBUSY.bit.CTRLB);

        tc->INTFLAG.bit.OVF = 1; // clear flag

        //NVIC_SetPriority(TC_IRQns[tcIdx], 0);
        NVIC_EnableIRQ(TC_IRQns[tcIdx]); // Enable Interrupt

        tc->PER.reg = top;
        while (tc->SYNCBUSY.bit.PER);

        tc->COUNT.reg = top;
        while (tc->SYNCBUSY.bit.COUNT);

        tc->CTRLA.reg |= TCC_CTRLA_ENABLE; // Enable TC
        while (tc->SYNCBUSY.bit.ENABLE);

        restart();
    }
    int TimeoutInterval_TCC::getValue(){
        tc->CTRLBSET.reg |= TCC_CTRLBSET_CMD_READSYNC;
        while (tc->SYNCBUSY.bit.CTRLB);
        return(tc->COUNT.reg);
    }
    void TimeoutInterval_TCC::setValue(int value){
        tc->COUNT.reg = value;
        while (tc->SYNCBUSY.bit.COUNT);
    };

    void TimeoutInterval_TCC::stop(){
        //digitalWrite(13,true);

        tc->CTRLBSET.reg |= TCC_CTRLBSET_CMD_STOP;
        while (tc->SYNCBUSY.bit.CTRLB);
        NVIC_DisableIRQ(TC_IRQns[tcIdx]); // Disable Interrupt
        //digitalWrite(13,false);

    }

    void TimeoutInterval_TCC::restart() {//resets if already counting, starts from curr value if not counting
        tc->CTRLBSET.reg |= TCC_CTRLBSET_CMD_RETRIGGER;
        while (tc->SYNCBUSY.bit.CTRLB);
        NVIC_EnableIRQ(TC_IRQns[tcIdx]); // Enable Interrupt
    }

    void TimeoutInterval_TCC::runISR() {
        if (tc->INTFLAG.bit.OVF) {
            tc->INTFLAG.bit.OVF = 1;
            void (*_isr)(TimeoutInterval*) = isr;
            if(isOneShot) isr=nullptr;

            if(_isr)_isr(this);
        }
    }
}
