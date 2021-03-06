//Include the Arduino Stepper Library
#include <Stepper.h>
#define LIGHT_SENSOR A0

// Constants for the motor:
// Number of steps per internal motor revolution
const float STEPS_PER_REV = 32;
//  Amount of Gear Reduction
const float GEAR_RED = 64;
// Number of steps per geared output rotation
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
// Number of Steps Required
int StepsRequired;
Stepper steppermotor(STEPS_PER_REV, 8, 10, 9, 11);

// Positioning constants
int blindsPos = 0;
int checks = 0;
const int CLOSED = 12; // The netSteps value for the blinds to be closed.

// Threshold constants:
const int lightSunny = 400;
const int lightDawn = 215;
const int lightMorning = 215;
const int lightNight = 30; // Not 0, to stop room lights from keeping it open

// IR stuff
#include <IRremote.h>  //including infrared remote header file     
int RECV_PIN = 3; // the pin where you connect the output pin of IR sensor     
IRrecv irrecv(RECV_PIN);     
decode_results results;
unsigned long key_value = 0;

boolean automatic = true;
int newPos = 0;

void setup(){
  Serial.begin(9600);

  // turning on onboard light when auto mode is on.
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH); 
  
  irrecv.enableIRIn();
  attachInterrupt(digitalPinToInterrupt(RECV_PIN), readIRSignal, RISING);
}
 
void loop(){
  if (automatic){
    Serial.println("AUTO");
    autoMode();
    newPos = blindsPos;
  } else{
    manualMode();
//    Serial.println("MANUAL");
    delay(500);
  }
}

void readIRSignal(){
//  delay(1);
  if (irrecv.decode(&results)){
//    Serial.println("INTERRUPT");
    Serial.println(results.value, HEX);
    
    if (results.value == 0XFFFFFFFF)
      results.value = key_value;

    switch(results.value){
      case 0xFFA25D:
        Serial.println("ON");
        automatic = true;
        digitalWrite(13, HIGH); 
        break;
      case 0xFF629D:
        Serial.println("TIMER");
        break;
      case 0xFFE21D:
        Serial.println("OFF");
        automatic = false;
        digitalWrite(13, LOW); 
        break;
      case 0xFF22DD:
        Serial.println("1");
        blindsPos = 0;
        newPos = 0;
        break;
      case 0xFFC23D:
        Serial.println("2");
        break ;               
      case 0xFFE01F:
        Serial.println("3");
        break ;  
      case 0xFF906F:
        Serial.println("4");
        break ;  
      case 0xFF6897:
        Serial.println("5");
        break ;
      case 0xFFB04F:
        Serial.println("6");
        break ;
      case 0xFF30CF:
        Serial.println("7");
        newPos = 0; 
        break ;
      case 0xFF7A85:
        Serial.println("8");
        newPos = CLOSED; //  Opens
        break ;
      case 0xFF10EF:
        Serial.println("-Bright");
        newPos = blindsPos - 1;
        break ;
      case 0xFF5AA5:
        Serial.println("+Bright");
        newPos = blindsPos + 1;
        break ;      
    }
    key_value = results.value;
    irrecv.resume(); 
  }
}

void manualMode(){
//  Serial.println(newPos);
  tiltBlinds(newPos);
}

// Leaves the blinds to be controlled by the light intensity
void autoMode(){
  int light = getAverageLightIntensity(10); // Calculated every 10 seconds
  Serial.println(light);
  if ((light > lightSunny) || (light < lightNight)){
    if (checks >= 12){
      // Closing blinds:
      tiltBlinds(CLOSED); 
      // We dont reset check so that it is more resistant against clouds
    } else {
      checks += 3;
    }
   
  }else{ // Mornings and dawns
    if (checks <= -4){
      // Opening blinds:
      tiltBlinds(0);
      checks = 0;
    } else {
      checks -= 1;
    }
  }
}

// timePeriod units = seconds
int getAverageLightIntensity(int timePeriod){
  int total = 0;
  for (int i = 0; i < timePeriod; i++){
    int light = analogRead(LIGHT_SENSOR);
    if (light < 0){
      Serial.print("ERROR: ");
      Serial.println(light);
    }
    total += light;
    delay(1000);
  }

  return (total/timePeriod);
}

// pos 0 == blinds are horizontal
void tiltBlinds(int pos){
  // The dif is also equal to the num steps to take.
  int dif = pos - blindsPos;
 
  if ((dif != 0) && (pos >= -CLOSED) && (pos<= CLOSED)){ // So it doesnt break the tilt mech.
    if (dif < 0){
      // Opening Blinds (going down)
      Serial.println("\t\t Down");
      turnMotor(1, abs(dif));
    } else {
      // Closing Blinds (going up)
      Serial.println("\t\t Up");
      turnMotor(0, abs(dif));
    }
  }
}

// directionIn: 
//    0 = counter-clockwise 
//    1 = clockwise
void turnMotor(int directionIn, float numSteps){
  if (directionIn == 0){  // Turns it counter-clockwise to tilt blinds UP
    StepsRequired  = -(STEPS_PER_OUT_REV/ 2)* numSteps;
    steppermotor.setSpeed(700);  
    steppermotor.step(StepsRequired);

    blindsPos += numSteps;
   
  } else{ // Turns it clockwise to tilt blinds DOWN
    StepsRequired  = (STEPS_PER_OUT_REV/ 2)* numSteps;
    steppermotor.setSpeed(700);  
    steppermotor.step(StepsRequired);

    blindsPos -= numSteps;
  }
}

// Extra Functions for debugging

void stepDebugger(){
  Serial.println("\t NET STEPS: " + String(blindsPos));
 
  Serial.println("Up or Down? (0/1)");
  int directionIn = readFromSerial().toInt();
  Serial.println("Num of half revolutions: ");
  float numSteps = (float) readFromSerial().toInt();
 
  turnMotor(directionIn, numSteps);
}


// Reads strings from the serial
String readFromSerial(){
  String serialInput = "";
  int incomingByte = 0;

  while (true){
    // send data only when you receive data:
    if (Serial.available() > 0) {
      // read the incoming byte:
      incomingByte = Serial.read();
 
      // concatonating or exiting with the input
      if (incomingByte == '\n'){
        return serialInput;
      } else {
        serialInput.concat(char(incomingByte));
      }
    }
  }
}
