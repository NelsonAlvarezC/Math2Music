// Author: Nelson Alvarez
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "filewave.h"
#include "py_interface.h"
#include "interface.h"

#define INPUT_SIZE 200

extern void notes_2_freq(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);
extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);
extern double* convolve(double* dst, double* filter, double* signal, unsigned int len_dst, unsigned int len_filter, unsigned int len_signal);

/*
 * Plays a wav song using python
 *  name: name of file
 *  pModule: Python module to call function
 *  args[]: vector of arguments to pass in function call
 */
void play_song(char* name, PyObject* pModule, char* args[]){
    if(ask_play_song()){
        if(pModule){
            strcpy(args[0], name);
            call_python_function(pModule, "play_wav", args, 1, NULL);
        }
        else
            fprintf(stderr, "[Play_song] Could not load Module\n");
    }
}

/*
 * Saves a format 3 (64-bit floating point) wav file based on an array of notes
 *  notes: array of notes to create song
 *  output_name: output wav file name
 *  *len_waves: use to set len of computed signal
 *  
 *  returns the signal
 */
double* create_song(char* notes, char* output_name, unsigned int* len_waves){
    double* freqs = (double*)malloc(INPUT_SIZE*sizeof(double));
    unsigned int notes_len = strlen(notes);
    
    double duration = 0.5;
    unsigned int samplerate = 44100;
    unsigned int samples_by_note = (unsigned int)(duration*samplerate);
    double* time_v = (double*)malloc(samples_by_note*sizeof(double));
    double* waves = (double*)malloc((notes_len+1)*samples_by_note*sizeof(double));
    (*len_waves) = (notes_len)*samples_by_note;

    get_time_vector(time_v, duration);
    
    notes_2_freq(freqs, notes , notes_len);

    get_waves(waves, freqs, time_v, notes_len, duration);

    wave_header_t* header = make_wave_header(notes_len*samples_by_note, 1, samplerate, 8);
    write_wav(output_name, waves, header, notes_len, samples_by_note);
   
    free(header);
    free(freqs);
    free(time_v);
    return waves;
}

int main()
{
    char* name;                                                 // file name to save song
    char* buffer = (char*)malloc(INPUT_SIZE);                   // buffer for notes
    char* filter_note = (char*)malloc(4*sizeof(char));          // note to use as filter, requires to be a vector
    double* filter_freq = (double*)malloc(16*sizeof(double));   // frequency of filter note

    unsigned int len_waves, len_filtered, len_filter;
    
    double* waves = NULL;                                       // used to save signal of create_song
    
    init_interpreter();                                         // inits the python interpreter
    PyObject* module_wav = init_module("filter_n_play");        // inits python module

    char* args[1];                                              // arguments for python function calls
    args[0] = (char*)malloc(50);

    while(!get_input_notes(buffer, INPUT_SIZE))
    {
        name = get_output_name();                               // song filename
        waves = create_song(buffer, name, &len_waves);

        if(waves)
        {
            play_song(name, module_wav, args);                  // asks to play song usign python
            filter_note[0] = ask_filter_song();

            if(filter_note[0] != 0)
            {
                name = get_output_name();                       // filtered song filename
                notes_2_freq(filter_freq, filter_note, 1);      // frequency of filter note

                printf("[Filter_note] Doing filter with freq: %lf\n", filter_freq[0]);
                sprintf(args[0], "%lf", filter_freq[0]);        // Convertion of double to char* because python function call

                // make_pass_band_filter returns a finite impulse response
                // to use as filter using a convolution with the waves signal
                double* filter = call_python_function(module_wav, "make_pass_band_filter", args, 1, &len_filter);

                len_filtered = len_filter + len_waves - 1;                          // calculates length of convolution output
                double* filtered = (double*)calloc(len_filtered, sizeof(double));   // saves memory for convolution output
                convolve(filtered, filter, waves, len_filtered, len_filter,len_waves);

                // Save wav file of filtered signal
                wave_header_t* header = make_wave_header(len_filtered, 1, 44100, 8);
                write_wav(name, filtered, header, 1, len_filtered);
                play_song(name, module_wav, args);

                free(header);
                free(filtered);
                free(filter);
            }

        }
        free(name);
        if(waves)
            free(waves);
    }
    if(module_wav)
        end_module(module_wav);

    end_interpreter();
    free(filter_note);
    free(filter_freq);
    free(buffer);
    free(args[0]);

    return 0;
}
