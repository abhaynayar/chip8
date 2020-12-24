#include <cstdio>
#include <cstdlib>
#include <ncurses.h>

#include "cpu.h"

uint8_t gfx[32][64];
uint8_t key_pressed;

uint8_t CPU::getVF() {
    return V[0xF];
}

void CPU::setVF() {
    V[0xF] = 1;
}

CPU::CPU() {
    I = 0; // Reset address register
    pc = 0x200; // Where the program ROM is loaded
    
    // Reset data registers
    for (int i=0; i<0x10; ++i) {
        V[i] = 0;
    }

    // Clear the screen
    for (int i=0; i<32; ++i) {
        for (int j=0; j<64; ++j) {
            gfx[i][j] = 0;
        }
    }
}

void CPU::halt() {
    puts("Unknown opcode, exiting.");
    endwin();
    exit(1337);
}

void CPU::dump() {
    printf("Program counter: %x\n", pc);
    puts("****** DUMPING CPU CONTENTS ******");
    printf("Address register: %x\n", I);
    printf("Data registers: ");
    for (int i=0; i<0x10; ++i) {
        printf("%x ", V[i]);
    }
    
    puts(""); // Newline
    mem.stack_dump();
    getchar(); // Wait for user to see CPU dump
}

uint16_t CPU::fetch() {
    return (uint16_t) mem.load(pc+1) | ((uint16_t) mem.load(pc) << 8);
}

void CPU::execute(uint16_t opcode) {
    uint16_t upper = (opcode & 0xF000) >> 12;
    uint16_t lower = opcode & 0x0FFF;

    switch(upper) {

        case 0x0: {
            if (lower == 0xe0) {
                for (int i=0; i<32; ++i) {
                    for (int j=0; j<64; ++j) {
                        gfx[i][j] = 0;
                    }
                }
            } else if (lower == 0xee) {
                // Return from a subroutine
                pc = mem.pop();
            } else {
                // Not necessary for most ROMs
                halt();
            }
            break;
        }

        case 0x1: {
            // 1NNN: jump to NNN
            pc = lower-2;
            break;
        }

        case 0x2: {
            // 2NNN: calls subroutine at NNN
            mem.push(pc);
            pc = lower-2;
            break;
        }

        case 0x3: {
            int X = (lower & 0xF00) >> 8;
            int NN = (lower & 0xFF);
            if (V[X] == NN) {
                pc += 2;
            }
            break;
        }

        case 0x4: {
            int X = (lower & 0xF00) >> 8;
            int NN = (lower & 0xFF);
            if (V[X] != NN) {
                pc += 2;
            }
            break;
        }

        case 0x5: {
            int X = (lower & 0xF00) >> 8;
            int Y = (lower & 0xF0) >> 4;
            if (V[X] == V[Y]) {
                pc += 2;
            }
            break;
        }

        case 0x6: {
            int X = (lower & 0xF00) >> 8;
            int NN = lower & 0xFF;
            V[X] = NN;
            break;
        }

        case 0x7: {
            int X = (lower & 0xF00) >> 8;
            int NN = lower & 0xFF;
            V[X] = (V[X] + NN) & 0xFF;
            break;
        }

        case 0x8: {
            int X = (lower & 0xF00) >> 8;
            int Y = (lower & 0xF0) >> 4;
            switch (lower & 0xF) {
                case 0x0: V[X] = V[Y]; break;
                case 0x1: V[X] = V[X] | V[Y]; break;
                case 0x2: V[X] = V[X] & V[Y]; break;
                case 0x3: V[X] = V[X] ^ V[Y]; break;
                case 0x4: {
                    uint16_t sum = V[X] + V[Y];
                    V[X] = sum & 0xFF;
                    
                    // Carry flag
                    if (sum > 0xFF) { V[0xF] = 1; }
                    else { V[0xF] = 0; }
                    break;
                }
                case 0x5: {
                    uint16_t sum = V[X] - V[Y];
                    V[X] = sum & 0xFF;

                    // Borrow flag
                    if (sum<0) { V[0xF] = 0; }
                    else { V[0xF] = 1; }
                    break;
                }
                case 0x6: {
                    V[0xF] = V[X] & 1;
                    V[X] >>= 1;
                    break;
                }
                case 0x7: {
                    uint16_t sum = V[Y] - V[X];
                    V[X] = sum & 0xFF;

                    // Borrow flag
                    if (sum<0) { V[0xF] = 0; }
                    else { V[0xF] = 1; }
                    break;
                }
                case 0xE: {
                    V[0xF] = (V[X] >> 7) & 1;
                    V[X] <<= 1;
                    break;
                }
                default: halt();
            }
            
            break;
        }

        case 0x9: {
            int X = (lower & 0xF00) >> 8;
            int Y = (lower & 0xF0) >> 4;
            if (V[X] != V[Y]) {
                pc += 2;
            }
            break;
        }

        case 0xA: {
            I = lower;
            break;
        }

        case 0xB: {
            pc = V[0] + lower - 2;
            break;
        }

        case 0xC: {
            int X = (lower & 0xF00) >> 8;
            int NN = (lower & 0xFF);
            V[X] = (rand() % 0xFF) & NN;
            break;
        }

        case 0xD: {
            int X = (lower & 0xF00) >> 8;
            int Y = (lower & 0xF0) >> 4;
            int N = lower & 0xF;

            bool collision_flag = false;
            for (int j=0; j<N; ++j) {
                uint8_t current_byte = mem.load(I+j);
                for (int i=0; i<8; ++i) {
                    
                    uint8_t current_bit = (current_byte >> (7-i)) & 1;
                    if (current_bit == 1 && // If we're trying to set...
                            current_bit == gfx[V[Y]+j][V[X]+i]) {
                        collision_flag = true;
                    }

                    gfx[V[Y]+j][V[X]+i] ^= current_bit; // ... but can't
                }
            }

            // check for collision - pixel gets unset
            V[0xF] = collision_flag?1:0;
            break;
        }

        case 0xE: {
            printw("\nInside CPU: %d    ", key_pressed);
            int X = (lower & 0xF00) >> 8;
            int lowest = lower & 0xFF;
            if (lowest == 0x9E) {
                if (key_pressed == V[X]) {
                    pc += 2;
                }
            } else if (lowest == 0xA1) {
                if (key_pressed != V[X]) {
                    pc += 2;
                }
            } else {
                halt();
            }

            break;
        }

        case 0xF: {

            int X = (lower & 0xF00) >> 8;
            switch(lower & 0xFF) {

                case 0x07: {
                    V[X] = delay_timer;
                    break;
                }

                case 0x0A: {
                    for (;;) {
                        // getchar() is blocking
                        uint8_t temp = getchar();
                        if (temp != 0) {
                            V[X] = temp;
                            break;
                        }
                    }
                    break;
                }

                case 0x15: {
                    delay_timer = V[X];
                    break;
                }

                case 0x18: {
                    sound_timer = V[X];
                    break;
                }

                case 0x1E: {
                    I += V[X];
                    break;
                }

                case 0x29: {
                    // Get the digit sprite for the number in V[X]
                    I = V[X]*5; // Every digit sprite takes 5 bytes
                    break;
                }

                case 0x33: {
                    // FIXME: Test if this actually works
                    // Store three-digit BCD in I, I+1, I+2
                    mem.store(I, (V[X]/100)%10);
                    mem.store(I+1, (V[X]/10)%10);
                    mem.store(I+2, V[X]%10);
                    break;
                }

                case 0x55: {
                    for (int i=0; i<0x10; ++i) {
                        mem.store(I+i, V[i]);
                    }
                    break;
                }

                case 0x65: {
                    for (int i=0; i<0x10; ++i) {
                        V[i] = mem.load(I+i);
                    }
                    break;
                }

                default: {
                    halt();
                }
            }
            break;
        }

        default: {
            halt();
        }
    }

    // Incrementing program counter
    pc += 2;
}

