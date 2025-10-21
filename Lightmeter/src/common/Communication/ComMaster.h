// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#pragma once

#include <stdint.h>

#include <Arduino.h>

namespace Communication {

class ComMaster {
public:
    void start();
    void stop();

    template<typename CmdT>
    inline bool sendCmd(CmdT cmd) {
        return sendCmd_(static_cast<uint8_t>(cmd));
    }

    template<typename CmdT, typename DataT>
    inline bool sendCmdWithData(CmdT cmd, const DataT& data) {
        return sendCmdWithData_(static_cast<uint8_t>(cmd), reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    }

    template<typename DataT>
    inline bool readData(DataT& data) {
        return readData_(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    }

    bool hasError() const { return m_error; }
    void resetError() { m_error = false; }

    void resetReadData();

private:
    bool sendCmd_(uint8_t cmd) { return sendCmdWithData_(cmd, nullptr, 0); }

    bool sendCmdWithData_(uint8_t cmd, const uint8_t* data, uint8_t dataLen);
    bool readData_(uint8_t* data, uint8_t dataLen);

    void startCond();
    void endCond();

    [[nodiscard]] bool sendBytes(const uint8_t* data, uint8_t len);
    [[nodiscard]] bool readBytes(uint8_t* data, uint8_t len);

    void sendByte(uint8_t b);
    [[nodiscard]] uint8_t readByte();

    void sendBit(bool bit);
    [[nodiscard]] bool readBit();

    void safeWriteData(bool val);

    bool m_error = false;
};

} // namespace Communication

