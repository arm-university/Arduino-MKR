#include <Servo.h>

int ErrorLEDs[] = {A1,A2,A3,A4}; //Four red LEDs
int OKLED = A5; //Green LED
int Buttons[] = {2,3,4,5}; //Four buttons
int ServoSignal = 6; //Servo
const int LockedPos = 90; //Sticking up
const int UnlockedPos = 0; //Laying flat
Servo Lock; //the lock
bool Locked = true; //status of lock
int CorrectCode[] = {0,1,2,3}; //Initial correct pin
int EnteredCode[] = {-1,-1,-1,-1}; //'empty' entered code
int CurrentDigit = 0; //how far through the code we are

void setup()
{
  Serial.begin(9600);
  for (int p = 0;p <= 3;p++) { //setup red LEDs and buttons
    pinMode(Buttons[p],INPUT_PULLUP);
    pinMode(ErrorLEDs[p],OUTPUT);
  }
  pinMode(OKLED,OUTPUT); //setup green LED
  Lock.attach(ServoSignal); //setup lock
  Lock.write(LockedPos); //Ensure lock is locked
}

void checkCode() { 
  bool codeCorrect = true; //Assume the code is correct
  for (int p = 0;p <= 3;p++) {
	if (EnteredCode[p] != CorrectCode[p]) { //if one is wrong
      codeCorrect = false; //then all is wrong
      digitalWrite(ErrorLEDs[p],HIGH); //turn on erro light
    }
  }
  resetEntered(); //Set the entered code back to -1s
  if (codeCorrect) { 
    Serial.println("Correct code!");
    digitalWrite(OKLED,HIGH); //Green LED on
    unlock(); //open lock
    delay(500);
    digitalWrite(OKLED,LOW); //Turn green LED back off
  } else {
    Serial.println("Incorrect code!");
    delay(1000);
    clearErrorLEDs(); //Turn red LEDs back off
  }
}

void resetEntered() {
  for (int p = 0;p <= 3;p++) {
    EnteredCode[p] = -1; //Set code back to -1s
  }
}
 
void lock() { //Lock the box
  Locked = true;
  Lock.write(LockedPos);
}

void unlock() { //Unlock the box
  Locked = false;
  Lock.write(UnlockedPos);
}

void clearErrorLEDs() { //Turn off all red LEDs
  for (int p = 0;p <= 3;p++) {
    digitalWrite(ErrorLEDs[p],LOW);
  }
}

void showCurrentDigits() { //Display what is entered
  Serial.print("Currently entered: "); //for debugging
  for (int p = 0;p <= 3;p++) {
    Serial.print(EnteredCode[p]);
  }
  Serial.println("");
}

void buttonPressed(int buttonID) {
  Serial.print("Button ");
  Serial.print(buttonID);
  Serial.println(" pressed.");
  digitalWrite(ErrorLEDs[buttonID],HIGH);//Flash the red
  EnteredCode[CurrentDigit] = buttonID;  //LED
  CurrentDigit++; //increment the position
  showCurrentDigits(); //Show the entered digits
  delay(500);
  clearErrorLEDs(); //Turn the LEDs off
  if (CurrentDigit == 4) { //4 digits entered
    if (Locked) {
		checkCode(); //Check the code if the box is locked
    } else { //if it's not locked...
	  for (int p = 0;p <= 3;p++) {
    	CorrectCode[p] = EnteredCode[p]; //Use the entered code
  	  }                                  //as the new pin
      resetEntered(); //Reset the entered code
      lock(); //lock the box
    }
    CurrentDigit = 0; //Go back to the start of the arrays
  }
}

void loop()
{
  for (int p = 0;p <= 3;p++) {//Check if each button is pressed
    if (digitalRead(Buttons[p]) == LOW) {
      buttonPressed(p); 
  	}
  }
}