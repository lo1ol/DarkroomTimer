// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#include "../../Config.h"

#ifdef COMMUNICATION_SLAVE_CLK_PIN

    #include "ComSlave.h"

    #include <Arduino.h>

    #include <GyverIO.h>

namespace Communication {

ComSlave* ComSlave::gInstance = nullptr;

ComSlave::ComSlave(int clkPin, int dataPin) {
    noInterrupts();
    gio::mode(COMMUNICATION_SLAVE_CLK_PIN, INPUT_PULLUP);
    gio::mode(COMMUNICATION_SLAVE_DATA_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(COMMUNICATION_SLAVE_DATA_PIN), isrDataChange_, CHANGE);
    attachInterrupt(digitalPinToInterrupt(COMMUNICATION_SLAVE_CLK_PIN), isrClkRise_, RISING);
    gInstance = this;
    interrupts();
}

bool ComSlave::addDataToSendBuf(const uint8_t* data, uint8_t len) {
    if (len + m_sendBufSize > sizeof(m_sendBuf))
        return false;

    // Serial.println("send");
    blockSending();

    memcpy(m_sendBuf + m_sendBufSize, data, len);
    m_sendBufSize += len;

    unblockSending();
    return true;
}

[[nodiscard]] bool ComSlave::readDataFromReadBuf(uint8_t* data, uint8_t len) {
    if (len > m_readBufSize)
        return false;

    // Serial.println("read");
    blockSending();

    memcpy(data, m_readBuf, len);
    memcpy(m_readBuf, m_readBuf + len, m_readBufSize - len);
    m_readBufSize -= len;

    unblockSending();
    return true;
}

void ComSlave::reset() {
    // Serial.println("reset");

    blockSending();

    m_readBufSize = 0;
    m_sendBufSize = 0;
    waitStart();

    unblockSending();
}

void ComSlave::blockSending() {
    // Serial.println("start block");
    m_notStartTransaction = true;
    while (m_state != State::WaitStart)
        ;
    noInterrupts();
    // Serial.println("end block");
}
void ComSlave::unblockSending() {
    // Serial.println("start unblock");
    m_notStartTransaction = false;
    interrupts();
    // Serial.println("end unblock");
}

void ComSlave::isrDataChange_() {
    if (!gInstance)
        return;

    gInstance->isrDataChange();
}

void ComSlave::isrClkRise_() {
    if (!gInstance)
        return;

    gInstance->isrClkRise();
}

void ComSlave::isrDataChange() {
    m_lastCommunicationTimeMs = millis();

    if (!gio::read(COMMUNICATION_SLAVE_CLK_PIN))
        return;

    if (safeReadData() || m_notStartTransaction) {
        waitStart();
        return;
    }

    m_state = State::WaitRWBit;
    // Serial.println("start");
}

void ComSlave::isrClkRise() {
    m_lastCommunicationTimeMs = millis();

    switch (m_state) {
    case State::WaitStart:
        // Serial.println("nothing");
        return;
    case State::WaitRWBit:
        // Serial.print("rwbit ");
        if (safeReadData()) {
            // Serial.println(1);
            m_state = State::WaitAckRead;
            m_byte = 0;
        } else {
            // Serial.println(0);
            m_state = State::WaitAckSend;
        }
        m_bit = 0;
        break;
    case State::WaitAckRead:
        // Serial.print("ack read ");
        if (m_readBufSize == sizeof(m_readBuf)) {
            // Serial.println(1);
            waitStart();
            // Serial.println("end impl");
        } else {
            // Serial.println(0);
            safeWriteData(LOW);
            m_state = State::WaitRead;
        }
        m_byte = 0;
        m_bit = 0;
        break;
    case State::WaitRead: {
        // Serial.print("read ");
        bool bit = safeReadData();
        m_byte |= bit << (m_bit++);
        // Serial.println(bit);

        if (m_bit == 8) {
            m_readBuf[m_readBufSize++] = m_byte;
            m_state = State::WaitNeedFinishRead;
        }
    } break;
    case State::WaitNeedFinishRead:
        // Serial.print("need finish read ");
        if (safeReadData()) {
            // Serial.println(1);
            waitStart();
            // Serial.println("end impl");
        } else {
            // Serial.println(0);
            m_state = State::WaitAckRead;
        }
        break;
    case State::WaitAckSend:
        // Serial.print("ack send ");
        if (m_sendBufSize) {
            // Serial.println(0);
            safeWriteData(false);
            m_byte = m_sendBuf[0];
            memcpy(m_sendBuf, m_sendBuf + 1, m_sendBufSize - 1);
            --m_sendBufSize;
            m_bit = 0;
            m_state = WaitSend;
        } else {
            // Serial.println(1);
            waitStart();
            // Serial.println("end impl");
        }
        break;
    case State::WaitSend: {
        // Serial.print("send ");
        bool val = (m_byte >> (m_bit++)) & 1;
        // Serial.println(val);
        safeWriteData(val);
        if (m_bit == 8) {
            m_state = State::WaitNeedFinishSend;
        }
    } break;
    case State::WaitNeedFinishSend:
        // Serial.print("need finish send ");
        if (safeReadData()) {
            // Serial.println(1);
            waitStart();
            // Serial.println("end impl");
        } else {
            // Serial.println(0);
            m_state = State::WaitAckSend;
        }
        break;
    }

    // reset interrupts
    EIFR = (1 << INTF0) | (1 << INTF1);
}

void ComSlave::safeWriteData(bool val) {
    gio::write(COMMUNICATION_SLAVE_DATA_PIN, val);
    gio::mode(COMMUNICATION_SLAVE_DATA_PIN, OUTPUT);
}

[[nodiscard]] bool ComSlave::safeReadData() {
    gio::mode(COMMUNICATION_SLAVE_DATA_PIN, INPUT_PULLUP);
    return gio::read(COMMUNICATION_SLAVE_DATA_PIN);
}

void ComSlave::waitStart() {
    gio::mode(COMMUNICATION_SLAVE_DATA_PIN, INPUT_PULLUP);
    m_state = State::WaitStart;
}

uint32_t ComSlave::lastCommunicationTimeMs() const {
    return millis() - m_lastCommunicationTimeMs;
}

} // namespace Communication

#endif
