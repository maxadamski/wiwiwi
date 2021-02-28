#!/usr/bin/env python
# pip install PySoundFile
import soundfile
import numpy as np
import sys, os
from scipy.fftpack import fft, fftfreq
from scipy.signal import decimate

def read_wav(path):
    wav, rate = soundfile.read(path, always_2d=True)
    # convert to mono
    if wav.shape[1] == 2:
        wav = (wav[:,0] + wav[:,1]) / 2
    else:
        wav = wav[:,0]
    return wav, rate

def kaiser(signal, beta=14):
    return signal * np.kaiser(len(signal), beta)

def hps(fft_data, repeat=4):
    if repeat < 2: repeat = 2
    fft_copy = fft_data.copy()
    for k in range(2, repeat+1):
        d = decimate(fft_data, k)
        fft_copy[:len(d)] *= d
    fft_copy[:10] = 0
    return fft_copy
    
def preprocess(wav):
    fy = wav
    fy = kaiser(fy, beta=50)
    fy = abs(fft(fy))
    fy = hps(fy, repeat=4)
    return fy

def predict(fx, fy):
    # fy must be a clean fft result
    m0, m1 = 85, 175  # typically [85,180]
    f0, f1 = 175, 355 # typically [165,255]
    mask = (m0 <= fx) & (fx <= f1)
    freq = fx[mask][np.argmax(fy[mask])]
    if m0 <= freq < m1: return 'M', freq
    if f0 <= freq < f1: return 'K', freq
    return 'M', freq
    
def predict_file(path):
    data, rate = read_wav(path)
    fx = fftfreq(len(data), d=1/rate)
    fy = preprocess(data)
    pred, freq = predict(fx, fy)
    return pred, freq

if __name__ == '__main__':
    try:
        path = sys.argv[1]
        label, _ = predict_file(path)
        print(label)
    except:
        print('M')
