//
// Created by Abhijeet Sudhir on 8/16/24.
//

#include "screen.h"
#include <iostream>
#include <chrono>
#include <thread>

Screen::Screen(int w, int h, char* path_arg) : width(w), height(h), window(nullptr), renderer(nullptr), sdlTexture(nullptr) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(2);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, width, height);

    sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    path = path_arg;
}

Screen::~Screen() {
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Screen::loadROM(const char* filename) {
    chip8.load(filename);
}

void Screen::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) exit(0);

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) exit(0);

            if (e.key.keysym.sym == SDLK_F1) {
                std::cout << "Reloading ROM" << std::endl;
                loadROM(path);  // Reload ROM, change as needed
            }

            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == keybindings[i]) {
                    chip8.key[i] = 1;
                }
            }
        }

        if (e.type == SDL_KEYUP) {
            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == keybindings[i]) {
                    chip8.key[i] = 0;
                }
            }
        }
    }
}

void Screen::updateScreen() {
    if (chip8.drawFlag) {
        chip8.drawFlag = false;

        for (int i = 0; i < 2048; ++i) {
            uint8_t pixel = chip8.gfx[i];
            pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
        }

        SDL_UpdateTexture(sdlTexture, nullptr, pixels, 64 * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}

void Screen::run() {
    while (true) {
        chip8.emulateCycle();
        handleEvents();
        updateScreen();
        std::this_thread::sleep_for(std::chrono::microseconds(1200));
    }
}
