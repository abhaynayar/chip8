#ifndef SCREEN_H
#define SCREEN_H
#include <cstdint>

// Screen dimension constants
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320


class Screen {
    uint8_t gfx[64*32];
public:
    Screen();
    static int drawThread(void *data);
};

#endif
