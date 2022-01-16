// Author: Blake Freer
// Date: January 15, 2022
// Purpose: Transition between a pattern of RGB values from serial data

#include<SoftwareSerial.h>

// Pin numbers
#define RED 8
#define GREEN 5
#define BLUE 7
#define RX 3
#define DELAY_UNIT 10   // Base delay in milliseconds

typedef unsigned long ul;

typedef struct patternElement Element;
struct patternElement {
    char r;
    char g;
    char b;
    char dur;
};

bool fade(Element* e) {
    // Fade bit stored at end of Red value
    return (e->r) & 1;
}
bool repeat(Element* e) {
    // Repeat bit stored at end of Green value
    return (e->g) & 1;
}
ul duration(Element* e) {
    // Duration is stored as a floating point integer.
    // The mantissa is the last bit of Blue and first 4 bits of duration (5 bits total)
    // While the exponent is the last 4 bits of duration
    // Represents an integer in form m * b^e, 1 <= m <= 32, 0 <= e <= 15 to be multiplied by some base unit of duration ex. 10 ms

    ul base = (((e->b) & 1) << 4) + ((e->dur) >> 4) + 1; // +1 changes range from 0->31 to 1->32
    ul exponent = (e->dur) & 0b1111;
    return base << exponent;
}

void interpolateRGB(char r1, char g1, char b1, char r2, char g2, char b2, char t){
    // Interpolate between two RGB values according to t
    // Used for fading between colours
    // t=0 => (r1, g1, b1), t=255 => (r2, g2, b2)
    analogWrite(RED, r1 + ((r2 - r1) * t / 256));
    analogWrite(GREEN, g1 + ((g2 - g1) * t / 256));
    analogWrite(BLUE, b1 + ((b2 - b1) * t / 256));
}

void setRGB(Element* e){
    // Set the PWM outputs
    analogWrite(RED, e->r);
    analogWrite(GREEN, e->g);
    analogWrite(BLUE, e->b);
}


patternElement elements[16]; // a pattern can be up to 16 elements long
//elements[0] = {0b11111110,0b00000000,0b00000001,0b10000001};
//elements[1] = {0b00000000,0b11111110,0b00000001,0b00110000};
//elements[2] = {0b00000000,0b00000000,0b11111111,0b10000010};
char patternLength = 3; // How many elements are in the pattern?

// Pattern Control Variables
int curElement = 0;
ul nextChange = 0;  // Change to next pattern element when millis() passes this value

SoftwareSerial serial(RX, 0); // TX is unused

void setup() {
    
    elements[0] = {0xFF, 0xFF, 0xFF, 0xFF}; // Default / bootup colour is white
    serial.begin(9600);
    
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    
    setRGB(elements[0]);
    nextChange = millis() + duration(elements[0]) * DELAY_UNIT;
}

void loop() {
    if(serial.available() > 0){
        patternLength = 0;
        curElement = 0;
        while(Serial.available() > 0){
            elements[patternLength] = {Serial.read(), Serial.read(), Serial.read(), Serial.read()};
            Element* e = elements[patternLength];
            patternLength++;
//            Serial.print("Red: ");
//            Serial.println(e->r, DEC);
//            Serial.print("Green: ");
//            Serial.println(e->g, DEC);
//            Serial.print("Blue: ");
//            Serial.println(e->b, DEC);
//            Serial.print("Fade: ");
//            Serial.println(fade(e), DEC);
//            Serial.print("Repeat: ");
//            Serial.println(repeat(e), DEC);
//            Serial.print("Duration (ms): ");
//            Serial.println(duration(e) * DELAY_UNIT);
//            Serial.println("*************");
        }
    }
    if(millis() > nextChange){
        curElement++;
        curElement %= patternLength;
        setRGB(elements[curElement]);
        nextChange = nextChange + duration(elements[curElement]) * DELAY_UNIT;
    }
    delay(DELAY_UNIT >> 1);
    
}
