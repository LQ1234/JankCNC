#pragma once
#include "Arduino.h"

#include "globals.h"
namespace TimerTimeout
{
    struct TimeoutInterval{
        virtual void enable()=0;
        virtual void start(bool, int)=0;
        virtual int getValue()=0;
        virtual void setValue(int)=0;
        virtual void stop()=0;
        virtual void restart()=0;
        virtual void runISR()=0;
        void (*isr)(TimeoutInterval*)=nullptr;

        int tcIdx;

        void clear(){
            stop();
            isr=nullptr;
        }
        void restartWithTime(int time){
            setValue(time);
            restart();
        }
        int timeTilTrigger(){
            return(getValue());
        }
        void setISR(void (*_isr)(TimeoutInterval*)){
            isr=_isr;
        }
    };
}
