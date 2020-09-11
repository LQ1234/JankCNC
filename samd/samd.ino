#include "avr/dtostrf.h"

#include "src/timerTimeoutInterval/timerTimeoutInterval.h"
#include "shared/message.h"
#include "src/lcd/lcd.h"
#include "src/ninaSPI/ninaSPI.h"
#include "src/mech/mech.h"
#include "src/NinaPassthrough/ninaPassthrough.h"
#include "src/util.h"


using TimerTimeout::TimeoutInterval;
using TimerTimeout::setTimeout;
using TimerTimeout::setInterval;

bool flagsendmsg=false;

bool passthroughMode;
const int passthroughPin=12;


float test_value;
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

    while(Serial.read()==-1);

    NinaSPI::begin();

    pinMode(LED_BUILTIN, OUTPUT);

    TimerTimeout::begin();
    Mech::begin();

    LCD::begin();

    Mech::ManualController::begin();

    NinaSPI::valueUpdateHandler=[](Pair p){
        NinaSPI::updateFloatPair(p, "test_value", test_value);
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

    NinaSPI::updateValues();
    /*
    {
        SPIMsgCommand cmd;
        cmd.type=SPIMsgType::ECHO;
        for(int i=0;i<100;i++){
            cmd.data.echo[i]=i;
        }

        SPIMsgResult res;
        memset(reinterpret_cast<char*>(&res),'d',sizeof(res));
        Serial.println("SAMD: Doing echo");
        NinaSPI::vertsend(reinterpret_cast<char*>(&cmd),sizeof(cmd),reinterpret_cast<char*>(&res),sizeof(res));

        for(int i=0;i<100;i++){
            if(cmd.data.echo[i]!=res.data.echo[i]){
                Serial.print("SAMD: Echo Failed\n  -A- ");
                printbuf(cmd.data.echo,sizeof(cmd.data.echo));
                Serial.print("\n  -B- ");
                printbuf(res.data.echo,sizeof(res.data.echo));
                Serial.println();
                break;
            }
        }
    }*/
    NinaPassthrough::pipe();

}
