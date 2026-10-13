#ifndef SHIELD_BASE_H
#define SHIELD_BASE_H
#include <Arduino.h>

// Required libraries:
// - arduino-cli lib install "Arduino"
class ShieldBase {
private:
    const char* name;
public:
    // explicit prevents automatic data type conversion , const Type* ptr prevents the object from being modified but you can change the pointer address
    // Type* const ptr means its a constant ptr, address cannnot be changed but the object can be changed
    explicit ShieldBase(const char* defaultName = "Shield") : name(defaultName ? defaultName : "Shield") {} 
    virtual ~ShieldBase() {};

    virtual const char* getName() const {return name;}
    virtual void setName(const char* newName) { name = newName ? newName : "Shield"; }

    virtual const uint8_t* getUsedPins() const = 0;

    // Number of pins in that array
    virtual uint8_t getUsedPinsCount() const = 0;

    // Optional: initialization hook
    virtual void init(){};
};

#endif