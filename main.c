#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "filewave.h"
#include "py_interface.h"
#include "interface.h"

#define INPUT_SIZE 200

extern void math2music(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);
extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);
extern double* convolve(double* dst, double* filter, double* signal, unsigned int len_dst, unsigned int len_filter, unsigned int len_signal);

/*
double* convolve(double* h, double* x, int len_h, int len_x, unsigned int* len_y){
    (*len_y) = len_h + len_x - 1;
    
    int start_x;
    int end_x;

    double *y = (double*)calloc(*len_y, sizeof(double));

    for(int i = 0; i < *len_y; ++i){
        start_x = (i >= len_h - 1)? i - (len_h-1): 0;
        end_x   = (i < len_x)? i: len_x - 1;
        for(int j = start_x; j <= end_x; ++j)
            y[i] += h[i - j]*x[j];
    }

    return y;
}
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
    
    math2music(freqs, notes , notes_len);

    get_waves(waves, freqs, time_v, notes_len, duration);

    wave_header_t* header = make_wave_header(notes_len*samples_by_note, 1, samplerate, 8);
    write_wav(output_name, waves, header, notes_len, samples_by_note);
   
    free(header);
    free(freqs);
    free(time_v);
    return waves;
}

int main(){
    char* buffer = (char*)malloc(INPUT_SIZE);
    
    char* name;
    char* filter_note = (char*)malloc(4*sizeof(char));
    double* filter_freq = (double*)malloc(16*sizeof(double));
    unsigned int len_waves;
    unsigned int len_filtered;
    unsigned int len_filter;
    double* waves = NULL;
    init_interpreter();
    PyObject* module_wav = init_module("filter_n_play");
    char* args[1];
    args[0] = (char*)malloc(50);

    while(!get_input_notes(buffer, INPUT_SIZE)){
        name = get_output_name();
        waves = create_song(buffer, name, &len_waves);
        if(waves){
            play_song(name, module_wav, args);
            filter_note[0] = ask_filter_song();
            if(filter_note[0] != 0){
                name = get_output_name();
                math2music(filter_freq, filter_note, 1);
                printf("Doing filter with freq: %lf\n", filter_freq[0]);
                sprintf(args[0], "%lf", filter_freq[0]);
                double* filter = call_python_function(module_wav, "make_pass_band_filter", args, 1, &len_filter);
                len_filtered = len_filter + len_waves - 1;
                double* filtered = (double*)calloc(len_filtered, sizeof(double));
                printf("Len: %d filtered[%d] = %lf\n", len_filtered, len_filtered-1, filtered[len_filtered-1]);
                convolve(filtered, filter, waves, len_filtered, len_filter,len_waves);
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
