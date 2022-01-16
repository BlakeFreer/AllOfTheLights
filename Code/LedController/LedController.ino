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
    byte r;
    byte g;
    byte b;
    byte dur;
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

    ul base = (((e->b) & 1) * 16) + ((e->dur) >> 4) + 1; // +1 changes range from 0->31 to 1->32
    byte bl = e->b;
//    Serial.print("BLUE: "); Serial.println(bl, BIN);
    ul exponent = (e->dur) & 0b1111;
//    Serial.print("DUR: "); Serial.println(e->dur, DEC);
    return (base << exponent) * DELAY_UNIT;
}

void interpolateRGB(byte r1, byte g1, byte b1, byte r2, byte g2, byte b2, byte t){
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
byte patternLength; // How many elements are in the pattern?

// Pattern Control Variables
int curElementIndex = 0;
Element* curElement;
ul nextChange = 0;  // Change to next pattern element when millis() passes this value
ul fadeStart = 0;   // Start fading (if applicable) when this time is reached

SoftwareSerial serial(RX, 0); // TX is unused

void setup() {

    elements[0] = {0b11111110,0b11111110,0b11111111,0b11111111};
        
    patternLength = 1;
    serial.begin(9600);
    
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    
    curElementIndex = -1;
    nextChange = 0;
}

void loop() {
    if(serial.available() > 0){
        patternLength = 0;
        while(serial.available() > 0){
            // Read in bytes. Ensure that the next byte has arrived before trying to read it
            elements[patternLength].r = serial.read(); while(serial.available() == 0) delay(1);
            elements[patternLength].g = serial.read(); while(serial.available() == 0) delay(1);
            elements[patternLength].b = serial.read(); while(serial.available() == 0) delay(1);
            elements[patternLength].dur = serial.read(); delay(5); // Delay at end to ensure no more bytes are incoming
            
            patternLength++;
            if(patternLength >= 16) break; // There are only 16 element spaces so cut off at 16
        }
        nextChange = millis(); // Change immediately upon receving new instruction
        curElementIndex = -1; // Will become 0 (first element) immediately after this
    }
    if(millis() > nextChange){
        curElementIndex++;
        curElementIndex %= patternLength;
        curElement = &elements[curElementIndex];
        setRGB(curElement);
        nextChange = nextChange + duration(curElement);
        fadeStart = nextChange - (duration(curElement) >> 1); // Fade will be the last 1/2 of the duration (if applicable)
    }
    if(fade(curElement)){
        if(millis() > fadeStart){
            // Fade between current colour and next
            // use map() to convert range [fadeStart < millis < nextChange] to [0 < t < 255]
            Element* nextElement = &elements[(curElementIndex + 1) % patternLength];
            // Use interpolate to set RGB pin outputs
            interpolateRGB(curElement->r, curElement->g, curElement->b, nextElement->r, nextElement->g, nextElement->b, map(millis(), fadeStart, nextChange, 0, 255));
        }
    }
    delay(5);
    
}
