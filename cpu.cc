#include "cpu.h"
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '#' : ' '), \
  (byte & 0x40 ? '#' : ' '), \
  (byte & 0x20 ? '#' : ' '), \
  (byte & 0x10 ? '#' : ' '), \
  (byte & 0x08 ? '#' : ' '), \
  (byte & 0x04 ? '#' : ' '), \
  (byte & 0x02 ? '#' : ' '), \
  (byte & 0x01 ? '#' : ' ') 

CPU::CPU() {
    I = 0; // Reset address register
    pc = 0x200; // Where the program ROM is loaded
    
    // Reset data registers
    for (int i=0; i<0x10; ++i) {
        V[i] = 0;
    }
}

void CPU::halt() {
    puts("Unknown opcode, exiting.");
    exit(1337);
}

void CPU::dump() {
    puts("****** DUMPING CPU CONTENTS ******");
    printf("Program counter: %x\n", pc);
    printf("Address register: %x\n", I);
    printf("Data registers: ");
    for (int i=0; i<0x10; ++i) {
        printf("%x ", V[i]);
    }
    puts(""); // Newline

    mem.stack_dump();
    getchar(); // Waiting for user input
}

uint16_t CPU::fetch() {
    return (uint16_t) mem.load(pc+1) | ((uint16_t) mem.load(pc) << 8);
}

uint8_t CPU::key() {
    return 0x6; // TODO: Get actual key press
    //return getchar();
}

void CPU::execute(uint16_t opcode) {
    uint16_t upper = (opcode & 0xF000) >> 12;
    uint16_t lower = opcode & 0x0FFF;
    
    switch(upper) {
        
        case 0x0: {
            if (lower == 0xe0) {
                // FIXME: Clear the display in SDL
                puts("Clearing the display");
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
            V[X] += NN;
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
                case 0x4: V[X] += V[Y]; break;
                case 0x5: V[X] -= V[Y]; break;
                case 0x6: V[X] >>= 1; break;
                case 0x7: V[X] = V[Y] - V[X]; break;
                case 0xE: V[X] <<= 1; break;
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
            V[X] = 23 & NN; // FIXME: Implement actual rand() lol
            break;
        }
        
        case 0xD: {
            int X = (lower & 0xF00) >> 8;
            int Y = (lower & 0xF0) >> 4;
            int N = lower & 0xF;

            // FIXME: Dump the sprite in SDL
            puts("Dumping the sprite:");
            printf("X:%x, Y:%x, N:%x\n", X, Y, N);
            for (int j=0; j<N+1; ++j) {
                int x = mem.load(I+j);
                printf(BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(x));
            }

            break;
        }
        
        case 0xE: {
            int X = (lower & 0xF00) >> 8;
            int lowest = lower & 0xFF;
            if (lowest == 0x9E) {
                if (key() == V[X]) {
                    pc += 2;
                }
            } else if (lowest == 0xA1) {
                if (key() != V[X]) {
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
                        uint8_t temp = key();
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
                
                //case 0x29: break;
                //case 0x33: break;
                
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

    // Updating timers: assuming CPU runs at same speed as timers (60hz)
    delay_timer = std::max(delay_timer-1, 0);
    sound_timer = std::max(sound_timer-1, 0);
}

