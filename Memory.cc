#include "Memory.h"

Memory::Memory() {
    for (uint16_t i=0; i<0x1000; ++i) {
        store(i,0);
    }
}

void Memory::store(uint16_t addr, uint8_t byte) {
    ram[addr] = byte;
}

uint8_t Memory::load(uint16_t addr) {
    return ram[addr];
}

void Memory::dump() {
    puts("Dumping memory:");
    for (uint16_t i=0; i<0x1000; ++i) {
        printf("%x:\t%x\n", i, load(i));
    }
}


