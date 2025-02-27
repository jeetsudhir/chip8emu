//
// Created by Abhijeet Sudhir on 8/11/24.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <ctime>
#include <thread>
#include <cmath>

#include "chip8core.h"


// Constructor and Destructor
Chip8::Chip8() = default;
Chip8::~Chip8() = default;

// Initialize CHIP-8 system
void Chip8::init() {
    pc = 0x200;    // Set program counter to 0x200
    opcode = 0;    // Reset current opcode
    I = 0;         // Reset index register
    sp = 0;        // Reset stack pointer

    // Clear display, stack, keypad, and registers
    std::fill(std::begin(gfx), std::end(gfx), false);
    std::fill(std::begin(stack), std::end(stack), 0);
    std::fill(std::begin(key), std::end(key), false);
    std::fill(std::begin(V), std::end(V), 0);
    std::fill(std::begin(memory), std::end(memory), 0);

    // Load fontset into memory
    std::copy(std::begin(chip8_fontset), std::end(chip8_fontset), memory);

    // Reset timers
    delay_timer = 0;
    sound_timer = 0;

    // Seed random number generator
    srand(static_cast<unsigned int>(time(nullptr)));
}

// Load ROM into memory
bool Chip8::load(const char *file_path) {
    init();  // Initialize before loading ROM

    printf("Loading ROM: %s\n", file_path);

    FILE* rom = fopen(file_path, "rb");
    if (!rom) {
        std::cerr << "Failed to open ROM" << std::endl;
        return false;
    }

    // Get file size
    fseek(rom, 0, SEEK_END);
    long rom_size = ftell(rom);
    rewind(rom);

    if (rom_size <= 0 || rom_size > 3584) {  // 4096 - 512 = 3584
        std::cerr << "ROM size is invalid or too large to fit in memory" << std::endl;
        fclose(rom);
        return false;
    }

    // Allocate memory and read ROM
    std::vector<char> rom_buffer(rom_size);
    if (fread(rom_buffer.data(), sizeof(char), rom_size, rom) != rom_size) {
        std::cerr << "Failed to read ROM" << std::endl;
        fclose(rom);
        return false;
    }

    // Copy buffer to memory starting at 0x200
    std::copy(rom_buffer.begin(), rom_buffer.end(), memory + 512);

    fclose(rom);
    return true;
}

// Emulate one cycle of the CHIP-8 CPU
void Chip8::emulateCycle() {
    // Fetch opcode
    opcode = (memory[pc] << 8) | memory[pc + 1];

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000:  // 00E0: Clear the display
                    std::fill(std::begin(gfx), std::end(gfx), false);
                    drawFlag = true;
                    pc += 2;
                    break;
                case 0x000E:  // 00EE: Return from subroutine
                    pc = stack[--sp] + 2;
                    break;
                default:
                    unimplemented_opcode();
            }
            break;

        case 0x1000:  // 1NNN: Jump to address NNN
            pc = opcode & 0x0FFF;
            break;

        case 0x2000:  // 2NNN: Call subroutine at NNN
            stack[sp++] = pc;
            pc = opcode & 0x0FFF;
            break;

        case 0x3000:  // 3XNN: Skip next instruction if VX == NN
            pc += (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) ? 4 : 2;
            break;

        case 0x4000:  // 4XNN: Skip next instruction if VX != NN
            pc += (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) ? 4 : 2;
            break;

        case 0x5000:  // 5XY0: Skip next instruction if VX == VY
            pc += (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
            break;

        case 0x6000:  // 6XNN: Set VX to NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;

        case 0x7000:  // 7XNN: Add NN to VX
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;

        case 0x8000:
            handle_8xxx();
            break;

        case 0x9000:  // 9XY0: Skip next instruction if VX != VY
            pc += (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) ? 4 : 2;
            break;

        case 0xA000:  // ANNN: Set I to address NNN
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        case 0xB000:  // BNNN: Jump to address NNN + V0
            pc = (opcode & 0x0FFF) + V[0];
            break;

        case 0xC000:  // CXNN: Set VX to random number masked by NN
            V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            break;

        case 0xD000:  // DXYN: Draw sprite
            draw_sprite();
            pc += 2;
            break;

        case 0xE000:
            handle_exxx();
            break;

        case 0xF000:
            handle_fxxx();
            break;

        default:
            unimplemented_opcode();
    }

    update_timers();
}

// Handle 8XY_ opcodes
void Chip8::handle_8xxx() {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0x000F) {
        case 0x0: V[x] = V[y]; break;
        case 0x1: V[x] |= V[y]; break;
        case 0x2: V[x] &= V[y]; break;
        case 0x3: V[x] ^= V[y]; break;
        case 0x4:
            V[0xF] = (V[y] > (0xFF - V[x])) ? 1 : 0;
            V[x] += V[y];
            break;
        case 0x5:
            V[0xF] = (V[x] >= V[y]) ? 1 : 0;
            V[x] -= V[y];
            break;
        case 0x6:
            V[0xF] = V[x] & 0x1;
            V[x] >>= 1;
            break;
        case 0x7:
            V[0xF] = (V[y] >= V[x]) ? 1 : 0;
            V[x] = V[y] - V[x];
            break;
        case 0xE:
            V[0xF] = (V[x] & 0x80) >> 7;
            V[x] <<= 1;
            break;
        default: unimplemented_opcode();
    }
    pc += 2;
}

// Handle EX__ opcodes
void Chip8::handle_exxx() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    switch (opcode & 0x00FF) {
        case 0x009E:
            pc += key[V[x]] ? 4 : 2;
            break;
        case 0x00A1:
            pc += !key[V[x]] ? 4 : 2;
            break;
        default:
            unimplemented_opcode();
    }
}

// Handle FX__ opcodes
void Chip8::handle_fxxx() {
    uint8_t x = (opcode & 0x0F00) >> 8;

    switch (opcode & 0x00FF) {
        case 0x07: V[x] = delay_timer; break;
        case 0x0A: wait_for_keypress(x); return;
        case 0x15: delay_timer = V[x]; break;
        case 0x18: sound_timer = V[x]; break;
        case 0x1E: I += V[x]; break;
        case 0x29: I = V[x] * 5; break;
        case 0x33: store_bcd(V[x]); break;
        case 0x55: std::copy(V, V + x + 1, memory + I); break;
        case 0x65: std::copy(memory + I, memory + I + x + 1, V); break;
        default: unimplemented_opcode();
    }
    pc += 2;
}

// Unimplemented opcode handling
void Chip8::unimplemented_opcode() const {
    std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::endl;
    exit(1);
}

// Update delay and sound timers
void Chip8::update_timers() {
    if (delay_timer > 0) --delay_timer;
}

// Wait for a keypress (blocking)
void Chip8::wait_for_keypress(uint8_t x) {
    bool key_pressed = false;
    while (!key_pressed) {
        for (int i = 0; i < 16; ++i) {
            if (key[i]) {
                V[x] = i;
                key_pressed = true;
                break;
            }
        }
    }
}

// Store BCD representation of Vx in memory locations I, I+1, I+2
void Chip8::store_bcd(uint8_t value) {
    memory[I]     = value / 100;
    memory[I + 1] = (value / 10) % 10;
    memory[I + 2] = value % 10;
}

// Draw sprite on the screen
void Chip8::draw_sprite() {
    uint8_t x = V[(opcode & 0x0F00) >> 8];
    uint8_t y = V[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;
    V[0xF] = 0;

    for (int yline = 0; yline < height; ++yline) {
        uint8_t pixel = memory[I + yline];
        for (int xline = 0; xline < 8; ++xline) {
            if ((pixel & (0x80 >> xline)) != 0) {
                if (gfx[(x + xline + ((y + yline) * 64))]) {
                    V[0xF] = 1;
                }
                gfx[x + xline + ((y + yline) * 64)] ^= true;
            }
        }
    }
    drawFlag = true;
}

