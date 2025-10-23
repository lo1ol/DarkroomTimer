# Communication protocol

Communication protocol similar to I2C:
* There is 2 PINS: CLK and DATA. CLK is used to send signal for Slave for reading
* There is no addresses (compare to I2C)
* If device listen PIN, then it sets INPUT_PULLUP mode on it
* Only master initiate communication
* Master should give enoght time to slave for reading/sending data
* Master could send/read data any time when he wants it
* Slave should answer on it immidiately. It uses interrupts for it and buffering data

1. Master sets START condition. It's special case when DATA pin goes down when CLK was downed before
2. Master sends R/W bit. Master set propre value on DATA pin: 1 (sending), 0 (reading) and send pulse signal on CLK
3. Master waits for ACK response. Slave should set DATA pin to 0 state, if it accept it. Slave should do it at next pulse on CLK pin
4. Master reading/sending data to DATA pin bit by bit, sending pulse signal on each reading/sending
5. Master sends FINISH bit: 0 (continue reading/sending), 1(finish)
6. Master sets END condition. It's special case when DATA pin goes up when CLK was downed before
