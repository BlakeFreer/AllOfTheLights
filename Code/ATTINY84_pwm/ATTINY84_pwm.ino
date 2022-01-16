// Author: Blake Freer
// Date: January 15, 2022
// Purpose: Test PWM and Serial on the ATTINY84

#include<SoftwareSerial.h>

#define RED 8
#define GREEN 5
#define BLUE 7
#define RX 3

SoftwareSerial serial(RX, 0); // TX = 0 is unused

void setup() {

    serial.begin(9600);
    
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    
}

char brightness = 0;
char ser = 0;

void loop() {

    if(serial.available() > 0){
         ser = serial.read();
    }
    // Turn white if last bit was odd, otherwise hold current brightness
    analogWrite(RED, brightness);
    analogWrite(GREEN, brightness);
    analogWrite(BLUE, brightness);
    brightness += ser;
    col %= 256;
    delay(50);
}
