//
// Created by lordrafa on 30/01/26.
//

#ifndef ARDUINOSERIALCHANNEL_H
#define ARDUINOSERIALCHANNEL_H

#include <channel.h>

class ArduinoSerialChannel final : public Channel {
    UART& _uart;

public:
    explicit ArduinoSerialChannel(UART& uart): Channel(), _uart(uart) {}
    ~ArduinoSerialChannel() override = default;

    int available() override { return _uart.available(); }

    int read() override { return _uart.read(); }
    size_t read(uint8_t* data, const size_t len) override { return 0; } // <-- unused

    size_t write(const uint8_t b) override { return _uart.write(b); }
    size_t write(const uint8_t* data, const size_t len) override { return _uart.write(data, len); }

    void flush() override {
        Serial.flush();
    }
};

#endif //ARDUINOSERIALCHANNEL_H
