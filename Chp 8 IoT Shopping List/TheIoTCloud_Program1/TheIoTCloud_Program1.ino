/*
  Test program to ensure external LED is working.
*/

int LED1 = 8;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize specified pin as an output.
  pinMode(LED1, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED1, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED1, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
