#include "globals.h"
#include "timeoutIntervalTCC.h"
#include "timeoutIntervalTC.h"
namespace TimerTimeout
{
    const bool availableTCPairs[]={true,true,false};

    //consts
    const int GCM_TC_Pairs[]={GCM_TCC0_TCC1,GCM_TCC2_TC3,GCM_TC4_TC5};
    const int CLKCTRL_GEN_GCLKs[]={GCLK_CLKCTRL_GEN_GCLK0,GCLK_CLKCTRL_GEN_GCLK1,GCLK_CLKCTRL_GEN_GCLK2,GCLK_CLKCTRL_GEN_GCLK3,GCLK_CLKCTRL_GEN_GCLK4,GCLK_CLKCTRL_GEN_GCLK5};

    const void* TCs[]={TCC0,TCC1,TCC2,TC3,TC4,TC5};
    const IRQn_Type TC_IRQns[]={TCC0_IRQn,TCC1_IRQn,TCC2_IRQn,TC3_IRQn,TC4_IRQn,TC5_IRQn};


    TimeoutInterval* timeoutIntervals[]={new TimeoutInterval_TCC(0),new TimeoutInterval_TCC(1),new TimeoutInterval_TCC(2),new TimeoutInterval_TC(3),nullptr,nullptr};
}
