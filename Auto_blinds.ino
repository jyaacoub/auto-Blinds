//Include the Arduino Stepper Library
#include <Stepper.h>
#define LIGHT_SENSOR A0
 
// Define Constants
const int lightSunny = 400;
const int lightDawn = 215;
const int lightMorning = 215;
const int lightNight = 30; // to stop actual lights from keeping it open
// Both lamp and main lights are on => 36
// only lamp facing it => ~30
// only main light => ~20
// None or lamp facing away => ~8
 
// Number of steps per internal motor revolution
const float STEPS_PER_REV = 32;
 
//  Amount of Gear Reduction
const float GEAR_RED = 64;
 
// Number of steps per geared output rotation
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
 
// Define Variables
 
// Number of Steps Required
int StepsRequired;
 
// Create Instance of Stepper Class
// Specify Pins used for motor coils
// The pins used are 8,9,10,11
// Connected to ULN2003 Motor Driver In1, In2, In3, In4
// Pins entered in sequence 1-3-2-4 for proper step sequencing
 
Stepper steppermotor(STEPS_PER_REV, 8, 10, 9, 11);

int blindsPos = 0;
int checks = 0;
int iterationsPassed = 0;
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
  iterationsPassed += 1;
}

void userMode(){
  if (iterationsPassed > 10 && checks != 0){
    checks = 0;
    iterationsPassed = 0;
  }
  
  int light = getAverageLightIntensity(10); // Calculated over 5 mins (300s)
  Serial.println(light);
  if (light < 0){
    Serial.println("ERROR");
  }else if ((light > lightSunny) || (light < lightNight)){
    if (checks >= 3){
      // Closing blinds:
      tiltBlinds(CLOSED);
      checks = 0;
    } else {
      checks += 1;
    }
   
  }else{ // Mornings and dawns
    if (checks <= -3){
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
