#ifndef COUNTER_STORE_H
#define COUNTER_STORE_H

#include <Arduino.h>
#include <EEPROM.h>

class CounterStore {
public:
    CounterStore();

    void begin();
    bool resetIfFirmwareChanged(const char* firmwareId, uint32_t resetValue);
    bool loadLatest(uint32_t& counterOut);
    bool save(uint32_t counterValue);
    void saveRunState(bool runState);
    bool loadRunState(bool& runStateOut) const;

private:
    static const uint8_t HEADER_SIZE = 32;
    static const uint8_t RECORD_SIZE = 16;
    static const uint8_t MAGIC0 = 0xA5;
    static const uint8_t MAGIC1 = 0x5A;
    static const uint8_t FW_MAGIC0 = 0xC3;
    static const uint8_t FW_MAGIC1 = 0x3C;
    static const uint8_t FW_ID_MAX_LEN = 24;
    static const uint8_t RUN_STATE_MAGIC = 0xA9;
    static const uint8_t RUN_STATE_ADDR_MAGIC = 28;
    static const uint8_t RUN_STATE_ADDR_VALUE = 29;

    uint16_t slotCount;
    int16_t lastSlot;
    uint32_t lastSeq;

    uint8_t checksum(const uint8_t* data, uint8_t len) const;
    bool isNewer(uint32_t a, uint32_t b) const;

    void encodeRecord(uint8_t* rec, uint32_t seq, uint32_t counter) const;
    bool decodeRecord(const uint8_t* rec, uint32_t& seqOut, uint32_t& counterOut) const;

    void readSlot(uint16_t slot, uint8_t* rec) const;
    void writeSlot(uint16_t slot, const uint8_t* rec);
};

#endif
