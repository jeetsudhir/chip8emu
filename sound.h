//
// Created by Abhijeet Sudhir on 8/12/24.
//

#ifndef CHIP8EMULATOR_SOUND_H
#define CHIP8EMULATOR_SOUND_H

#include <iostream>
#include <SDL.h>

class Sound {
public:
    Sound();
    ~Sound();
    void play_beep();

private:
    static void audio_callback(void* userdata, Uint8* stream, int len);
    SDL_AudioSpec wav_spec;
    SDL_AudioDeviceID device_id;
    int beep_frequency;
    int beep_duration;
    int beep_position;
};

#endif //CHIP8EMULATOR_SOUND_H
