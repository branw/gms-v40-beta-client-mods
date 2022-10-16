#ifndef MAPLE_6AEC0C599614419AB86BF66F373D577C_HPP
#define MAPLE_6AEC0C599614419AB86BF66F373D577C_HPP

#include <cstdint>

struct COutPacket {
    virtual ~COutPacket() = 0;

    uint8_t *buffer;
    uint32_t length;
};

#endif//MAPLE_6AEC0C599614419AB86BF66F373D577C_HPP
