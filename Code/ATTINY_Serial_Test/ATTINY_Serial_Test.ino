// Author: Blake Freer
// Date: January 15, 2022
// Purpose: Send Serial data to test if the ATTINY84 can receive it

void setup() {
    Serial.begin(9600);
}

void loop() {
    Serial.write(1);
    delay(1000);
    Serial.write(0);
    delay(500);
}
