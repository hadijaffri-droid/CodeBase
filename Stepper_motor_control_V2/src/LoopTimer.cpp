#include "LoopTimer.h"

LoopTimer::LoopTimer()
: loopStart(0),
  loopEnd(0),
  currentLoopCounter(0),
  measuredLoopCount(0),
  lastLoopTimeMs(0),
  sumLoopDurMs(0),
  avgLoopDurMs(0),
  estimateReady(false)
{
}

void LoopTimer::init(uint32_t loopStart_, uint32_t loopEnd_) {
    loopStart          = loopStart_;
    loopEnd            = loopEnd_;
    currentLoopCounter = loopStart_;
    lastLoopTimeMs     = millis();
    sumLoopDurMs       = 0;
    measuredLoopCount  = 0;
    avgLoopDurMs       = 0;
    estimateReady      = false;
}

void LoopTimer::update(uint32_t loopCounter) {
    currentLoopCounter = loopCounter;      // store latest value

    uint32_t nowMs     = millis();
    uint32_t loopDurMs = nowMs - lastLoopTimeMs;
    lastLoopTimeMs     = nowMs;

    // Only measure when we are inside the test range:
    if (loopCounter <= loopStart + 1 || loopCounter > loopEnd) {
        return;
    }

    // First 5 loops -> simple average
    if (measuredLoopCount < 5) {
        sumLoopDurMs      += loopDurMs;
        measuredLoopCount += 1;

        if (measuredLoopCount == 5) {
            avgLoopDurMs  = sumLoopDurMs / measuredLoopCount;
            estimateReady = true;
        }
    } else {
        // After that -> exponential moving average (weight 9/10 old, 1/10 new)
        avgLoopDurMs = (avgLoopDurMs * 9 + loopDurMs) / 10;
    }
}

uint64_t LoopTimer::getRemainingMs() const {
    if (!estimateReady) return 0;
    if (currentLoopCounter >= loopEnd) return 0;

    uint32_t remainingLoops = loopEnd - currentLoopCounter;
    uint64_t remainingMs    = (uint64_t)remainingLoops * (uint64_t)avgLoopDurMs;
    return remainingMs;
}

uint64_t LoopTimer::getRemainingS() const {
    uint64_t remainingMs = getRemainingMs();
    return remainingMs / 1000ULL;
}

DateTime LoopTimer::estimateEndTime(const DateTime& now) const {
    uint64_t remainingSeconds = getRemainingS();
    if (remainingSeconds == 0) {
        return now;
    }
    TimeSpan delta((int32_t)remainingSeconds);
    return now + delta;
}
