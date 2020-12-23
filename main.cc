#include <cstdio>
#include <cstdint>

#include "cpu.h"
#include "memory.h"
#include "screen.h"

// Using SDL and standard IO
#include <SDL2/SDL.h>  
#include <SDL2/SDL_thread.h>

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        puts("Usage: chip8 <filename>");
        return -1;
    }

    // ---------- Reading from file ----------- //

    FILE *fptr;
    fptr = fopen(argv[1], "rb");

    if (fptr == NULL) {
        puts("File does not exist.");
        return -1;
    }

    CPU cpu; // Also contains memory

    // Copy code from file to memory
    // Program code starts from 0x200
    uint8_t byte;
    uint16_t addr = 0x200;
    while(fread(&byte, sizeof(byte), 1, fptr) != 0) {
        cpu.mem.store(addr, byte);
        addr++;
    }

    //cpu.mem.dump();
    fclose(fptr);

    // ----------- Spawn a screen thread -------------- //
    
    //Screen scr; // Initializing SDL screen object
    //SDL_Thread* threadID = SDL_CreateThread(Screen::drawThread, "LazyThread", NULL);

    // ----------- Fetch-decode-execute loop ----------//
 
    uint32_t inst;
    for (;;) {
        //cpu.dump(); // TODO: Fix infinite loop
        uint16_t opcode = cpu.fetch();
        printf("Current opcode: %x\n", opcode);
        cpu.execute(opcode); // Also includes "decode"
    }

    //SDL_WaitThread(threadID, NULL);
    return 0;
}

