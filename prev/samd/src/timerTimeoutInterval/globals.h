#pragma once

#include "Arduino.h"
#include "timeoutInterval.h"
namespace TimerTimeout
{
    struct TimeoutInterval;

    extern const bool availableTCPairs[];

    //consts
    extern const int GCM_TC_Pairs[];
    extern const int CLKCTRL_GEN_GCLKs[];

    extern const void* TCs[];
    extern const IRQn_Type TC_IRQns[];

    //internal
    extern TimeoutInterval* timeoutIntervals[6];
}
