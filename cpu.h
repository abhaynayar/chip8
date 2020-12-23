#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include "memory.h"

class CPU {
    uint8_t V[0x10]; // Data registers
    uint16_t I; // Address register
    uint16_t pc; // Program counter

    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;


public:
    CPU();
    Memory mem;
    
    void halt();
    void dump();
    uint8_t key();
    
    uint16_t fetch(); // Fetches opcode from the memory
    void execute(uint16_t); // Decodes and executes the opcode
};

#endif
