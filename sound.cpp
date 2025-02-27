//
// Created by Abhijeet Sudhir on 8/12/24.
//

#include "sound.h"
#include <iostream>
#include <cmath>

const int BEEP_FREQUENCY = 440; // Frequency in Hz
const int BEEP_DURATION = 500;  // Duration in milliseconds

Sound::Sound() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_zero(wav_spec);
    wav_spec.freq = 44100;
    wav_spec.format = AUDIO_F32SYS;
    wav_spec.channels = 1;
    wav_spec.samples = 4096;
    wav_spec.callback = audio_callback;
    wav_spec.userdata = this;

    device_id = SDL_OpenAudioDevice(nullptr, 0, &wav_spec, nullptr, 0);
    if (device_id == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

Sound::~Sound() {
    SDL_CloseAudioDevice(device_id);
    SDL_Quit();
}

void Sound::play_beep() {
    beep_frequency = BEEP_FREQUENCY;
    beep_duration = BEEP_DURATION * (wav_spec.freq / 1000); // Convert duration to samples
    beep_position = 0;

    SDL_PauseAudioDevice(device_id, 0);
}

void Sound::audio_callback(void* userdata, Uint8* stream, int len) {
    Sound* sound = static_cast<Sound*>(userdata);
    float* fstream = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);

    for (int i = 0; i < samples; ++i) {
        if (sound->beep_position < sound->beep_duration) {
            fstream[i] = 0.5f * std::sin(2.0f * M_PI * sound->beep_frequency * sound->beep_position / sound->wav_spec.freq);
            ++sound->beep_position;
        } else {
            fstream[i] = 0.0f;
        }
    }
}
