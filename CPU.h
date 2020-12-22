#ifndef CPU_H
#define CPU_H

#include <cstdint>

class CPU {
    uint8_t V[0x10]; // Data registers
    uint16_t I; // Address register
};

#endif
