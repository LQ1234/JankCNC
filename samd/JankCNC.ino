#include "src/timerTimeoutInterval/timerTimeoutInterval.h"
#include "src/lcd/lcd.h"
#include "src/web/web.h"
#include "src/mech/mech.h"
#include "src/NinaPassthrough/ninaPassthrough.h"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__


using TimerTimeout::TimeoutInterval;
using TimerTimeout::setTimeout;
using TimerTimeout::setInterval;

bool flagsendmsg=false;
bool passthroughMode;
const int passthroughPin=12;
void setup() {
    pinMode(12, INPUT);
    passthroughMode=digitalRead(passthroughPin);
    if(passthroughMode){
        LCD::begin();
        LCD::write("Nina Passthrough");
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        NinaPassthrough::begin();
    }else{
        pinMode(LED_BUILTIN, OUTPUT);
        Serial.begin(9600);

        TimerTimeout::begin();
        Mech::begin();

        //setTimeout([](TimeoutInterval* tc){
        //},0);
        //Mech::resetUpDown();

        LCD::begin();
        Web::begin();

        Mech::ManualController::begin();

        setInterval([](TimeoutInterval* ti){
            flagsendmsg=true;
        },100);
    }
}

char cmd[256];
int cmdidx=0;
void loop() {
    if(passthroughMode){
        NinaPassthrough::loop();

    }else{
        {
            int byte = Serial.read();
            if(byte!=-1){
                if(byte=='\n'){
                    cmd[cmdidx]=0;
                    //Serial.print("Command: ");
                    //Serial.println(cmd);
                    Mech::ManualController::eval(cmd);

                    cmdidx=0;
                }else{
                    cmd[cmdidx]=byte;
                    cmdidx++;
                }
            }
        }

        //Serial.println(Web::isEventStreamClientConnected());
        Web::checkForClient();


        if(flagsendmsg)Web::sendEvent("fred","hola");
        flagsendmsg=false;
    }
}
