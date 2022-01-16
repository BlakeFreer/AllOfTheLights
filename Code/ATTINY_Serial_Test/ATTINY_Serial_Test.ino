// Author: Blake Freer
// Date: January 15, 2022
// Purpose: Send Serial data to test if the ATTINY84 can receive and process it

// send r+g together
byte red[] =  {0b11111111,0b00000001,0b00000001,0b10000010};
byte green[] = {0b00000001,0b11111111,0b00000001,0b10000010};
//delay 10 seconds and send b+w together
byte blue[] = {0b00000000,0b00000000,0b11111111,0b10000001};;
byte white[] = {0b11111110,0b11111110,0b11111111,0b10000001};

void setup() {
    Serial.begin(9600);
}

void loop() {
//    Serial.write(red, 4);
//    Serial.write(green, 4);
    Serial.write(red[0]);
    Serial.write(red[1]);
    Serial.write(red[2]);
    Serial.write(red[3]);
    Serial.write(green[0]);
    Serial.write(green[1]);
    Serial.write(green[2]);
    Serial.write(green[3]);
    delay(10 * 1000);
//    Serial.write(blue, 4);
//    Serial.write(white, 4);
    Serial.write(blue[0]);
    Serial.write(blue[1]);
    Serial.write(blue[2]);
    Serial.write(blue[3]);
    Serial.write(white[0]);
    Serial.write(white[1]);
    Serial.write(white[2]);
    Serial.write(white[3]);
    delay(10 * 1000);
}
