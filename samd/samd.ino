#include "avr/dtostrf.h"

void printSerial(const char*);

#include "src/util.h"
#include "src/timerTimeoutInterval/timerTimeoutInterval.h"
#include "shared/message.h"
#include "src/lcd/lcd.h"
#include "src/ninaSPI/ninaSPI.h"
#include "src/mech/mech.h"
#include "src/NinaPassthrough/ninaPassthrough.h"


using TimerTimeout::TimeoutInterval;
using TimerTimeout::setTimeout;
using TimerTimeout::setInterval;

bool flagsendmsg=false;

bool passthroughMode;
const int passthroughPin=12;


float test_value;

void (*serialHandler)(char*);
void printSerial(const char* data){
    Serial.print(data);
    Pair p;
    strcpy(p.key,"serial");
    strcpy(p.value,data);
    NinaSPI::sendUpdatePair(p);
}


void setup() {

    pinMode(12, INPUT);
    passthroughMode=digitalRead(passthroughPin);
    if(passthroughMode){
        LCD::begin();
        LCD::write("Nina Passthrough");
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        NinaPassthrough::begin();
        while(true){
            NinaPassthrough::loop();
        }
    }

    //while(Serial.read()==-1);

    NinaSPI::begin();

    pinMode(LED_BUILTIN, OUTPUT);

    TimerTimeout::begin();
    Mech::begin();

    LCD::begin();

    Mech::ManualController::begin();


    NinaSPI::valueUpdateHandler=[](Pair p){
        NinaSPI::updateFloatPair(p, "test_value", test_value);

        Serial.print("SAMD: MESSAGE ");
        printbuf(p.key,20);
        Serial.print("-> ");
        printbuf(p.value,80);
        Serial.println();


        if(strcmp(p.key,"serial")==0){
            serialHandler(p.value);
        }
    };
    serialHandler=[](char* cmd){
        Mech::ManualController::eval(cmd);
    };
}

char cmd[256];
int cmdidx=0;

void loop() {
    {
        int byte = Serial.read();
        if(byte!=-1){
            if(byte=='\n'){
                cmd[cmdidx]=0;
                serialHandler(cmd);
                cmdidx=0;
            }else{
                cmd[cmdidx]=byte;
                cmdidx++;
            }
        }
    }

    NinaSPI::updateValues();

    NinaPassthrough::pipe();
    //delay(500);
}
