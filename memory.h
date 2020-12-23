#ifndef MEMORY_H
#define MEMORY_H

#include <cstdio>
#include <cstdint>

class Memory {
    uint8_t ram[0x1000]; // Memory - 4KiB
    uint16_t stack[16];  // 16 nested stack calls
    uint16_t sp; // Stack pointer

public:
    Memory();
    uint8_t load(uint16_t addr);
    void store(uint16_t addr, uint8_t byte);
    void dump();

    // Stack operations
    void stack_dump();
    void push(uint16_t addr);
    uint16_t pop();

};

#endif
