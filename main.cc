#include "CPU.h"
#include "Memory.h"

#include <cstdio>
#include <cstdint>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        puts("Usage: chip8 <filename>");
        return -1;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "rb");

    if (fptr == NULL) {
        puts("File does not exist.");
        return -1;
    }

    Memory mem;

    // Copy code from file to memory
    // Program code starts from 0x200
    uint8_t byte;
    uint16_t addr = 0x200;
    while(fread(&byte, sizeof(byte), 1, fptr) != 0) {
        mem.store(addr, byte);
        addr++;
    }

    mem.dump();
    fclose(fptr);

/*
    // Fetch-decode-execute loop
    CPU cpu;
    uint32_t inst;
    do {
        inst = cpu.fetch();
        cpu.execute(inst);
        cpu.dump();
    } while(inst != 0);
*/ 
    return 0;
}

