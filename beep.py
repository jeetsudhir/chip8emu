import numpy as np
import wave
import struct

import numpy as np
import wave
import struct

def generate_beep(filename, duration_ms=500, freq=440, volume=1.0, sample_rate=44100):
    # Calculate the number of samples
    n_samples = int(sample_rate * duration_ms / 1000.0)

    # Create a numpy array for the waveform
    t = np.linspace(0, duration_ms / 1000.0, n_samples, False)
    waveform = volume * np.sin(2 * np.pi * freq * t)

    # Convert waveform to bytes
    waveform_bytes = struct.pack('<' + 'h' * n_samples, *(np.int16(waveform * 32767)))

    # Write to a WAV file
    with wave.open(filename, 'w') as wav_file:
        wav_file.setnchannels(1)  # Mono sound
        wav_file.setsampwidth(2)  # 2 bytes per sample
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(waveform_bytes)

# Generate the beep.wav file with max volume
generate_beep('beep.wav', duration_ms=500, freq=440, volume=1.0)