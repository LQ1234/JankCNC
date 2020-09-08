#pragma once

#include "Arduino.h"
#include <SPI.h>
#include "../timerTimeoutInterval/timerTimeoutInterval.h"
#include "../NinaPassthrough/ninaPassthrough.h"


#ifdef SPIWIFI_SS
static uint8_t SLAVESELECT = SPIWIFI_SS;
#else
static uint8_t SLAVESELECT = 10;
#endif

#ifdef SPIWIFI_ACK
static uint8_t SLAVEREADY = SPIWIFI_ACK;
#else
static uint8_t SLAVEREADY = 7;
#endif

#ifdef SPIWIFI_RESET
static uint8_t SLAVERESET = (uint8_t)SPIWIFI_RESET;
#else
static uint8_t SLAVERESET  = 5;
#endif

static bool inverted_reset = false;


namespace Web{
    using TimerTimeout::TimeoutInterval;
    using TimerTimeout::setTimeout;
    using TimerTimeout::setInterval;

    bool send=false;
    bool lines=false;

    void begin(){

        #ifdef ARDUINO_SAMD_MKRVIDOR4000
              inverted_reset = false;
        #else
              if (SLAVERESET > PINS_COUNT) {
                inverted_reset = true;
                SLAVERESET = ~SLAVERESET;
              }
        #endif

        NinaPassthrough::begin();

        SPIWIFI.begin();
        TimerTimeout::begin();

        pinMode(SLAVESELECT, OUTPUT);
        pinMode(SLAVEREADY, INPUT);
        pinMode(SLAVERESET, OUTPUT);

        while(Serial.read()==-1);
        Serial.println(digitalRead(SLAVEREADY)?"high":"low");

        digitalWrite(SLAVERESET, inverted_reset ? LOW : HIGH);
        delay(10);
        digitalWrite(SLAVERESET, inverted_reset ? HIGH : LOW);
        delay(750);


        setInterval([](TimeoutInterval* ti){
            send=true;
            setTimeout([](TimeoutInterval* ti){
                lines=true;
            },750);
        },1500);
    }


    void loop(){
        char data[]="message from samdPAD";
        while(1){

            //Serial.println(digitalRead(SLAVEREADY)?"high":"low");
            NinaPassthrough::pipe();

            if(send){

                bool flag_to;




                SPIWIFI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
                digitalWrite(SLAVESELECT,LOW);
                NinaPassthrough::pipe();

                while (!(digitalRead(SLAVEREADY) == LOW))NinaPassthrough::pipe();

                /*
                for (unsigned long start = millis(); (digitalRead(SLAVEREADY) != HIGH);){
                    if(millis() - start > 5){
                        flag_to=true;
                        break;
                    }
                }
                if(flag_to){
                    digitalWrite(SLAVESELECT,HIGH);
                    SPIWIFI.endTransaction();
                    continue;
                }*/
                Serial.println("SAMD: SENDING");
                for(int i=0;i<20;i++){
                    SPIWIFI.transfer(data[i]);
                }

                Serial.println("SAMD: SENT");

                digitalWrite(SLAVESELECT,HIGH);
                SPIWIFI.endTransaction();

                NinaPassthrough::pipe();


                delay(1);
                while (!(digitalRead(SLAVEREADY) == LOW))NinaPassthrough::pipe();




                SPIWIFI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
                digitalWrite(SLAVESELECT,LOW);

                NinaPassthrough::pipe();



                Serial.println("SAMD: RECEIVING");

                char data[20+1];
                data[20]=0;
                memset(data,'C',20);
                for(int i=0;i<20;i++){
                    data[i]=SPIWIFI.transfer('A');
                }
                Serial.print("SAMD: ");
                Serial.print(data);
                Serial.println();

                digitalWrite(SLAVESELECT,HIGH);
                SPIWIFI.endTransaction();

                send=false;


            }

            if(lines){
                Serial.println();
                Serial.println("----------------------------");
                Serial.println();
                lines=false;
            }

            //Serial.println("bruh");


        }
    }
}
