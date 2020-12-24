#include <cstdio>
#include <cstdint>
#include <ncurses.h>

#include "cpu.h"
#include "memory.h"

extern uint8_t gfx[32][64];
extern uint8_t key_pressed;

int max(int a, int b) {
    if (a>b) return a;
    else return b;
}

uint8_t fonts[16][5] = {
    { 0xF0, 0x90, 0x90, 0x90, 0xF0 },
    { 0x20, 0x60, 0x20, 0x20, 0x70 },
    { 0xF0, 0x10, 0xF0, 0x80, 0xF0 },
    { 0xF0, 0x10, 0xF0, 0x10, 0xF0 },
    { 0x90, 0x90, 0xF0, 0x10, 0x10 },
    { 0xF0, 0x80, 0xF0, 0x10, 0xF0 },
    { 0xF0, 0x80, 0xF0, 0x90, 0xF0 },
    { 0xF0, 0x10, 0x20, 0x40, 0x40 },
    { 0xF0, 0x90, 0xF0, 0x90, 0xF0 },
    { 0xF0, 0x90, 0xF0, 0x10, 0xF0 },
    { 0xF0, 0x90, 0xF0, 0x90, 0x90 },
    { 0xE0, 0x90, 0xE0, 0x90, 0xE0 },
    { 0xF0, 0x80, 0x80, 0x80, 0xF0 },
    { 0xE0, 0x90, 0x90, 0x90, 0xE0 },
    { 0xF0, 0x80, 0xF0, 0x80, 0xF0 },
    { 0xF0, 0x80, 0xF0, 0x80, 0x80 }
};

uint8_t key_convert(uint8_t x) {
    if (x >= 'a' && x <= 'f') return x-'a'+10;
    else if (x >= '0' && x <= '9') return x-'0';
    else return 222;
}

// TODO:
// - Fix paddle ghost after collision in breakout
// - Only update screen when draw flag is set
// - Fix global variables
// - Put opcode in cpu.dump()

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        puts("Usage: chip8 <filename>");
        return -1;
    }

    // ---------- Loading ROM into memory ---------- //

    FILE *fptr;
    fptr = fopen(argv[1], "rb");

    if (fptr == NULL) {
        puts("File does not exist.");
        return -1;
    }

    CPU cpu;

    // Program code starts from 0x200
    uint16_t addr = 0x200;
    
    uint8_t byte;
    while(fread(&byte, sizeof(byte), 1, fptr) != 0) {
        cpu.mem.store(addr, byte);
        addr++;
    }

    //cpu.mem.dump();
    fclose(fptr);

    // --------- Loading fonts into memory ---------- //

    addr = 0;
    for (int i=0; i<0xF; ++i) {
        for (int j=0; j<5; ++j) {
            cpu.mem.store(addr, fonts[i][j]);
            addr++;
        }
    }

    // --------- Fetch-decode-execute loop ----------//

    initscr();
    for (int clock=0;; clock++) {

        // Read the keyboard
        timeout(1);
        uint8_t debounce = getch();
        if (debounce != 255){
            key_pressed = key_convert(debounce);
        }

        // Execute a CPU cycle
        uint16_t opcode = cpu.fetch();
        cpu.execute(opcode);

        // Printing the screen array
        for (int i=0; i<32; ++i) {
            for (int j=0; j<64; ++j) {
                move(i,j);
                if (gfx[i][j]) {
                    addch('X');
                }
                else {
                    addch(' ');
                }
            }
        }

        // Arbritrary timer factor (change this if too fast/slow)
        // Cycles are also controlled by ncurses::timeout()
        if ((clock%20) == 0) {
            cpu.delay_timer = max(cpu.delay_timer-1, 0);
            cpu.sound_timer = max(cpu.sound_timer-1, 0);
        }

        refresh();
    }

    getch();
    endwin();
    return 0;

}

