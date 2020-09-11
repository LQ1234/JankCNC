#include "Arduino.h"

#include "globals.h"
#include "timeoutIntervalTC.h"

namespace TimerTimeout
{
    TimeoutInterval_TC::TimeoutInterval_TC(int _tcIdx){
        tcIdx=_tcIdx;
        tc = (TcCount16*) TCs[_tcIdx];
    }
    void TimeoutInterval_TC::enable() {
        tc->CTRLA.reg |= TC_CTRLA_MODE_COUNT16; // Set counter to 16 bit mode
        while (tc->STATUS.bit.SYNCBUSY);

        tc->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set counter to match freq mode
        while (tc->STATUS.bit.SYNCBUSY);

        tc->CTRLA.reg |= TC_CTRLA_PRESCALER_DIV256; // Set prescaler to 256
        while (tc->STATUS.bit.SYNCBUSY);


        tc->INTENSET.bit.OVF = 1; // Detect overflow/underflow
        tc->INTENSET.bit.MC0 = 0; // Don't detect match/capture for CC0
    }
    void TimeoutInterval_TC::start(bool oneshot, int top){
        if(oneshot) tc->CTRLBSET.reg |= TC_CTRLBSET_ONESHOT; // make oneshot
        else tc->CTRLBCLR.reg |= TC_CTRLBCLR_ONESHOT; // remove oneshot
        while (tc->STATUS.bit.SYNCBUSY);

        isOneShot=oneshot;

        tc->CTRLBSET.reg |= TC_CTRLBSET_DIR; // Set direction to down
        while (tc->STATUS.bit.SYNCBUSY);

        tc->INTFLAG.bit.OVF = 1; // clear flag

        //NVIC_SetPriority(TC_IRQns[tcIdx], 0);
        NVIC_EnableIRQ(TC_IRQns[tcIdx]); // Enable Interrupt

        tc->CC[0].reg = top;
        tc->COUNT.reg = top;
        while (tc->STATUS.bit.SYNCBUSY);

        tc->CTRLA.reg |= TC_CTRLA_ENABLE; // Enable TC
        while (tc->STATUS.bit.SYNCBUSY);

        restart();
    }
    int TimeoutInterval_TC::getValue() {
        tc->READREQ.reg = TC_READREQ_RREQ;
        while (tc->STATUS.bit.SYNCBUSY);
        return(tc->COUNT.reg);
    }

    void TimeoutInterval_TC::setValue(int value){
        tc->COUNT.reg = value;
        while (tc->STATUS.bit.SYNCBUSY);
    }
    void TimeoutInterval_TC::stop() {
        //digitalWrite(13,true);

        tc->CTRLBSET.reg |= TC_CTRLBSET_CMD_STOP;
        while (tc->STATUS.bit.SYNCBUSY);
        NVIC_DisableIRQ(TC_IRQns[tcIdx]); // Disable Interrupt
        //digitalWrite(13,false);

    }

    void TimeoutInterval_TC::restart(){//resets if already counting, starts from curr value if not counting
        tc->CTRLBSET.reg |= TC_CTRLBSET_CMD_RETRIGGER;
        while (tc->STATUS.bit.SYNCBUSY);
        NVIC_EnableIRQ(TC_IRQns[tcIdx]); // Enable Interrupt
    }

    void TimeoutInterval_TC::runISR() {
        if (tc->INTFLAG.bit.OVF) {
            tc->INTFLAG.bit.OVF = 1;
            void (*_isr)(TimeoutInterval*) = isr;
            if(isOneShot) isr=nullptr;

            if(_isr)_isr(this);
        }
    }
}
