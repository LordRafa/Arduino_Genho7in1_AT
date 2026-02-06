//
// Created by lordrafa on 30/01/26.
//

#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <channel.h>

class ModbusRTU {
    Channel& _channel;
    static uint16_t calcCrc16(const uint8_t* data, size_t len);

public:
    explicit ModbusRTU(Channel& channel): _channel(channel) {}
    size_t readHoldingReg(uint8_t* regs, uint8_t addr, uint8_t reg, uint8_t nregs) const;
};

#endif //MODBUS_RTU_H
