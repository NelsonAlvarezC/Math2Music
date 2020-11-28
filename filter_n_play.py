import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

from scipy.io.wavfile import write, read
from playsound import playsound

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

def make_pass_band_filter(freq = 293.66974569918125):
    f1 = freq - 10.0
    f2 = freq + 10.0
    h = signal.firwin(2001, [f1, f2], fs = 44100.0, pass_zero = False)

    h = h.tolist();
    return h;

def play_wav(name):
    playsound(name)


#a_wave = get_wave(261.63)



#note_freqs = get_piano_notes()
#music_notes = 'C-C-C-D-D-D-E-E-D-D-C-C'

#data = get_song_data(music_notes)

#h = make_pass_band_filter()
#w1, h1 = signal.freqz(h, fs = 44100.0, worN=4000)

#plt.plot(w1, 20*np.log10(np.abs(h1)), 'b')
#plt.show();

#data_filtered = signal.convolve(data, h, mode="valid")

#write('test.wav',samplerate, data)
#write('test3.wav',samplerate, data_filtered)
