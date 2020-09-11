#pragma once
// Based on "SerialNINAPassthrough" example
namespace NinaPassthrough{
    unsigned long baud = 115200;

    int rts = -1;
    int dtr = -1;

    void begin(){
        Serial.begin(baud);
        SerialNina.begin(baud);

        pinMode(NINA_GPIO0, OUTPUT);
        pinMode(NINA_RESETN, OUTPUT);
    }
    void loop(){
        if (rts != Serial.rts()) {
            digitalWrite(NINA_RESETN, Serial.rts() ? LOW : HIGH);
            rts = Serial.rts();
        }

        if (dtr != Serial.dtr()) {
            digitalWrite(NINA_GPIO0, (Serial.dtr() == 0) ? HIGH : LOW);
            dtr = Serial.dtr();
        }

        if (Serial.available()) {
            SerialNina.write(Serial.read());
        }

        if (SerialNina.available()) {
            Serial.write(SerialNina.read());
        }

        if (Serial.baud() != baud) {    // check if the USB virtual serial wants a new baud rate
            rts = -1;
            dtr = -1;

            baud = Serial.baud();
            SerialNina.begin(baud);
        }
    }
    void pipe(){
        if (rts != Serial.rts()) {
            digitalWrite(NINA_RESETN, Serial.rts() ? LOW : HIGH);
            rts = Serial.rts();
        }

        if (dtr != Serial.dtr()) {
            digitalWrite(NINA_GPIO0, (Serial.dtr() == 0) ? HIGH : LOW);
            dtr = Serial.dtr();
        }

        while (SerialNina.available()) {
            Serial.write(SerialNina.read());
        }

        if (Serial.baud() != baud) {    // check if the USB virtual serial wants a new baud rate
            rts = -1;
            dtr = -1;

            baud = Serial.baud();
            SerialNina.begin(baud);
        }
    }
}
