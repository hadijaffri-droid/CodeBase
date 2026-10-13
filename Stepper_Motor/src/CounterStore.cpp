#include "CounterStore.h"
#include <string.h>

CounterStore::CounterStore()
    : slotCount(0),
      lastSlot(-1),
      lastSeq(0) {
}

void CounterStore::begin() {
    const int totalBytes = EEPROM.length();
    if (totalBytes <= HEADER_SIZE) {
        slotCount = 0;
        return;
    }
    slotCount = uint16_t((totalBytes - HEADER_SIZE) / RECORD_SIZE);
}

bool CounterStore::resetIfFirmwareChanged(const char* firmwareId, uint32_t resetValue) {
    if (slotCount == 0 || firmwareId == nullptr) {
        return false;
    }

    const uint8_t fwLen = uint8_t(strnlen(firmwareId, FW_ID_MAX_LEN));
    uint8_t expectedCrc = fwLen;
    for (uint8_t i = 0; i < fwLen; ++i) {
        expectedCrc ^= uint8_t(firmwareId[i]);
    }

    const uint8_t m0 = EEPROM.read(0);
    const uint8_t m1 = EEPROM.read(1);
    const uint8_t lenStored = EEPROM.read(2);
    const uint8_t crcStored = EEPROM.read(3);

    bool sameFirmware = false;
    if (m0 == FW_MAGIC0 && m1 == FW_MAGIC1 && lenStored == fwLen && crcStored == expectedCrc) {
        sameFirmware = true;
        for (uint8_t i = 0; i < fwLen; ++i) {
            if (EEPROM.read(4 + i) != uint8_t(firmwareId[i])) {
                sameFirmware = false;
                break;
            }
        }
    }

    if (sameFirmware) {
        return false;
    }

    // Different firmware: invalidate all records, reset sequence tracking, and write new firmware marker.
    const int totalBytes = EEPROM.length();
    for (int addr = HEADER_SIZE; addr < totalBytes; ++addr) {
        EEPROM.write(addr, 0xFF);
    }
    lastSlot = -1;
    lastSeq = 0;

    EEPROM.write(0, FW_MAGIC0);
    EEPROM.write(1, FW_MAGIC1);
    EEPROM.write(2, fwLen);
    EEPROM.write(3, expectedCrc);
    for (uint8_t i = 0; i < FW_ID_MAX_LEN; ++i) {
        const uint8_t ch = (i < fwLen) ? uint8_t(firmwareId[i]) : 0;
        EEPROM.write(4 + i, ch);
    }

    saveRunState(false);

    save(resetValue);
    return true;
}

void CounterStore::saveRunState(bool runState) {
    EEPROM.write(RUN_STATE_ADDR_MAGIC, RUN_STATE_MAGIC);
    EEPROM.write(RUN_STATE_ADDR_VALUE, runState ? 1 : 0);
}

bool CounterStore::loadRunState(bool& runStateOut) const {
    if (EEPROM.read(RUN_STATE_ADDR_MAGIC) != RUN_STATE_MAGIC) {
        return false;
    }
    runStateOut = EEPROM.read(RUN_STATE_ADDR_VALUE) != 0;
    return true;
}

uint8_t CounterStore::checksum(const uint8_t* data, uint8_t len) const {
    uint8_t c = 0;
    for (uint8_t i = 0; i < len; ++i) {
        c ^= data[i];
    }
    return c;
}

bool CounterStore::isNewer(uint32_t a, uint32_t b) const {
    return int32_t(a - b) > 0;
}

void CounterStore::encodeRecord(uint8_t* rec, uint32_t seq, uint32_t counter) const {
    rec[0] = MAGIC0;
    rec[1] = MAGIC1;

    rec[2] = uint8_t(seq & 0xFF);
    rec[3] = uint8_t((seq >> 8) & 0xFF);
    rec[4] = uint8_t((seq >> 16) & 0xFF);
    rec[5] = uint8_t((seq >> 24) & 0xFF);

    rec[6] = uint8_t(counter & 0xFF);
    rec[7] = uint8_t((counter >> 8) & 0xFF);
    rec[8] = uint8_t((counter >> 16) & 0xFF);
    rec[9] = uint8_t((counter >> 24) & 0xFF);

    rec[10] = 0;
    rec[11] = checksum(rec, 11);

    rec[12] = 0xFF;
    rec[13] = 0xFF;
    rec[14] = 0xFF;
    rec[15] = 0xFF;
}

bool CounterStore::decodeRecord(const uint8_t* rec, uint32_t& seqOut, uint32_t& counterOut) const {
    if (rec[0] != MAGIC0 || rec[1] != MAGIC1) {
        return false;
    }

    const uint8_t expected = checksum(rec, 11);
    if (rec[11] != expected) {
        return false;
    }

    seqOut = uint32_t(rec[2]) |
             (uint32_t(rec[3]) << 8) |
             (uint32_t(rec[4]) << 16) |
             (uint32_t(rec[5]) << 24);

    counterOut = uint32_t(rec[6]) |
                 (uint32_t(rec[7]) << 8) |
                 (uint32_t(rec[8]) << 16) |
                 (uint32_t(rec[9]) << 24);

    return true;
}

void CounterStore::readSlot(uint16_t slot, uint8_t* rec) const {
    const int base = HEADER_SIZE + int(slot) * RECORD_SIZE;
    for (uint8_t i = 0; i < RECORD_SIZE; ++i) {
        rec[i] = EEPROM.read(base + i);
    }
}

void CounterStore::writeSlot(uint16_t slot, const uint8_t* rec) {
    const int base = HEADER_SIZE + int(slot) * RECORD_SIZE;
    for (uint8_t i = 0; i < RECORD_SIZE; ++i) {
        EEPROM.write(base + i, rec[i]);
    }
}

bool CounterStore::loadLatest(uint32_t& counterOut) {
    if (slotCount == 0) {
        return false;
    }

    bool found = false;
    uint32_t bestSeq = 0;
    uint32_t bestCounter = 0;
    int16_t bestSlot = -1;

    uint8_t rec[RECORD_SIZE];

    for (uint16_t slot = 0; slot < slotCount; ++slot) {
        readSlot(slot, rec);

        uint32_t seq = 0;
        uint32_t counter = 0;
        if (!decodeRecord(rec, seq, counter)) {
            continue;
        }

        if (!found || isNewer(seq, bestSeq)) {
            found = true;
            bestSeq = seq;
            bestCounter = counter;
            bestSlot = int16_t(slot);
        }
    }

    if (!found) {
        return false;
    }

    lastSeq = bestSeq;
    lastSlot = bestSlot;
    counterOut = bestCounter;
    return true;
}

bool CounterStore::save(uint32_t counterValue) {
    if (slotCount == 0) {
        return false;
    }

    const uint16_t nextSlot = (lastSlot < 0)
        ? 0
        : uint16_t((uint16_t(lastSlot) + 1) % slotCount);

    const uint32_t seq = (lastSlot < 0) ? 1 : (lastSeq + 1);

    uint8_t rec[RECORD_SIZE];
    encodeRecord(rec, seq, counterValue);
    writeSlot(nextSlot, rec);

    lastSlot = int16_t(nextSlot);
    lastSeq = seq;
    return true;
}
