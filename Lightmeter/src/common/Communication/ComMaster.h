// THIS FILE SHOULD BE SYNCED WITH OTHER PROJECT
// CALL `scripts/sync_common.sh` before working with it

#pragma once

#include <stdint.h>

namespace Communication {

class ComMaster {
public:
    ComMaster();

    template<class T>
    bool sendData(const T& data) {
        return sendBytes(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
    }

    template<class T>
    [[nodiscard]] bool readData(T& data) {
        return readBytes(reinterpret_cast<uint8_t*>(&data), sizeof(data));
    }

    void resetReadData();

private:
    void start();
    void end();

    [[nodiscard]] bool sendBytes(const uint8_t* data, uint8_t len);
    [[nodiscard]] bool readBytes(uint8_t* data, uint8_t len);

    void sendByte(uint8_t b);
    [[nodiscard]] uint8_t readByte();

    void sendBit(bool bit);
    [[nodiscard]] bool readBit();

    void safeWriteData(bool val);
};

} // namespace Communication

