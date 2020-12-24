#include "memory.h"

Memory::Memory() {
    // Clear the memory
    for (uint16_t i=0; i<0x1000; ++i) {
        //store(i,0);
        ram[i] = 0;
    }

    // Clear the stack
    for (int i=0; i<16; ++i) {
        stack[i] = 0;
    }

    // Stack pointer
    sp = 0;
}

void Memory::store(uint16_t addr, uint8_t byte) {
    ram[addr] = byte;
}

uint8_t Memory::load(uint16_t addr) {
    return ram[addr];
}

void Memory::dump() {
    puts("****** DUMPING RAM CONTENTS ******");
    for (uint16_t i=0; i<0x1000; ++i) {
        printf("%x:\t%x\n", i, load(i));
    }
}

// ------------- Stack function ------------- //

void Memory::stack_dump() {
    printf("Stack contents: ");
    for (int i=0; i<0x10; ++i) {
        printf("%x ", stack[i]);
    }
    
    printf(" {sp: %x}\n", sp);
}

void Memory::push(uint16_t addr) {
    stack[sp] = addr;
    sp++;
}

uint16_t Memory::pop() {
    sp--;
    return stack[sp];
}
 
