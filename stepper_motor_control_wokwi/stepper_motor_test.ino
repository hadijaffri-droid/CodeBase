#include <Arduino.h> 
#include <AccelStepper.h>
#include "Shield_Base.h"
#include "button.h"
#include "LCD_I2C.h"
const uint8_t dir_pin = 7;           // green
const uint8_t step_pin = 8;          // white
const uint8_t enable_pin = 11;       // grey

// --- CONFIGURATION ---
const int TOTAL_CYCLES_TO_RUN = 1;   // Define N: How many complete cycles to repeat
int currentCycleCount = 0;           // Tracks completed test cycles

// Global Variables
long targetPosition = 0; 
long totalStepsMoved = 0;
long steps_to_rotate = 0;            // Globalized so both loop and helper functions can see it
String lcd_string = "";
AccelStepper stepper(1, step_pin, dir_pin); // stepper uses an internal driver, use 1 as the first parameter for the stepper

// create a button object,  need 2 pins for each button
Button left_button("Left_button",3,400); // left button has pin 3 for interrupt, pin 4 for power which is not used since we will use a 5v rail
Button right_button("Right_button",2,400); // right button has pin 2 for interrupt, pin 5 for power which is not used since will use a 5v rail

// create an LCD object
LCD_I2C lcd(0x27,20,4);
// State Machine Definition
enum State { MOVING_CCW, MOVING_CW, RUN_TEST_SEQUENCE, FINISHED };
State currentState = MOVING_CCW;

// Helper function prototypes
void moveAndRun(long steps);
void oscillatePattern(int repetitions);

void setup() 
{
  pinMode(enable_pin, OUTPUT); 
  digitalWrite(enable_pin, LOW); 
  
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(3000);
  stepper.setAcceleration(2000); 
  stepper.setSpeed(2000);         // Start moving CCW
  Serial.begin(9600); 
  delay(500);
  Serial.println("Moving CCW to LS");
}


void loop() 
{ 
  switch(currentState) 
  {
    case MOVING_CCW:
      stepper.runSpeed(); 
      
      if (left_button.buttonWasPressed()) 
      {
       // need to reset the interrupt flag
        Serial.println("Resetting zero point");
        
        stepper.setCurrentPosition(0); 
        stepper.setSpeed(-2000); // Reverse direction towards Switch 1, move clockwise
        
        currentState = MOVING_CW; 
      }
      break;

    case MOVING_CW:
      stepper.runSpeed(); 
      
      if (right_button.buttonWasPressed()) 
      {
        
        totalStepsMoved = -1 * stepper.currentPosition(); // total steps moved is a positive value, since current position gave a negative value
        
        Serial.println("Right limit (Switch 1) Reached!");
        Serial.print("Total Steps Moved between limits: ");
        Serial.println(totalStepsMoved);
        
        stepper.setCurrentPosition(0);        // Set absolute coordinate origin at Switch 1
        targetPosition = totalStepsMoved / 2; // Midpoint distance calculation
        
        // Compute our 45-degree scale dynamically based on the current scan
        float stepsPerDegree = (float)(targetPosition * 2) / 300.0;
        steps_to_rotate = 45.0 * stepsPerDegree; // is a positive value, using steps_to_rotate will move CCW
        if (steps_to_rotate <= 0) steps_to_rotate = 1;
        
        // Move directly to the core test logic execution state
        currentState = RUN_TEST_SEQUENCE; 
      }
      break;

    case RUN_TEST_SEQUENCE:
      currentCycleCount++;
      lcd_string = "Cycle = "+String(currentCycleCount);
      lcd.write_center(0,lcd_string.c_str());
      Serial.print("\n===============================");
      Serial.print(" STARTING TESTING CYCLE #");
      Serial.print(currentCycleCount);
      Serial.println(" ===============================");

      // Step 1: Move from Switch 1 to the middle position
      Serial.println(" -> Moving to physical midpoint...");
      stepper.moveTo(targetPosition);   // target position is a positve value, moveTo rotates in CCW direction 
      stepper.runToPosition();  // this is blocking

      // Step 2: Run first oscillation loop (20 times) and stay in center when done
      Serial.println(" -> Executing Oscillation Routine #1 (20 iterations)...");
      oscillatePattern(20);

      // Step 3: Move from middle all the way back to Switch 1
      Serial.println(" -> Returning to Switch 1...");
      moveAndRun(-1 * targetPosition);

      // Step 4: Move from Switch 1 back to the middle
      Serial.println(" -> Returning to physical midpoint...");
      moveAndRun(targetPosition);

      // Step 5: Run second oscillation loop (20 times) and stay in center when done
      Serial.println(" -> Executing Oscillation Routine #2 (20 iterations)...");
      oscillatePattern(20);

      // Step 6: Move from the middle all the way to Switch 2 (Left switch)
      Serial.println(" -> Returning to Switch 2 (Left)...");
      moveAndRun(targetPosition); // Moving an additional half-span past center lands on Switch 2

      // Check loop boundary conditions to see if we repeat or terminate
      if (currentCycleCount >= TOTAL_CYCLES_TO_RUN) {
        currentState = FINISHED;
      } else {
        // Cycle is complete! Start over by letting it touch switch 2 again
        Serial.println("Cycle Finished! Re-engaging home parameters...");
        currentState = MOVING_CCW; 
      }
      break;

    case FINISHED:
      Serial.println("\nAll configured testing blocks completed successfully!");
      Serial.flush(); 
      while(1) {  
        delay(1000); 
      }
      break;
  }
}

/**
 * Helper function to command a relative movement and block until execution completes.
 */
void moveAndRun(long steps) {
  stepper.move(steps);
  while (stepper.distanceToGo() != 0) {
    stepper.run(); // non blocking , must be called repeatedly
  }
}

/**
 * Helper function that moves out by 45 degrees, runs a series of back-and-forth 
 * sweeps, and drops smoothly back into the origin midpoint when finished.
 */
void oscillatePattern(int repetitions) {
  // 1. Initial consistency move to start at the -45 position
  moveAndRun(steps_to_rotate);
  
  // 2. Continuous sweep profiles
  for(int i = 0; i < repetitions; i++) {
     moveAndRun(-2 * steps_to_rotate); // Sweep CW to 45 position
     moveAndRun(2 * steps_to_rotate);  // Sweep CCW back to -45 position
     lcd_string = "Osc"+String(i+1);
     lcd.write_center(1,lcd_string.c_str());
  }
  // 3. Return move to step down precisely on the 0 center alignment map
  moveAndRun(-1 * steps_to_rotate); 
}
