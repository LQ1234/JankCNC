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


namespace NinaSPI{
    using TimerTimeout::TimeoutInterval;
    using TimerTimeout::setTimeout;
    using TimerTimeout::setInterval;


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

        digitalWrite(SLAVERESET, inverted_reset ? LOW : HIGH);
        delay(10);
        digitalWrite(SLAVERESET, inverted_reset ? HIGH : LOW);
        delay(750);
    }

    bool slaveselect(){

        SPIWIFI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
        digitalWrite(SLAVESELECT,LOW);

        for (unsigned long start = millis(); digitalRead(SLAVEREADY) != LOW;){
            NinaPassthrough::pipe();

            if(millis() - start > 15){
                digitalWrite(SLAVESELECT,HIGH);
                SPIWIFI.endTransaction();

                return(false);
            }
        }
        return(true);
    }
    void slavedeselect(){
        digitalWrite(SLAVESELECT,HIGH);
        SPIWIFI.endTransaction();
    }

    bool send(char* data, int datalen, char* result, int resultlen) {
        bool didtimeout=false;
        //Serial.println("SAMD: A");
        if(!slaveselect())return(false);

        /*
        Serial.print("SAMD: Sending ");
        printbuf(data,datalen);
        Serial.println();
        */

        for(int i=0;i<datalen;i++){//send
            SPIWIFI.transfer(data[i]);
        }
        //Serial.println("SAMD: Sent");

        slavedeselect();

        for (unsigned long start = millis(); (digitalRead(SLAVEREADY) != HIGH);){
            NinaPassthrough::pipe();

            if(millis() - start > 5){
                didtimeout=true;
                //Serial.println("NINA: TIMEOUT");
                break;
            }
        }
        delay(1);
        //Serial.println("SAMD: B");

        NinaPassthrough::pipe();
        if(!slaveselect())return(false);

        //Serial.println("SAMD: Receiving");
        for(int i=0;i<resultlen;i++){
            result[i]=SPIWIFI.transfer('A');
        }

        /*
        Serial.print("SAMD: Received ");
        printbuf(result,resultlen);
        Serial.println();
        */
        
        //Serial.println("SAMD: C");

        slavedeselect();
        //if(didtimeout)Serial.println("SAMD: TIMEOUT");
        return(true);
    }

    bool vertsend(char* data, int datalen, char* result, int resultlen) {
        int attempts=0;
        while((!send(data, datalen, result, resultlen))&&attempts<5)attempts++;
    }
}
#include "utils.h"
