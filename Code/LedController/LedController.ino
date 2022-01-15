// Author: Blake Freer
// Date: January 15, 2022
// Purpose: Transition between a pattern of RGB values from serial data

// PWN Output pins
#define RED 3
#define GREEN 5
#define BLUE 6

#define DELAY_UNIT 10   // Base delay is 10ms

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

    ul base = ((e->b) & 1) << 4 + (e->dur) >> 4 + 1; // +1 changes range from 0->31 to 1->32
    ul exponent = (e->dur) & 0b1111;
    return dur = base << exponent;
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


Element elements[16]; // a pattern can be up to 16 elements long
elements[0] = {0xFF, 0xFF, 0xFF, 0xFF}; // Default / bootup colour is white
char patternLength = 1; // How many elements are in the pattern?

// Pattern Control Variables
int curElement = 0;
ul nextChange = 0;  // Change to next pattern element when millis() passes this value

void setup() {
    Serial.begin(9600);
    
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    nextChange = millis();
    setRGB(patternElements[0]);
}

void loop() {
    if(Serial.available() > 0){
        patternLength = 0;
        curElement = 0;
        while(Serial.available() > 0){
            elements[patternLength] = {Serial.read(), Serial.read(), Serial.read(), Serial.read()};
            patternLength++;
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
