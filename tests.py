import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
import os

FS = 44100
N = int(FS)
freq = 100.0
x = np.sin(2 * np.pi * np.arange(N) * freq / FS)
wavfile.write('audio/sine_441.wav', FS, x)

def plot_fft(x, fs, *args, **kwargs):
    freqs = np.fft.rfftfreq(len(x), 1.0 / fs)
    X = np.fft.rfft(x) / len(x)
    plt.semilogx(freqs, 20 * np.log10(np.abs(X)), *args, **kwargs)

# plot_fft(x, FS)

target_fs = 96 # kHz

os.system(f'./build/src_sndfile audio/sine_441.wav audio/sine_{target_fs}_SRC.wav {1000 * target_fs} 0')
os.system(f'./build/src_sndfile audio/sine_441.wav audio/sine_{target_fs}_HPR.wav {1000 * target_fs} 1')
os.system(f'./build/src_sndfile audio/sine_441.wav audio/sine_{target_fs}_LCZ.wav {1000 * target_fs} 2')

fs, x1 = wavfile.read('audio/sine_96_SRC.wav')
plot_fft(x1, fs, label='SRC')

# fs, x2 = wavfile.read('audio/sine_96_HPR.wav')
# plot_fft(x2, fs, '--', label='HP')

fs, x3 = wavfile.read('audio/sine_96_LCZ.wav')
plot_fft(x3, fs, '--', label='Lanczos')

plt.grid()
plt.ylim(-150)
plt.legend()

plt.title('SRC Comparison')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Amplitude [dB]')

plt.show()
