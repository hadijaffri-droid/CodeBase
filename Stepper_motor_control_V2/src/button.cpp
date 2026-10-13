#include "Button.h"

bool PRESSED = false;  // button pin pulled up to 5V if button not pressed
bool UNPRESSED = true;
Button* Button::instance = nullptr;
Button::Button(const char* name, uint8_t buttonPin, int debounceDelay):
pin(buttonPin),
debounceDelay(debounceDelay), // default 50 ms
lastReading(HIGH),            // INPUT_PULLUP -> default unpressed = HIGH
isActiveVar(false),           // not pressed
lastChangeTime(0),
buttonName(name),
pressed(true)
{
  usedPinsArr[0] = {buttonPin};
  pinMode(buttonPin, INPUT);
}

// Button::~Button(){}

uint8_t Button::pinGet(){
  return pin;
}


void Button::debounceDelaySet(int delayIn_ms){
    debounceDelay = delayIn_ms;
}

int Button::debounceDelayGet(){
  return debounceDelay;
}

void Button::begin(bool useInterrupt)
{
  if(useInterrupt)
  {
    instance = this; // give the address of the interrupt button to the instance pointer
    attachInterrupt(digitalPinToInterrupt(this->pinGet()), Button::interruptWrapper, FALLING);
  }
}
void Button::interruptWrapper ()
{
 if (instance!= nullptr)
 {
   instance->handleInterrupt(); // use the interrupt wrapper to call the ISR
 }
}
void Button:: handleInterrupt ()
{
  if(this->isActive())
  {
    pressed = !pressed; // invert the state
    //Serial.println("CHANGE STATE");
  }
}

bool Button:: get_pause_state()
{
  return this->pressed;
}
bool Button::buttonChangedState() {
    //Serial.begin(9600);
    //Serial.print("Button changed state called");
    unsigned long now = millis();
    bool reading = digitalRead(pin);   // raw input
    //Serial.print(String("reading: ") + reading);
    //Serial.println(String("; lastReading: ") + lastReading);

    // If the reading changed, reset the debounce timer
    if (reading != lastReading) {
        lastChangeTime = now;
        lastReading = reading;
    }

    // Has the reading been stable long enough?
    if ((now - lastChangeTime) > (unsigned long)debounceDelay) {
        // interpret raw reading to 'pressed' boolean
        bool pressed = (reading == LOW);   // with INPUT_PULLUP: LOW = pressed

        if (pressed != isActiveVar) {
            // state changed
            isActiveVar = pressed;
            return true;
        }
    }

    return false;
}

bool Button::isActive(bool updateState) {
  if (updateState){
    //buttonChangedState();
    isActiveVar = !digitalRead(pinGet());
  }
  return isActiveVar;
}

bool Button::isActive(){
  return isActive(true);
}

bool Button::buttonWasPressed(){
  if (buttonChangedState() && isActiveVar){
    Serial.print("Button ");
    Serial.print(this->getName());
    Serial.println(" pressed");
    return true;
  } else {
    return false;
  }
}

bool Button::buttonWasUnpressed(){
  if (buttonChangedState() && !isActiveVar){
    Serial.print("Button ");
    Serial.print(this->getName());
    Serial.println(" unpressed");
    return true;
  } else {
    return false;
  }
}

const char* Button::getName() const {
  return buttonName;
}
