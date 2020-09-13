#include <Servo.h>
namespace Mech{
    Servo servoRight;
    Servo servoLeft;
    Servo servoUp;
    Servo servoDown;

    Servo servoPen;


    const int lowerServoPin=3;
    const int lowerDetectPin=2;
    const int leftPingPin=4;
    const int leftServoPin=5;
    const int upperServoPin=8;
    const int upperDetectPin=7;
    const int rightServoPin=6;
    const int penServoPin=9;

    //painfully calibrated coeffs
    float backcoeff=.4;
    float rightleftrat=.9;

    using TimerTimeout::TimeoutInterval;
    using TimerTimeout::setTimeout;
    using TimerTimeout::setInterval;

    void setUpDown(int dir){//-90 to 90
        if(dir > 0){//go up
            servoUp.write(-dir+90);
            servoDown.write(-dir*backcoeff+90);
        }else{
            servoDown.write(-dir+90);
            servoUp.write(-dir*backcoeff+90);
        }
    }

    void setLeftRight(int dir){//-90 to 90
        if(dir > 0){//go right
            servoRight.write(-dir+90);
            servoLeft.write(dir*backcoeff*rightleftrat+90);
        }else{
            servoLeft.write(dir*rightleftrat+90);
            servoRight.write(-dir*backcoeff+90);
        }
    }


    void resetUpDown(){
        bool onLower=false;
        setUpDown(-20);
        while(!onLower){
            onLower=!digitalRead(lowerDetectPin);
            Serial.println(onLower);
        }
        bool onUpper=false;
        setUpDown(20);
        while(!onUpper){
            onUpper=!digitalRead(upperDetectPin);
        }
        setUpDown(0);
    }

    void begin() {
        servoRight.attach(rightServoPin);
        servoLeft.attach(leftServoPin);
        servoUp.attach(upperServoPin);
        servoDown.attach(lowerServoPin);

        servoPen.attach(penServoPin);

        servoRight.write(90);
        servoLeft.write(90);
        servoUp.write(90);
        servoDown.write(90);

        servoPen.write(0);

        pinMode(lowerDetectPin, INPUT_PULLUP);
        pinMode(upperDetectPin, INPUT_PULLUP);


        //resetUpDown();
        //setLeftRight(10);
        //servoRight.write(10);
        //servoLeft.write(100);
    }

    namespace ManualController{
        int dur=300;
        void begin(){

        }
        void eval(const char* cmd){

            Serial.print("Command: ");
            Serial.print(cmd);
            Serial.print("\n");

            char newlined[100];
            strcpy(newlined,cmd);
            strcat(newlined,"\n");
            printSerial(newlined);

            if(strcmp(cmd,"left")==0){
                setLeftRight(-20);
                setTimeout([](TimeoutInterval* tc){
                    setLeftRight(0);
                },dur);
            }
            if(strcmp(cmd,"right")==0){
                setLeftRight(20);
                setTimeout([](TimeoutInterval* tc){
                    setLeftRight(0);
                },dur);
            }
            if(strcmp(cmd,"tightlr")==0){
                servoLeft.write(-20+90);
                servoRight.write(-20+90);
                setTimeout([](TimeoutInterval* tc){
                    setLeftRight(0);
                },dur);
            }
            if(strcmp(cmd,"looselr")==0){
                servoLeft.write(20+90);
                servoRight.write(20+90);
                setTimeout([](TimeoutInterval* tc){
                    setLeftRight(0);
                },dur);
            }
            if(strcmp(cmd,"up")==0){
                setUpDown(20);
                setTimeout([](TimeoutInterval* tc){
                    setUpDown(0);
                },dur);
            }

            if(strcmp(cmd,"down")==0){
                setUpDown(-20);
                setTimeout([](TimeoutInterval* tc){
                    setUpDown(0);
                },dur);
            }

            if(strcmp(cmd,"tightud")==0){
                servoDown.write(-20+90);
                servoUp.write(20+90);
                setTimeout([](TimeoutInterval* tc){
                    setUpDown(0);
                },dur);
            }
            if(strcmp(cmd,"looseud")==0){
                servoDown.write(20+90);
                servoUp.write(-20+90);
                setTimeout([](TimeoutInterval* tc){
                    setUpDown(0);
                },dur);
            }

            if(strncmp(cmd,"setdur ",7)==0){
                dur=atoi(cmd+7);

                char str[5];
                itoa(dur, str, 10);

                strcpy(newlined,"Set dur to ");
                strcat(newlined,str);
                strcat(newlined,"\n");
                printSerial(newlined);
            }

            if(strcmp(cmd,"vertup")==0){
                setUpDown(20);
            }
            if(strcmp(cmd,"vertstop")==0){
                setUpDown(0);
            }
            if(strcmp(cmd,"vertdown")==0){
                setUpDown(-20);
            }


            if(strcmp(cmd,"horleft")==0){
                setLeftRight(-20);
            }
            if(strcmp(cmd,"horstop")==0){
                setLeftRight(0);
            }
            if(strcmp(cmd,"horright")==0){
                setLeftRight(20);
            }
        }
        void end(){

        }
    }
}
