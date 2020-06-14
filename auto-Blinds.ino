//Include the Arduino Stepper Library
#include <Stepper.h>
#define LIGHT_SENSOR A0
 
// Define Constants
const int lightSunny = 400;
const int lightDawn = 215;
const int lightMorning = 215;
const int lightNight = 30; // Not 0, to stop room lights from keeping it open
 
// Number of steps per internal motor revolution
const float STEPS_PER_REV = 32;
 
//  Amount of Gear Reduction
const float GEAR_RED = 64;
 
// Number of steps per geared output rotation
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
 
// Define Variables
 
// Number of Steps Required
int StepsRequired;
 
Stepper steppermotor(STEPS_PER_REV, 8, 10, 9, 11);

int blindsPos = 0;
int checks = 0;
const int CLOSED = 12; // The netSteps value for the blinds to be closed.
 
void setup(){
  Serial.begin(9600);
  // turning off onboard light
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  
//tiltBlinds(-CLOSED);
}
 
void loop(){
  Serial.println(analogRead(LIGHT_SENSOR));
  delay(100);
  userMode();
}

void userMode(){
  int light = getAverageLightIntensity(10); // Calculated every 10 seconds
  Serial.println(light);
  if (light < 0){
    Serial.println("ERROR");
  }else if ((light > lightSunny) || (light < lightNight)){
    if (checks >= 12){
      // Closing blinds:
      tiltBlinds(CLOSED); // check isnt reset to zero so it doesnt go up and down due to clouds
      
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

int getAverageLightIntensity(int timePeriod){
  // time period is num of seconds to calculate average over
  int total = 0;
  for (int i = 0; i < timePeriod; i++){
    int light = analogRead(LIGHT_SENSOR);
    if (light < 0){
      Serial.println(light);
    }
    total += light;
    delay(1000);
  }

  return (total/timePeriod);
}

void tiltBlinds(int pos){
  // The dif is also equal to the num steps to take.
  int dif = pos - blindsPos;
 
  if (dif != 0){
    if (dif < 0){
      // Opening Blinds (going down)
      turnMotor(1, abs(dif));
    } else {
      // Closing Blinds (going up)
      turnMotor(0, abs(dif));
    }
  }
}

void turnMotor(int directionIn, float numSteps){
  if (directionIn == 0){
//    Serial.println("Turing counter clockwise..."); // UP
    StepsRequired  = -(STEPS_PER_OUT_REV/ 2)* numSteps;
    steppermotor.setSpeed(700);  
    steppermotor.step(StepsRequired);

    blindsPos += numSteps;
   
  } else{
//    Serial.println("Turing clockwise..."); //DOWN
    StepsRequired  = (STEPS_PER_OUT_REV/ 2)* numSteps;
    steppermotor.setSpeed(700);  
    steppermotor.step(StepsRequired);

    blindsPos -= numSteps;
  }
}

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
