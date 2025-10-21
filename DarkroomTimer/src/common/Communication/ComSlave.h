// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#pragma once

#include <stdint.h>

namespace Communication {

class ComSlave {
public:
    ComSlave(int clkPin, int dataPin);

    template<class T>
    bool sendData(const T& data) {
        return addDataToSendBuf(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    }

    template<class T>
    bool readData(T& data) {
        return readDataFromReadBuf(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    }

    void reset();

    uint32_t lastCommunicationTimeMs() const;

private:
    bool addDataToSendBuf(const uint8_t* data, uint8_t len);
    bool readDataFromReadBuf(uint8_t* data, uint8_t len);

    void blockSending();
    void unblockSending();

    static void isrClkRise_();
    static void isrDataChange_();

    void isrDataChange();
    void isrClkRise();

    void safeWriteData(bool val);
    bool safeReadData();

    void waitStart();

    enum State {
        WaitStart,
        WaitRWBit,
        WaitRead,
        WaitAckRead,
        WaitNeedFinishRead,
        WaitSend,
        WaitAckSend,
        WaitNeedFinishSend,
    };

    static ComSlave* gInstance;

    uint8_t m_readBufSize = 0;
    uint8_t m_sendBufSize = 0;
    uint8_t m_readBuf[16] = {};
    uint8_t m_sendBuf[16] = {};
    uint8_t m_bit = 0;
    uint8_t m_byte = 0;
    volatile State m_state = State::WaitStart;
    volatile bool m_notStartTransaction = false;

    uint32_t m_lastCommunicationTimeMs = 0;
};

} // namespace Communication

