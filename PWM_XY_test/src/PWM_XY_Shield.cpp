#include "PWM_XY_Shield_cpp.h"

// ---------------------------------------------------------------- constructor
PWM_XY_Shield::PWM_XY_Shield(Stream& serial, const char* name)
  : ShieldBase(name), _ser(serial), usedPinsArr{0, 1}, _maxFrequency(DEFAULT_MAX_FREQ_HZ)
{
  response_buffer_count = 0;
}

// -------------------------------------------------------------------- getters
const char* PWM_XY_Shield::getName() const
{
  return ShieldBase::getName();
}

const uint8_t* PWM_XY_Shield::getUsedPins() const
{
  return usedPinsArr;
}

uint8_t PWM_XY_Shield:: getUsedPinsCount() const
{
  return 3;
}

// Sends "read" / "READ" alternately, finds "D<digits>" in the reply and returns the duty.
uint16_t PWM_XY_Shield::getDutyCycle()
{
  for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
  {
    Serial.print("Iteration no : ");
    Serial.print(attempt + 1);
    field_text = "";

    // alternate between both read commands so each is sent an equal number of times (is there a better way ?)
    if (attempt % 2 == 0)
    {
      _ser.print(read_commands[0]);
    }
    else
    {
      _ser.print(read_commands[1]);
    }

    response = readResponseBuffer();
    reply_length = response_buffer_count;
    scan_pos = 0;
    field_found = false;

    for (int marker_pos = 0; marker_pos < reply_length; marker_pos++)
    {
      // 'D' followed by a digit marks the duty information
      if (response[marker_pos] == 'D' &&
          (isdigit(response[marker_pos + 1])) &&
          marker_pos <= reply_length - 1)
      {
        // copy until the first '\0' or '\n'
        scan_pos = marker_pos;
        while ((response[scan_pos] != '\0' && response[scan_pos] != '\n') && scan_pos != reply_length)
        {
          field_text += response[scan_pos];
          scan_pos++;
        }
        field_found = true;
      }
    }

    if (field_found)
    {
      Serial.print(" Duty found = ");
      break;
    }
    else if (!field_found)
    {
      Serial.println(" Failed to parse duty cycle, trying again.... ");
    }
  }

  // convert the digits after the leading 'D' into a number
  number_start = field_text.c_str() + 1;
  unsigned long duty = strtoul(number_start, nullptr, 10);
  return duty;
}

// Sends "read" / "READ" alternately, finds "F<number>" in the reply and returns the frequency in Hz.
// Returns -1 if nothing could be parsed.
float PWM_XY_Shield::getFrequency()
{
  for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
  {
    Serial.print("Iteration no : ");
    Serial.print(attempt + 1);
    field_text = "";

    // alternate between both read commands so each is sent an equal number of times
    if (attempt % 2 == 0)
    {
      _ser.print(read_commands[0]);
    }
    else
    {
      _ser.print(read_commands[1]);
    }

    response = readResponseBuffer();
    reply_length = response_buffer_count;
    scan_pos = 0;
    field_found = false;

    for (int marker_pos = 0; marker_pos < reply_length; marker_pos++)
    {
      // 'F' followed by a digit or '.' means frequency information (not "FAIL")
      if (response[marker_pos] == 'F' &&
          (isdigit(response[marker_pos + 1]) || response[marker_pos + 1] == '.') &&
          marker_pos <= reply_length - 1)
      {
        // LPWM: "F1.40KHz\n", older module: no unit. Copy until '\0' or '\n'.
        scan_pos = marker_pos;
        while ((response[scan_pos] != '\0' && response[scan_pos] != '\n') && scan_pos != reply_length)
        {
          field_text += response[scan_pos];
          scan_pos++;
        }
        field_found = true;
      }
    }

    if (field_found)
    {
      Serial.print(" Frequency found = ");
      break;
    }
    else if (!field_found)
    {
      Serial.println(" Failed to parse frequency, trying again.... ");
    }
  }

  // nothing found: signal failure
  if (field_text.length() < 2)
  {
    return -1;
  }

  // skip the leading 'F' and read the number; unit_start points at the first character after it
  number_start = field_text.c_str() + 1;
  char* unit_start;
  float freq_hz = strtof(number_start, &unit_start); // strtof = string to float

  // no digits could be read
  if (unit_start == number_start)
  {
    return -1;
  }

  if (*unit_start == 'K' || *unit_start == 'k')       // LPWM module, kHz
  {
    freq_hz *= 1000.0f;
  }
  else if (*unit_start != 'H' && *unit_start != 'z')  // older module, no unit (kHz)
  {
    freq_hz *= 1000.0f;
  }

  return freq_hz;
}

// -------------------------------------------------------------------- setters
void PWM_XY_Shield::setName(const char* newName)
{
  ShieldBase::setName(newName);
}

// Builds the command for the module's range format, then retries until the module answers DOWN.
bool PWM_XY_Shield::setFrequency(uint32_t hz)
{
  if (hz >= _maxFrequency)
  {
    return false;
  }

  // Command format per range:
  //   1 - 999 Hz       -> F%03u      e.g. 100    -> "F100"
  //   1 - 9.99 kHz     -> F%u.%02u   e.g. 3600   -> "F3.60"   
  //   10 - 99.9 kHz    -> F%u.%u     e.g. 68300  -> "F68.3"   
  //   100 kHz and up   -> F%u.%u.%u  e.g. 105000 -> "F1.0.5" 
  const uint32_t khz          = hz / 1000;   // whole kHz
  const uint32_t hz_below_khz = hz % 1000;   // leftover Hz (0..999)

  if (hz < 1000)
  {
    snprintf(command_buffer, sizeof(command_buffer), "F%03u", (unsigned)hz);
  }
  else if (hz < 10000)
  {
    const uint32_t tens_of_hz = hz_below_khz / 10;
    snprintf(command_buffer, sizeof(command_buffer), "F%u.%02u",
             (unsigned)khz, (unsigned)tens_of_hz);
  }
  else if (hz < 100000)
  {
    const uint32_t hundreds_of_hz = hz_below_khz / 100;
    snprintf(command_buffer, sizeof(command_buffer), "F%u.%u",
             (unsigned)khz, (unsigned)hundreds_of_hz);
  }
  else
  {
    const uint32_t hundreds_digit = khz / 100;
    const uint32_t tens_digit     = (khz / 10) % 10;
    const uint32_t ones_digit     = khz % 10;
    snprintf(command_buffer, sizeof(command_buffer), "F%u.%u.%u",
             (unsigned)hundreds_digit, (unsigned)tens_digit, (unsigned)ones_digit);
  }

  for (int attempt = 0; attempt < MAX_ATTEMPTS; attempt++)
  {
    while (_ser.available()) _ser.read();   // drop garbage bytes

    _ser.print(command_buffer);
    response = readResponseBuffer();

    // the LPWM module may reply "FAIL\nDOWN\n", so DOWN anywhere in the reply counts as success
    if (strstr(response, "DOWN") != nullptr) // strstr = string search, searches for a substring within a string, returns a pointer to the start of the substring if found
    {
      freq_value = hz;
      return true;
    }
  }
  return false;   // never received DOWN
}

bool PWM_XY_Shield::setDutyCycle(uint8_t percentage)
{
  snprintf(command_buffer, sizeof(command_buffer), "D%03u", (unsigned)percentage);
  _ser.print(command_buffer);
  response = readResponseBuffer();
  if (strcmp(response, "DOWN") == 0)
  {
    return true;
  }
  else if (strcmp(response, "FAIL") == 0 || strcmp(response, "FALL") == 0)
  {
    return false;
  }
}

bool PWM_XY_Shield::setMode(int mode)
{
  if (mode == 1 || mode == 0)
  {
    if (mode == 1)
    {
      snprintf(command_buffer, sizeof(command_buffer), "MODE%u", (unsigned)mode);
    }
    else if (mode == 0)
    {
      snprintf(command_buffer, sizeof(command_buffer), "MODE%u", (unsigned)mode);
    }
    return true;
  }
  else
  {
    return false;
  }
}

void PWM_XY_Shield::pwm_enable()   // does not work yet
{
  _ser.print("ON");
  response = readResponseBuffer();
}

void PWM_XY_Shield::pwm_disable()  // does not work yet
{
  snprintf(command_buffer, sizeof(command_buffer), "OFF");
  _ser.print(command_buffer);
  response = readResponseBuffer();
}

// ------------------------------------------------------------- serial helpers
// Collects every byte the module sends for 200 ms and returns it as a C string.
const char* PWM_XY_Shield::readResponseBuffer()
{
  response_buffer_count = 0;
  uint32_t start = millis();
  while (millis() - start < 200)
  {
    while (_ser.available() > 0)
    {
      char chIn = _ser.read();
      if (response_buffer_count < sizeof(response_buffer) - 1)
      {
        response_buffer[response_buffer_count++] = chIn;
      }
    }
  }
  response_buffer[response_buffer_count] = '\0';
  return response_buffer;
}

// Debug helper: sends one command and prints the raw reply to the console.
// This was used to test what the different version of the PWM modules return when they are given commands in different formats
// List of commands = "READ","F1.04","F1004","read","D054","d054","f1.04","ON","OFF"
void PWM_XY_Shield::probe(const char* command)
{
  while (_ser.available())
  {
    _ser.read();
  }
  _ser.print(command);
  delay(100);
  Serial.print(command);
  Serial.print(" -> ");
  while (_ser.available())
  {
    Serial.write(_ser.read());
    Serial.print(' ');
  }
  Serial.println();
}
