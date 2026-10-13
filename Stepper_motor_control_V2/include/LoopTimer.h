#ifndef LOOP_TIMER_H
#define LOOP_TIMER_H

#include <Arduino.h>
#include <RTClib.h>   // for DateTime, TimeSpan

// Required libraries:
// - arduino-cli lib install "Arduino"
// - arduino-cli lib install "RTClib"
class LoopTimer {
public:
    LoopTimer();

    // Call once in setup, after everything is initialized
    void init(uint32_t loopStart, uint32_t loopEnd);

    // Call once per loop() iteration, passing the current loop counter
    void update(uint32_t loopCounter);

    // Returns true when an average is available (after >= 5 measured loops)
    bool hasEstimate() const { return estimateReady; }

    // Remaining test duration in s, based on the last loopCounter given to update()
    uint64_t getRemainingS() const;

    // Average loop duration in ms (0 if not ready)
    uint32_t getAverageLoopS() const { return avgLoopDurMs / 1000ULL; }

    // Estimate end time using an external "now" (e.g. rtc.now())
    DateTime estimateEndTime(const DateTime& now) const;

private:
    uint32_t loopStart;
    uint32_t loopEnd;
    uint32_t currentLoopCounter;

    uint16_t measuredLoopCount;

    uint32_t lastLoopTimeMs;
    uint32_t sumLoopDurMs;
    uint32_t avgLoopDurMs;    // ms
    bool     estimateReady;

    // Remaining test duration in ms for the currentLoopCounter
    uint64_t getRemainingMs() const;
    // Average loop duration in ms (0 if not ready)
    uint32_t getAverageLoopMs() const { return avgLoopDurMs; }
};

#endif // LOOP_TIMER_H
