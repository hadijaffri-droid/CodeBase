#include <Arduino.h> 
#include <AccelStepper.h>
#include <time.h>
#include <RTClib.h>
#include "Shield_Base.h"
#include "button.h"
#include "LCD_I2C.h"
#include "CounterStore.h"
#include "LoopTimer.h"

// -- PARAMETER START
const uint8_t DIR_PIN = 7;  // green
const uint8_t STEP_PIN= 8;  // white
const uint8_t ENABLE_PIN = 11;  // grey
const uint8_t END_SWITCH_LEFT_PIN = 4;
const uint8_t END_SWITCH_RIGHT_PIN = 12;
const uint8_t INTERRUPT_BUTTON =2;
const int OSCILLATIONS = 5;
const long TOTAL_CYCLES=2000;
const long MOTOR_MAX_SPEED_STEPS_PER_SECOND = 3000;
const long  MOTOR_SPEED_STEPS_PER_SECOND=2000;
const long  MOTOR_ACCLERATION_STEPS_PER_SECOND_SQUARED =2000;
const uint32_t START_CYCLE_INDEX = 0; // Used after a new upload/build is detected
uint32_t currentCycleCount = START_CYCLE_INDEX;
// --PARAMETER END /////

//--- CONFIGURATION ---
const long STEPS_PER_ROTATION = 10000;
long steps_x_degree_movement= 0;
const int dir_left = -1;
const int dir_right = 1;
char time_char_buffer[30];
int lcd_rows = 4;
int lcd_columns = 20;
uint8_t lcd_address = 0x27;
int angle_ninety_degrees = 90;

//-- CREATE OBJECTS --
AccelStepper stepper(1, STEP_PIN,DIR_PIN); //(1) means a stepper driver (with Step and Direction pins).
CounterStore counterStore;
Button Left_button("LEFT_BUTTON", END_SWITCH_LEFT_PIN,200); 
Button Right_button("RIGHT_BUTTON", END_SWITCH_RIGHT_PIN,200); 
Button Interrupt_button("Interrupt_button",INTERRUPT_BUTTON,200);
LCD_I2C lcd(lcd_address,lcd_columns,lcd_rows);
LoopTimer Timer;

// HELPER FUNCTION PROTOTYPTES
void moveStepsRelative(long position, int direction  = 1 );
void oscillate_Relative(int repititions,long distance_in_angle, int oscillation_start_direction); // changed the function arguments
void moveToSwitch(Button& LimitSwitch, const int direction);
void showCycleCount(uint32_t count);
void saveCycleCount(uint32_t count);
void lcd_row_clear(int row);
bool is_finished();
void show_time_left_on_console();
void motor_enable(bool set_motor);
long degree_to_steps(int);
void show_time_left_on_lcd(); 
void setup()
{
  Serial.begin(115200); 
  delay(500);
  pinMode(ENABLE_PIN, OUTPUT); 
  motor_enable(HIGH);
  stepper.setMaxSpeed(MOTOR_MAX_SPEED_STEPS_PER_SECOND);
  stepper.setAcceleration(MOTOR_ACCLERATION_STEPS_PER_SECOND_SQUARED); 
  stepper.setSpeed(MOTOR_SPEED_STEPS_PER_SECOND);
  lcd.init();
  counterStore.begin();
  Interrupt_button.begin(true); // interrupts is being attached here
  const char firmwareId[] = __DATE__ " " __TIME__;
  if (!counterStore.resetIfFirmwareChanged(firmwareId, START_CYCLE_INDEX)) 
  {
    counterStore.loadLatest(currentCycleCount);
  } else 
  {
    currentCycleCount = START_CYCLE_INDEX;
  }
  Timer.init(currentCycleCount,TOTAL_CYCLES);
  showCycleCount(currentCycleCount);
  Serial.println("Moving to switch A");
}

void loop()
{
  moveToSwitch(Left_button, dir_right);  // move towards A, dir_right moves the stepper CCW
  stepper.setCurrentPosition(0); // reset position
  moveToSwitch(Right_button,dir_left);  // move towards B , a dir_left moves the stepper CW
  long middlePosition = (stepper.currentPosition()/2); // calculate Middle position
  moveStepsRelative(middlePosition,dir_left); // move to middle position after reaching B
  steps_x_degree_movement = degree_to_steps(45);
  moveStepsRelative(steps_x_degree_movement,dir_right); // Move Initial - 45 degrees
  oscillate_Relative(OSCILLATIONS,angle_ninety_degrees,dir_left); // start CCW
  currentCycleCount++; // increment count
  saveCycleCount(currentCycleCount); 
  showCycleCount(currentCycleCount);
  Timer.update(currentCycleCount);
  show_time_left_on_console();
  show_time_left_on_lcd();
  if(is_finished())
  {
    lcd.write_center(2,"TOTAL CYCLES REACHED");
    Serial.println("Total Cycles Reached");
    while(1);
  }
}

void moveStepsRelative(long position, int direction = 1)
{
  stepper.setCurrentPosition(0);
  stepper.moveTo(direction*position);
  while(stepper.distanceToGo() != 0)
  {
    if(Interrupt_button.get_pause_state())
    {
    Serial.println("==PAUSED==");
    lcd.write_center(2,"==PAUSED==");
     while(Interrupt_button.get_pause_state())
     {
     }
    Serial.println("==RESUMED==");
    lcd_row_clear(2);
    stepper.moveTo(position); // recalcualte the position of the stepper when you move out
    } 
    stepper.run(); // does not use acceleration profile. Runs to target at constant speed set by setSpeed(). Is non Blocking
  }
  //stepper.runToPosition();
}

void oscillate_Relative(int repititions,long distance_in_angle, int oscillation_start_direction)
{
lcd_row_clear(1);
long angle_in_steps = degree_to_steps(distance_in_angle);
for (int i = 0; i < repititions;i++)
{
  moveStepsRelative(angle_in_steps,oscillation_start_direction); // move clockwise
  moveStepsRelative(angle_in_steps,-oscillation_start_direction);  // move counter clockwise
  String lcd_string = "Osc = "+String(i+1);
  lcd.write_center(1,lcd_string.c_str());
}
}

void moveToSwitch(Button& LimitSwitch, const int dir)
{
  if(dir == dir_right)
  {
     Serial.println("Set direction counter clockwise");
     stepper.setSpeed(MOTOR_SPEED_STEPS_PER_SECOND); // positive Speed moves counter clockwise
  }
  else if(dir == dir_left)
  {
     Serial.println("Set direction clockwise");
     stepper.setSpeed(-1*MOTOR_SPEED_STEPS_PER_SECOND); // negative Speed moves clockwise
  }
  while(!LimitSwitch.isActive())
  {
    //Serial.print("INTERRUPT BUTTON STATE : ");
    //Serial.println(interrupt_button.get_pause_state());
    if(Interrupt_button.get_pause_state())
    {
      Serial.println("==PAUSED==");
      lcd.write_center(2,"==PAUSED==");
      while (Interrupt_button.get_pause_state())
      {
      }
      Serial.println("==RESUMED==");
      lcd_row_clear(2);
    }
    stepper.runSpeed(); // runSpeed runs infinitely, does not respect the targetPosition
  }

}

void showCycleCount(uint32_t count) 
{
  String lcd_string = "Cycles = " + String(count);
  lcd.write_center(0, lcd_string.c_str());
  Serial.print("Completed cycles: ");
  Serial.println(currentCycleCount);
}

void saveCycleCount(uint32_t count) 
{
  if (!counterStore.save(count)) {
    Serial.println("Warning: cycle counter could not be saved to EEPROM");
  }
}

void lcd_row_clear(int row)
{
    lcd.write_center(row,"                    ");
}

bool is_finished()
{
   if(currentCycleCount >= TOTAL_CYCLES)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void show_time_left_on_console()
{
  uint64_t TIME = Timer.getRemainingS();
  int seconds = (TIME%60); // first extract the minutes, then use the remainder for seconds
  int minutes = (TIME/60)%60; // first find total number of minutes, then use mod to remove hours inside the minutes
  int hours = (TIME/3600);
  Serial.print("TIME LEFT: ");
  Serial.print(hours);
  Serial.print(" hours ");
  Serial.print(minutes);
  Serial.print(" minutes ");
  Serial.print(seconds);
  Serial.println(" seconds");
}

void motor_enable(bool set_motor)
{
  if(set_motor)
  {
    digitalWrite(ENABLE_PIN, HIGH);
  }
  else
  {
    digitalWrite(ENABLE_PIN,LOW);
  }
}

long degree_to_steps(int degrees)
{
  return (STEPS_PER_ROTATION*degrees)/360;
}

void show_time_left_on_lcd()
{
  DateTime end = Timer.getRemainingS();
  char buf[] = "hh:mm:ss";
  end.toString(buf);
  snprintf(time_char_buffer, sizeof(time_char_buffer), "END TIME : %s", buf);
  lcd.write_center(3,time_char_buffer);
}