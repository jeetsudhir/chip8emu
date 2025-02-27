//
// Created by Abhijeet Sudhir on 8/11/24.
//

#ifndef CHIP8EMULATOR_CHIP8CORE_H
#define CHIP8EMULATOR_CHIP8CORE_H

#include <cstdint>
#include <random>
#include <cstdint>
#include "fontset.h"
#include "sound.h"

class Chip8 {
private:
    uint16_t stack[16]{};
    uint16_t sp{};
    uint8_t memory[4096]{};
    uint8_t V[16]{};
    uint16_t pc{};
    uint16_t opcode{};
    uint16_t I{};
    uint8_t delay_timer{};
    uint8_t sound_timer{};

    static SDL_AudioSpec wav_spec;
    static Uint32 wav_length;
    static Uint8 *wav_buffer;
    //static SDL_AudioDeviceID device_id = 0;

    void init();

    void handle_8xxx();

    void handle_exxx();

    void handle_fxxx();

    void unimplemented_opcode() const;

    void update_timers();

    void wait_for_keypress(uint8_t x);

    void store_bcd(uint8_t value);

    void draw_sprite();

    void init_audio();

    void cleanup_audio();

public:
    bool  gfx[64 * 32]{};
    bool  key[16]{};
    bool drawFlag{};

    Chip8();
    ~Chip8();

    void emulateCycle();
    bool load(const char *file_path);

    static void play_sound();
};

#endif //CHIP8EMULATOR_CHIP8CORE_H
