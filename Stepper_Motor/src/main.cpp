#include <Arduino.h> 
#include <AccelStepper.h>
#include "Shield_Base.h"
#include "button.h"
#include "LCD_I2C.h"
#include "CounterStore.h"
const uint8_t dir_pin = 7;           // green
const uint8_t step_pin = 8;          // white
const uint8_t enable_pin = 11;       // grey
const uint8_t END_SWITCH_LEFT_PIN = 12;
const uint8_t END_SWITCH_RIGHT_PIN = 3;
const uint8_t PAUSE_BUTTON =2;
// --- CONFIGURATION ---
const uint32_t START_CYCLE_INDEX = 0; // Used after a new upload/build is detected
uint32_t currentCycleCount = START_CYCLE_INDEX;
const int OSCILLATIONS = 5;
const long MOTOR_SPEED_STEPS_PER_SECOND = 800;
const long TOTAL_CYCLES=5;
const unsigned long DEBOUNCE_DELAY = 50;
// Global Variables
const int STEPS_PER_ROTATION = 10000;
long targetPosition = 0; 
long totalStepsMoved = 0;
long steps_45_degree_movement = 0;            // Globalized so both loop and helper functions can see it
String lcd_string = "";
volatile unsigned long lastInterrupt= 0;
volatile bool state = true;
AccelStepper Stepper(1, step_pin, dir_pin); // stepper uses an internal driver, use 1 as the first parameter for the stepper
CounterStore CounterStore;

// create a button object,  need 2 pins for each button
Button left_button("Left_button", END_SWITCH_RIGHT_PIN,50); // left button has pin 3 for interrupt, pin 4 for power which is not used since we will use a 5v rail
Button right_button("Right_button", END_SWITCH_LEFT_PIN,50); // right button has pin 2 for interrupt, pin 5 for power which is not used since will use a 5v rail

// create an LCD object
LCD_I2C lcd(0x27,20,4);
// State Machine Definition
enum State { MOVING_TO_SWITCH_A, MOVING_TO_SWITCH_B, RUN_TEST_SEQUENCE,PAUSED,FINISHED};
State currentState = PAUSED;
State nextState = MOVING_TO_SWITCH_A;
// Helper function prototypes
void moveAndRun(long steps);

void moveUntilSwitch(Button& limitSwitch, float speed);
void moveToMiddleThen45(long TargetPosition);
void oscillatePattern(int repetitions,long start,long end);
void showCycleCount();
void saveCycleCount();
void pause();
void setup() 
{
  pinMode(enable_pin, OUTPUT); 
  digitalWrite(enable_pin, HIGH); 
  pinMode(PAUSE_BUTTON,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAUSE_BUTTON),pause,FALLING);
  Stepper.setCurrentPosition(0);
  Stepper.setMaxSpeed(1000);
  Stepper.setAcceleration(200); 
  Stepper.setSpeed(MOTOR_SPEED_STEPS_PER_SECOND);  // Start moving toward switch A
  Serial.begin(9600); 
  delay(500);
  lcd.init();
  CounterStore.begin();

  const char firmwareId[] = __DATE__ " " __TIME__;
   if (!CounterStore.resetIfFirmwareChanged(firmwareId, START_CYCLE_INDEX)) {
    CounterStore.loadLatest(currentCycleCount);
    
    nextState = MOVING_TO_SWITCH_A; 
    currentState = PAUSED;
    state = true; 
    lcd.write_center(2, "PAUSED - Press Start");
  } else {
    currentCycleCount = START_CYCLE_INDEX;
    currentState = PAUSED; // Fresh build also starts paused
    state = true;
  }


  showCycleCount();
  Serial.println("Moving to switch A");
}

void pause()
{
  unsigned long interruptTime = millis();
  if(interruptTime-lastInterrupt > DEBOUNCE_DELAY)
  {
    state=!state;
  }
  //Serial.println("State = ");
  //Serial.print(state);
  lastInterrupt=interruptTime;
}
void loop() 
{ 
  switch(currentState) 
  {
    case MOVING_TO_SWITCH_A:
      Stepper.runSpeed(); 
      
      if (left_button.buttonWasPressed()) 
      {
       // need to reset the interrupt flag
        Serial.println("Switch A reached. Resetting zero point");
        
        Stepper.setCurrentPosition(0); 
        Stepper.setSpeed(-MOTOR_SPEED_STEPS_PER_SECOND); // Reverse direction towards switch B
        
        currentState = MOVING_TO_SWITCH_B; 
      }
      break;

    case MOVING_TO_SWITCH_B:
      Stepper.runSpeed(); 
      
      if (right_button.buttonWasPressed()) 
      {
        
        totalStepsMoved = -1 * Stepper.currentPosition(); // total steps moved is a positive value, since current position gave a negative value
        
        Serial.println("Right limit (Switch 1) Reached!");
        Serial.print("Total Steps Moved between limits: ");
        Serial.println(totalStepsMoved);
        
        Stepper.setCurrentPosition(0);        // Set absolute coordinate origin at Switch 1
        targetPosition = totalStepsMoved / 2; // Midpoint distance calculation
        
        // Compute our 45-degree scale dynamically based on the current scan
        //float stepsPerDegree = (float)(targetPosition * 2) / 300.0;

        float stepsPerDegree = (float)STEPS_PER_ROTATION / 360.0;
        steps_45_degree_movement = long(45.0 * stepsPerDegree); // positive moves from switch B toward switch A
        if (steps_45_degree_movement <= 0){
          steps_45_degree_movement = 1;
        }
        Serial.print("steps_45_degree_movement =");
        Serial.println(steps_45_degree_movement);
        
        // Move directly to the core test logic execution state
        currentState = RUN_TEST_SEQUENCE; 
      }
      break;

    case RUN_TEST_SEQUENCE:
      Serial.print("\n===============================");
      Serial.print(" STARTING TESTING CYCLE AFTER #");
      Serial.print(currentCycleCount);
      Serial.println(" ===============================");

      // From switch B: move to center, then to -45 degrees around that center.
      moveToMiddleThen45(targetPosition);

      Serial.println(" -> Executing Oscillation Routine #1 (20 iterations)...");
      lcd.write_center(1,"                    ");
      oscillatePattern(OSCILLATIONS,-(2*steps_45_degree_movement),(2*steps_45_degree_movement));
      Serial.println(" -> Moving to switch A...");
      moveUntilSwitch(left_button, MOTOR_SPEED_STEPS_PER_SECOND);
      //stepper.setCurrentPosition(totalStepsMoved);

      // From switch A: move back to center, then to -45 degrees around that center.
      moveToMiddleThen45(-targetPosition);
       lcd.write_center(1,"                    ");
      Serial.println(" -> Executing Oscillation Routine #2 (20 iterations)...");
      oscillatePattern(OSCILLATIONS,-(2*steps_45_degree_movement),(2*steps_45_degree_movement));
      Serial.println(" -> Moving to switch B...");
      moveUntilSwitch(right_button, -MOTOR_SPEED_STEPS_PER_SECOND);
      //stepper.setCurrentPosition(0);

      currentCycleCount++;
      saveCycleCount();
      showCycleCount();
      Serial.println("Cycle complete. Starting next cycle from switch B.");
      if(currentCycleCount==TOTAL_CYCLES)
      {
        currentState = FINISHED ;
        lcd.write_center(0, "Cycles Completed");
        while(1);
      }
      else if(state)
      {
        lcd.write_center(2,"                    ");
        lcd.write_center(2,"PAUSED");
        Serial.println("Paused");
        nextState=MOVING_TO_SWITCH_A;
        Stepper.setSpeed(MOTOR_SPEED_STEPS_PER_SECOND);
        currentState=PAUSED;

      }
      else
      {
        currentState = MOVING_TO_SWITCH_A;
        Stepper.setSpeed(MOTOR_SPEED_STEPS_PER_SECOND);
      }
      break;
      case PAUSED:

      if(!state)
      {
        lcd.write_center(2,"                    ");
        lcd.write_center(2,"RESUMED");
        Serial.println("Resumed");
        currentState=nextState;
      }
      break;
      case FINISHED:
      Serial.print("Total Number of Cycles Reached");
      Serial.println(currentCycleCount-1);
      break;
  }
}

/*
 * Helper function to command a relative movement and block until execution completes.
 */
void moveAndRun(long steps) {
  Stepper.setCurrentPosition(0);
  Stepper.moveTo(steps);
  /*Serial.print("Steps = ");
  Serial.println(steps);
  Serial.print("Distance to go = ");
  Serial.println(stepper.distanceToGo());*/
  while (Stepper.distanceToGo() != 0) {
    Stepper.runSpeed(); // non blocking , must be called repeatedly
  }
}

void moveUntilSwitch(Button& limitSwitch, float speed) {
  Stepper.setSpeed(speed);
  while (!limitSwitch.isActive()) {
    Stepper.runSpeed();
  }
}

void moveToMiddleThen45(long TargetPosition) {
  Serial.println(" -> Moving to physical midpoint...");
  moveAndRun(TargetPosition);
  Serial.println(" -> Moving to -45 degrees from midpoint...");
  moveAndRun(steps_45_degree_movement);
}

/**
 * Starts at -45 degrees from center, then sweeps +90 and -90 degrees.
 */
void oscillatePattern(int repetitions, long start_pos, long end_pos) {
  for(int i = 0; i < repetitions; i++) {
    moveAndRun(start_pos);
    moveAndRun(end_pos);
     //moveAndRun(2 * steps_to_rotate);
     //moveAndRun(-2 * steps_to_rotate);
     lcd_string = "Osc = "+String(i+1);
     lcd.write_center(1,lcd_string.c_str());
  }
}

void showCycleCount() {
  lcd_string = "Cycles = " + String(currentCycleCount+1);
  lcd.write_center(0, lcd_string.c_str());
  Serial.print("Completed cycles: ");
  Serial.println(currentCycleCount);
}

void saveCycleCount() {
  if (!CounterStore.save(currentCycleCount)) {
    Serial.println("Warning: cycle counter could not be saved to EEPROM");
  }
}
