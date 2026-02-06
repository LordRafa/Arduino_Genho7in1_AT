//
// Created by lordrafa on 30/01/26.
//

#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstddef>
#include <cstdint>

struct Channel {
    virtual ~Channel() = default;
    virtual int available() = 0;
    virtual int read() = 0;
    virtual size_t read(uint8_t* data, size_t len) = 0;
    virtual size_t write(uint8_t b) = 0;
    virtual size_t write(const uint8_t* data, size_t len) = 0;
    virtual void flush() = 0;
};

#endif //CHANNEL_H
