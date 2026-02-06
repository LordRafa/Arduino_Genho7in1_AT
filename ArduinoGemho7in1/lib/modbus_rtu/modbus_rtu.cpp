//
// Created by lordrafa on 30/01/26.
//

#include <cstring>
#include <cstdint>
#include <string>

#include "modbus_rtu.h"

using namespace std;

#define MODBUS_RTY_OVERHEAD_BYTES 5
#define MAX_RX_BYTES 517 // 256 reg uint16 + 5 CRC/STDC_HEADERS

// Read single register
size_t ModbusRTU::readHoldingReg(uint8_t* regs, const uint8_t addr, const uint8_t reg, const uint8_t nregs) const {
    if (nregs < 1) return 0;

    uint8_t frame[8] = {addr, 0x03, 0x00, reg, 0x00, nregs, 0x00, 0x00};
    const uint16_t crc = calcCrc16(frame, 6);
    frame[6] = static_cast<uint8_t>(crc & 0xFF); // CRC low
    frame[7] = static_cast<uint8_t>((crc >> 8) & 0xFF); // CRC high
    _channel.write(frame, sizeof(frame));

    uint8_t rx[MAX_RX_BYTES];
    const size_t n_reg_bytes = nregs * sizeof(uint16_t);
    const size_t n_bytes = MODBUS_RTY_OVERHEAD_BYTES + n_reg_bytes;
    const size_t n_read = _channel.read(rx, n_bytes);

    // If read byte is not the expected one return 0
    if ((n_read < 1) || (n_read != n_bytes)) return 0;

    // If CRC is wrong return 0
    const uint16_t crc_read = (rx[n_read - 1] << 8) + rx[n_read - 2];
    if (crc_read != calcCrc16(rx, n_read - 2)) return 0;

    const size_t payload_len = rx[2];
    memcpy(regs, &rx[3], payload_len);

    return payload_len / 2;
}

// private functions

// CRC16/MODBUS: poly 0xA001, init 0xFFFF. Append Low byte then High byte.
uint16_t ModbusRTU::calcCrc16(const uint8_t* data, const size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
            else crc >>= 1;
        }
    }
    return crc;
}
