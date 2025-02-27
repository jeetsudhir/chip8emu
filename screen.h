//
// Created by Abhijeet Sudhir on 8/16/24.
//

#ifndef CHIP8EMULATOR_SCREEN_H
#define CHIP8EMULATOR_SCREEN_H

#include <SDL.h>
#include <cstdint>
#include "chip8core.h"

const uint8_t keybindings[16] = {
        SDLK_x, // 0
        SDLK_1, // 1
        SDLK_2, // 2
        SDLK_3, // 3
        SDLK_q, // 4
        SDLK_w, // 5
        SDLK_e, // 6
        SDLK_a, // 7
        SDLK_s, // 8
        SDLK_d, // 9
        SDLK_z, // A
        SDLK_c, // B
        SDLK_4, // C
        SDLK_r, // D
        SDLK_f, // E
        SDLK_v  // F
};

class Screen {
public:
    Screen(int width, int height, char* path);
    ~Screen();
    void loadROM(const char* filename);
    void run();

private:
    int width;
    int height;
    char* path;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* sdlTexture;
    uint32_t pixels[2048];
    Chip8 chip8;

    void handleEvents();
    void updateScreen();
};

#endif //CHIP8EMULATOR_SCREEN_H
