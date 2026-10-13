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
const uint8_t DIR_PIN = 7;           // green
const uint8_t STEP_PIN= 8;          // white
const uint8_t ENABLE_PIN = 11;       // grey
// --PARAMETER END /////

// --- DEFINE PINS ---
const uint8_t END_SWITCH_LEFT_PIN = 4;
const uint8_t END_SWITCH_RIGHT_PIN = 12;
const uint8_t INTERRUPT_BUTTON =2;
const int DIR_LEFT = -1;
const int DIR_RIGHT = 1;

//--- CONFIGURATION ---

const uint32_t START_CYCLE_INDEX = 0; // Used after a new upload/build is detected
uint32_t currentCycleCount = START_CYCLE_INDEX;
const int OSCILLATIONS = 5;
const long TOTAL_CYCLES=2000;
const long MOTOR_MAX_SPEED_STEPS_PER_SECOND = 3000;
const long  MOTOR_SPEED_STEPS_PER_SECOND=2000;
const long  MOTOR_ACCLERATION_STEPS_PER_SECOND_SQUARED =2000;
const long STEPS_PER_ROTATION = 10000;
long STEPS_X_DEGREE_MOVEMENT= 0;
char TimeBuffer[30];
//-- CREATE OBJECTS --
AccelStepper stepper(1, STEP_PIN,DIR_PIN); //(1) means a stepper driver (with Step and Direction pins).
CounterStore counterStore;
Button left_button("LEFT_BUTTON", END_SWITCH_LEFT_PIN,200); 
Button right_button("RIGHT_BUTTON", END_SWITCH_RIGHT_PIN,200); 
Button interrupt_button("Interrupt_button",INTERRUPT_BUTTON,200);
LCD_I2C lcd(0x27,20,4);
LoopTimer timer;
RTC_DS3231 rtc;
DateTime startTime("Oct 12 2026", "00:00:00"); 
// HELPER FUNCTION PROTOTYPTES
void moveToPositionRelative(long position);
void Oscillate_Relative(int repititions,long distance_in_angle); // changed the function arguments
void moveToSwitch(Button& LimitSwitch, const int direction);
void showCycleCount(uint32_t count);
void saveCycleCount();
void lcd_row_clear(int row);
bool is_finished();
void show_time_left_on_console();
void Motor_init(bool);
long degree_to_steps(int);
void show_time_left_on_lcd(const DateTime& now); 
void setup()
{
  Serial.begin(115200); 
  delay(500);
  Motor_init(HIGH);
  stepper.setMaxSpeed(MOTOR_MAX_SPEED_STEPS_PER_SECOND);
  stepper.setAcceleration(MOTOR_ACCLERATION_STEPS_PER_SECOND_SQUARED); 
  stepper.setSpeed(MOTOR_SPEED_STEPS_PER_SECOND);
  lcd.init();
  counterStore.begin();
  interrupt_button.begin(true); // attach interrupt here
  const char firmwareId[] = __DATE__ " " __TIME__;
  if (!counterStore.resetIfFirmwareChanged(firmwareId, START_CYCLE_INDEX)) 
  {
    counterStore.loadLatest(currentCycleCount);
  } else 
  {
    currentCycleCount = START_CYCLE_INDEX;
  }
  timer.init(currentCycleCount,TOTAL_CYCLES);
  showCycleCount(currentCycleCount);
  Serial.println("Moving to switch A");
}

void loop()
{
  Serial.println("TESTING");
  moveToSwitch(left_button, DIR_RIGHT);  // move towards A, DIR_RIGHT moves the stepper CCW
  stepper.setCurrentPosition(0); // reset position
  moveToSwitch(right_button,DIR_LEFT);  // move towards B , a DIR_LEFT moves the stepper CW
  long middlePosition = (stepper.currentPosition()/2); // calculate Middle position
  moveToPositionRelative(DIR_LEFT*middlePosition); // move to middle position after reaching B
  STEPS_X_DEGREE_MOVEMENT = degree_to_steps(45);
  moveToPositionRelative(DIR_RIGHT*STEPS_X_DEGREE_MOVEMENT); // Move Initial - 45 degrees
  Oscillate_Relative(OSCILLATIONS,90);
  currentCycleCount++; // increment count
  saveCycleCount(); 
  showCycleCount(currentCycleCount);
  timer.update(currentCycleCount);
  show_time_left_on_console();
  show_time_left_on_lcd(startTime);
  if(is_finished())
  {
    while(1);
  }
}

void moveToPositionRelative(long position)
{
  stepper.setCurrentPosition(0);
  stepper.moveTo(position);
  while(stepper.distanceToGo() != 0)
  {
    if(interrupt_button.get_pause_state())
    {
    Serial.println("==PAUSED==");
    lcd.write_center(2,"==PAUSED==");
    while(interrupt_button.get_pause_state())
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

void Oscillate_Relative(int repititions,long distance_in_angle)
{
lcd_row_clear(1);
long angle_in_steps = degree_to_steps(distance_in_angle);
for (int i = 0; i < repititions;i++)
{
  moveToPositionRelative(-1*angle_in_steps); // move clockwise
  moveToPositionRelative(angle_in_steps);  // move counter clockwise
  String lcd_string = "Osc = "+String(i+1);
  lcd.write_center(1,lcd_string.c_str());
}
}

void moveToSwitch(Button& LimitSwitch, const int dir)
{
  if(dir == 1)
  {
     Serial.println("Set direction counter clockwise");
     stepper.setSpeed(DIR_RIGHT*MOTOR_SPEED_STEPS_PER_SECOND); // positive Speed moves counter clockwise
  }
  else if(dir == -1)
  {
     Serial.println("Set direction clockwise");
     stepper.setSpeed(DIR_LEFT*MOTOR_SPEED_STEPS_PER_SECOND); // negative Speed moves clockwise
  }
  while(!LimitSwitch.isActive())
  {
    //Serial.print("INTERRUPT BUTTON STATE : ");
    //Serial.println(interrupt_button.get_pause_state());
    if(interrupt_button.get_pause_state())
    {
      Serial.println("==PAUSED==");
      lcd.write_center(2,"==PAUSED==");
      while (interrupt_button.get_pause_state())
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

void saveCycleCount() 
{
  if (!counterStore.save(currentCycleCount)) {
    Serial.println("Warning: cycle counter could not be saved to EEPROM");
  }
}

void lcd_row_clear(int row)
{
    lcd.write_center(row,"                    ");
}

bool is_finished()
{
   if(currentCycleCount == TOTAL_CYCLES)
  {
    lcd.write_center(2,"TOTAL CYCLES REACHED");
    Serial.println("Total Cycles Reached");
    return true;
  }
  else
  {
    return false;
  }
}

void show_time_left_on_console()
{
  uint64_t TIME = timer.getRemainingS();
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

void Motor_init(bool set)
{
  pinMode(ENABLE_PIN, OUTPUT); 
  if(set)
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

void show_time_left_on_lcd(const DateTime& now)
{
  DateTime end = timer.estimateEndTime(now);
  char buf[] = "hh:mm:ss";
  end.toString(buf);
  snprintf(TimeBuffer, sizeof(TimeBuffer), "END TIME : %s", buf);
  lcd.write_center(3,TimeBuffer);
}