import numpy as np
from scipy.io import wavfile
import matplotlib.pyplot as plt
import os

FS = 44100
N = int(FS)
freq = 100.0
x = np.sin(2 * np.pi * np.arange(N) * freq / FS)
wavfile.write('audio/sine_441.wav', FS, x)

def plot_fft(x, fs):
    freqs = np.fft.rfftfreq(len(x), 1.0 / fs)
    X = np.fft.rfft(x) / len(x)
    plt.semilogx(freqs, 20 * np.log10(np.abs(X)))

# plot_fft(x, FS)

# os.system('./build/src_sndfile audio/sine_441.wav audio/sine_96_SRC.wav 96000 0')
# os.system('./build/src_sndfile audio/sine_441.wav audio/sine_96_HPR.wav 96000 1')

fs, x1 = wavfile.read('audio/sine_96_SRC.wav')
plot_fft(x1, fs)

fs, x2 = wavfile.read('audio/sine_96_HPR.wav')
plot_fft(x2, fs)

plt.show()
