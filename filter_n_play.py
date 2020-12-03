import scipy.signal as signal
from playsound import playsound

samplerate = 44100

def make_pass_band_filter(freq = 293.66974569918125):
    f1 = freq - 10.0        # frequency gap to filter
    f2 = freq + 10.0
    h = signal.firwin(2001, [f1, f2], fs = 44100.0, pass_zero = False)  # creates a pass band filter

    h = h.tolist(); # converts from numpy to list
    return h;

def play_wav(name):
    playsound(name) #plays a song by file name
