from pprint import pprint
import numpy as np
import struct
from scipy.io.wavfile import write
import scipy.signal as signal
import matplotlib.pyplot as plt

samplerate = 44100

def get_wave(freq, duration=0.5):
    '''
    Function takes the "frequecy" and "time_duration" for a wave 
    as the input and returns a "numpy array" of values at all points 
    in time
    '''
    
    amplitude = 4096
    t = np.linspace(0, duration, int(samplerate * duration))
    wave = amplitude * np.sin(2 * np.pi * freq * t)
    
    return wave

def get_piano_notes():
    '''
    Returns a dict object for all the piano 
    note's frequencies
    '''
    # White keys are in Uppercase and black keys (sharps) are in lowercase
    octave = ['C', 'c', 'D', 'd', 'E', 'F', 'f', 'G', 'g', 'A', 'a', 'B'] 
    base_freq = 261.63 #Frequency of Note C4
    
    note_freqs = {octave[i]: base_freq * pow(2,(i/12)) for i in range(len(octave))}        
    note_freqs[''] = 0.0 # silent note
    
    return note_freqs

def get_song_data(music_notes):
    '''
    Function to concatenate all the waves (notes)
    '''
    note_freqs = get_piano_notes() # Function that we made earlier
    song = [get_wave(note_freqs[note]) for note in music_notes.split('-')]
    song = np.concatenate(song)
    return song 

def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = signal.butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

  # To get the piano note's frequencies
a_wave = get_wave(261.63)

note_freqs = get_piano_notes()
music_notes = 'C-C-C-D-D-D-E-E-D-D-C-C'

data = get_song_data(music_notes)

b, a = signal.iirnotch(290.6, 30.0, 44100.0)
w, h = signal.freqz(b,a, fs = 44100.0)

data2 = signal.filtfilt(b,a,data)
#data2 = signal.convolve(data, w)

write('test.wav',samplerate, data)
write('test2.wav',samplerate, data2)
