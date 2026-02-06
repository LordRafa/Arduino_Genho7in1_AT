//
// Created by lordrafa on 30/01/26.
//

#ifndef ARDUINORS485CHANNEL_H
#define ARDUINORS485CHANNEL_H

#include <Serial.h>
#include <channel.h>

class ArduinoRS485Channel final : public Channel {
    UART& _uart;
    UART& _uart_debug;
    const uint8_t _de_pin;
    bool _debug_en = false;

    void rs485TxEnable(const bool en) const { digitalWrite(_de_pin, en ? HIGH : LOW); }
    void debugPrintHex(const std::string& prefix, const uint8_t* v, size_t len) const;

public:
    ArduinoRS485Channel(UART& uart, UART& uart_debug, uint8_t de_pin);
    ~ArduinoRS485Channel() override = default;

    int available() override { return _uart.available(); }

    int read() override { return _uart.read(); }
    size_t read(uint8_t* data, size_t maxlen) override;

    size_t write(const uint8_t b) override { return write(&b, 1); }
    size_t write(const uint8_t* data, size_t len) override;

    void flush() override { _uart.flush(); }

    void set_debug_en(const bool debug_en) { _debug_en = debug_en; }
};

#endif //ARDUINORS485CHANNEL_H
