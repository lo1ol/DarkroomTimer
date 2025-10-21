// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#include "../../Config.h"
#include "../../Hardware.h"

#ifdef COMMUNICATION_MASTER_CLK_PIN

    #include "ComMaster.h"

    #include <Arduino.h>

namespace Communication {

void ComMaster::start() {
    gio::write(COMMUNICATION_MASTER_CLK_PIN, HIGH);
    gio::write(COMMUNICATION_MASTER_DATA_PIN, HIGH);
    gio::mode(COMMUNICATION_MASTER_CLK_PIN, OUTPUT);
    gio::mode(COMMUNICATION_MASTER_DATA_PIN, OUTPUT);
}

void ComMaster::stop() {
    gio::mode(COMMUNICATION_MASTER_CLK_PIN, INPUT);
    gio::mode(COMMUNICATION_MASTER_DATA_PIN, INPUT);
}

bool ComMaster::sendCmdWithData_(uint8_t cmd, const uint8_t* data, uint8_t dataLen) {
    uint8_t buf[16];
    buf[0] = cmd;
    memcpy(buf + 1, data, dataLen);
    bool rc = sendBytes(buf, dataLen + 1);
    if (!rc) {
        m_error = true;
        resetReadData();
    }

    delay(COMMUNICATION_SLAVE_HANDLE_TIME_MS);
    return rc;
}

bool ComMaster::readData_(uint8_t* data, uint8_t dataLen) {
    bool rc = readBytes(data, dataLen);
    if (!rc) {
        m_error = true;
        if (dataLen > 1)
            resetReadData();
    }

    return rc;
}

void ComMaster::resetReadData() {
    // reset all data
    uint8_t byte;
    while (readData(byte))
        ;
}

void ComMaster::startCond() {
    safeWriteData(LOW);
    gio::write(COMMUNICATION_MASTER_CLK_PIN, LOW);
    delayMicroseconds(COMMUNICATION_MASTER_DELAY_US);
}

void ComMaster::endCond() {
    gio::write(COMMUNICATION_MASTER_CLK_PIN, HIGH);
    delayMicroseconds(COMMUNICATION_MASTER_DELAY_US);
    safeWriteData(HIGH);
}

[[nodiscard]] bool ComMaster::sendBytes(const uint8_t* data, uint8_t len) {
    startCond();

    // rw bit
    sendBit(1);
    for (uint8_t i = 0; i != len; ++i) {
        // ack bit
        if (readBit()) {
            endCond();
            return false;
        }

        sendByte(data[i]);
        // find bit
        sendBit(i == (len - 1));
    }

    endCond();

    return true;
}

[[nodiscard]] bool ComMaster::readBytes(uint8_t* data, uint8_t len) {
    startCond();

    // rw bit
    sendBit(0);
    for (uint8_t i = 0; i != len; ++i) {
        // ack bit
        if (readBit()) {
            endCond();
            return false;
        }
        data[i] = readByte();
        // fin bit
        sendBit(i == (len - 1));
    }

    endCond();

    return true;
}

void ComMaster::sendByte(uint8_t b) {
    for (uint8_t i = 0; i != 8; ++i)
        sendBit((b >> i) & 0x01);
}

[[nodiscard]] uint8_t ComMaster::readByte() {
    uint8_t res = 0;
    for (uint8_t i = 0; i != 8; ++i)
        res |= readBit() << i;

    return res;
}

void ComMaster::sendBit(bool bit) {
    safeWriteData(bit);
    gio::write(COMMUNICATION_MASTER_CLK_PIN, HIGH);
    delayMicroseconds(1);
    gio::write(COMMUNICATION_MASTER_CLK_PIN, LOW);
    delayMicroseconds(COMMUNICATION_MASTER_DELAY_US);
}

[[nodiscard]] bool ComMaster::readBit() {
    gio::mode(COMMUNICATION_MASTER_DATA_PIN, INPUT_PULLUP);
    delayMicroseconds(COMMUNICATION_MASTER_DELAY_US);
    gio::write(COMMUNICATION_MASTER_CLK_PIN, HIGH);
    delayMicroseconds(1);
    gio::write(COMMUNICATION_MASTER_CLK_PIN, LOW);
    delayMicroseconds(COMMUNICATION_MASTER_DELAY_US);
    return gio::read(COMMUNICATION_MASTER_DATA_PIN);
}

void ComMaster::safeWriteData(bool val) {
    gio::write(COMMUNICATION_MASTER_DATA_PIN, val);
    gio::mode(COMMUNICATION_MASTER_DATA_PIN, OUTPUT);
    delayMicroseconds(COMMUNICATION_MASTER_DELAY_US);
}

} // namespace Communication

#endif

