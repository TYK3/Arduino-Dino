#include <Servo.h>
#include <Stepper.h>
#include <HCSR04.h>
#include <pitches.h>

Servo myservo;
Servo myservo2;
Servo myservo3;
Servo myservo4;

const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 8,10,9,11);

int LDRpin = A0; // LDR connected to analog pin A0
int LDR2pin = A1;// LDR2 connected to analog pin A1
int buzzerpin = A2;// buzzer connected to analog pin A2
int trigger2Pin = A3; // Radar Ultrasonic sensor trigger pin
int echo2Pin = A4; // Radar Ultrasonic sensor echo pin
int pos = 0;
int a1;
int distance;
long duration;
int cycleCount = 0;


const int BLEDPin = 4; // Digital output for yellow LEDs
const int GLEDPin = 6; // Digital output for green LEDs
const int RLEDPin = 5; // Digital output for red LEDs
const int triggerPin = 12; // Ultrasonic sensor trigger pin
const int echoPin = 13; // Ultrasonic sensor echo pin
const int REST = 50;

UltraSonicDistanceSensor sensor1 (triggerPin,echoPin);
UltraSonicDistanceSensor sensor2 (trigger2Pin,echo2Pin);

void setup() {
  myservo.attach(7);
  myservo2.attach(3);
  myservo3.attach(2);
  myservo4.attach(A5);
  Serial.begin(9600);
  pinMode(BLEDPin, OUTPUT);
  pinMode(RLEDPin, OUTPUT);
  pinMode(GLEDPin, OUTPUT);
  pinMode(LDRpin, INPUT);
  pinMode(buzzerpin, OUTPUT);
  songs();
  
}

void songs(){
  int melody[] = {
  NOTE_AS4, NOTE_AS4, NOTE_AS4,
  NOTE_F5, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_F5, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,

  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  
  NOTE_C6, NOTE_G5, NOTE_G5, REST, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C6, NOTE_C6,
  NOTE_F6, NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5,
  NOTE_C6

};

  int durations[] = {
  8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,

  8, 8, 8, 2, 4,
  8, 8, 8, 2, 8, 16,
  4, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 4, 8, 16,
  4, 8, 8, 8, 8, 8,

  8, 16, 2, 8, 8,
  4, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 4, 8, 16,
  4, 8, 4, 8, 4, 8, 4, 8,
  1

 };

  int size = sizeof(durations) / sizeof(int); 
	 for (int note = 0; note < size; note++) { 
	   //to calculate the note duration, take one second divided by the note type. 
	   //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc. 
	   int duration = 1000 / durations[note]; 

     tone(buzzerpin, melody[note], duration); 

	   //to distinguish the notes, set a minimum time between them. 
	   //the note's duration + 30% seems to work well: 
	   int pauseBetweenNotes = duration * 1.30; 
	   delay(pauseBetweenNotes); 
	   //stop the tone playing: 
	   noTone(buzzerpin); 
	 } 
} 

void songtrigger(){
  int LDR2value = analogRead(LDR2pin);
  if (LDR2value > 550 && LDR2value < 750){
    songs();
}
}

void radar() {
  int LDRvalue = analogRead(LDRpin);
  
  if (LDRvalue > 500 && LDRvalue < 700 ) { // You might need to adjust this threshold based on your LDR readings
      digitalWrite(RLEDPin, HIGH);

    // rotates the servo motor from 15 to 165 degrees
      for(int i=15;i<=165;i++){  
      myservo4.write(i);
      delay(30);
  

      if (distance <= 50) {
      tone(buzzerpin, 500);
      delay(50);
      noTone(buzzerpin);
      delay(50);
    }

    distance = calculateDistance();// Calls a function for calculating the distance measured by the Ultrasonic sensor for each degree
    Serial.print(i); // Sends the current degree into the Serial Port
    Serial.print(","); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
    Serial.print(distance); // Sends the distance value into the Serial Port
    Serial.print("."); // Sends addition character right next to the previous value needed later in the Processing IDE for indexing
 
    }
  // Repeats the previous lines from 165 to 15 degrees
      for(int i=165;i>15;i--){  
      myservo4.write(i);
      delay(30);

     if (distance <= 50) {
      tone(buzzerpin, 500);
      delay(50);
      noTone(buzzerpin);
      delay(50);
    }

    distance = calculateDistance();
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
    }
 }else{
  digitalWrite(RLEDPin, LOW);
  }
}


// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance(){ 
  digitalWrite(trigger2Pin, LOW); 
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigger2Pin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigger2Pin, LOW);
  duration = pulseIn(echo2Pin, HIGH); // Reads the echoPin, returns the sound wave travel time in microseconds
  distance= duration*0.034/2;
  return distance;
  
  }

void dinosaurRoar() {
 const int numNotes = 8;
 const int noteDurations[] = {200, 150, 200, 150, 200, 150, 200, 150};
 const int noteFrequencies[] = {1047, 996, 1047, 996, 1047, 996, 1047, 996};

 for (int thisNote = 0; thisNote < numNotes; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerpin, noteFrequencies[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
 }
}

void bite() {
  int LDRvalue = analogRead(LDRpin);
  Serial.print("LDR:");
  Serial.println(LDRvalue);
  delay(15);

  if (LDRvalue < 210) { 
  digitalWrite(BLEDPin, HIGH);
  digitalWrite(GLEDPin, LOW);

  for (pos = 0; pos <= 180; pos += 3) {
    myservo.write(pos);
    delay(15);
    }

  for (pos = 180; pos >= 0; pos -= 3) {
    myservo.write(pos);
    delay(15);
    } 
  } else {
      digitalWrite(BLEDPin, LOW);
      digitalWrite(GLEDPin, HIGH);
    }
  
}

void wings_tails() {
  int LDR2value = analogRead(LDR2pin);
  Serial.print("LDR2:");
  Serial.println(LDR2value);
  delay(15);

  if (LDR2value < 100) { 
  digitalWrite(GLEDPin, LOW);
  digitalWrite(BLEDPin, HIGH);
  dinosaurRoar();

  for (pos = 0; pos <= 180; pos += 3) {
    myservo2.write(pos);
    myservo3.write(pos);
    delay(15);
    }

  for (pos = 180; pos >= 0; pos -= 3) {
    myservo2.write(pos);
    myservo3.write(pos);
    delay(15);
    } 
  } else {
      digitalWrite(BLEDPin, LOW);
      digitalWrite(GLEDPin, HIGH);
    }
  
}



void move() {
      // Turn off red LEDs
      digitalWrite(RLEDPin, LOW);

      // Move forward (clockwise rotation continuously)
      myStepper.setSpeed(15);

      // Turn on the LED on (HIGH is the voltage level)
      digitalWrite(GLEDPin, HIGH);

      // Spin the stepper motor one revolution
      myStepper.step(100);

      // Turn off the green LED when the motor stops
      digitalWrite(GLEDPin, LOW);

      // Delay for stability
      delay(10);
}

void move2() {
  // Turn off green LEDs
  digitalWrite(GLEDPin, LOW);

  // Move backward (anticlockwise rotation continuously)
   myStepper.setSpeed(15);

  // Turn on the red LED and keep it on until the motor stops
  digitalWrite(RLEDPin, HIGH);

  // Spin the stepper motor one revolution
   myStepper.step(-100);

  // Turn off the red LED when the motor stops
  digitalWrite(RLEDPin, LOW);

  // Delay for stability
  delay(10);
}

void move3(){
  // Stay in place
  myStepper.step(0);
  // Control the LEDs
  digitalWrite(GLEDPin, HIGH); // Turn on green LED
  digitalWrite(RLEDPin, LOW);   // Turn off red LEDs
}

void mover(){
if (a1>15 && a1<90){
  move();
}
else if (a1>1 && a1<15){
  move2();
}
else{
  move3();
}
}

void loop() {
a1 = sensor1.measureDistanceCm(); 
Serial.print("USdistance:");
Serial.println(a1);

bite();
wings_tails();
radar();
songtrigger();
mover();
}