//
// Created by lordrafa on 31/01/26.
//

#include "ArduinoRS485Channel.h"

#define RS485_READ_TIMEOUT 2000
#define RS485_PRE_WRITE_DELAY_US 200
#define RS485_POST_WRITE_DELAY_US 1400

using namespace std;

ArduinoRS485Channel::ArduinoRS485Channel(UART& uart, UART& uart_debug, const uint8_t de_pin):
    Channel(), _uart(uart), _uart_debug(uart_debug), _de_pin(de_pin) {
    pinMode(_de_pin, OUTPUT);
    rs485TxEnable(false);
}

size_t ArduinoRS485Channel::read(uint8_t* data, const size_t maxlen) {
    size_t n = 0;
    uint32_t start = millis();

    while (millis() - start < RS485_READ_TIMEOUT) {
        while (_uart.available() > 0 && n < maxlen) {
            data[n++] = static_cast<uint8_t>(_uart.read());
            start = millis(); // extend while bytes keep coming
        }
        if (n == maxlen) break;
    }

    debugPrintHex("rs485 read: ", data, n);
    return n;
}

size_t ArduinoRS485Channel::write(const uint8_t* data, const size_t len) {
    debugPrintHex("rs485 sending: ", data, len);

    rs485TxEnable(true);
    delayMicroseconds(RS485_PRE_WRITE_DELAY_US);

    const size_t n = _uart.write(data, len);
    _uart.flush();

    delayMicroseconds(RS485_POST_WRITE_DELAY_US);
    rs485TxEnable(false);

    return n;
}

void ArduinoRS485Channel::debugPrintHex(const string& prefix, const uint8_t* v, const size_t len) const {
    if (!_debug_en) return;

    _uart_debug.print("(debug) - ");
    _uart_debug.print(prefix.c_str());
    for (size_t i = 0; i < len; i++) {
        if (v[i] < 0x10)
            _uart_debug.print('0'); // leading zero
        _uart_debug.print(v[i], HEX);
        _uart_debug.print(' ');
    }
    _uart_debug.print("\r\n");
}
