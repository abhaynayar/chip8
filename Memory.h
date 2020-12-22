#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>
#include <cstdio>

class Memory {
    uint8_t ram[0x1000]; // Memory - 4KiB
public:
    Memory();
    uint8_t load(uint16_t addr);
    void store(uint16_t addr, uint8_t byte);
    void dump();
};

#endif
