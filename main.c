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

int main(){
    char* buffer = (char*)malloc(INPUT_SIZE);
    while(!get_input_notes(buffer, INPUT_SIZE)){

    }
    free(buffer);
    return 0;


    char* notes = (char*)malloc(100);
    double* freqs = (double*)malloc(100*sizeof(double));
    strcpy(notes, "CD FfgDECDECD FfgDECD Ffg\0");
    unsigned int notes_len = strlen(notes);
    
    double duration = 0.5;
    unsigned int samplerate = 44100;
    unsigned int samples_by_note = (unsigned int)(duration*samplerate);
    double* time_v = (double*)malloc(samples_by_note*sizeof(double));
    double* waves = (double*)malloc((notes_len+1)*samples_by_note*sizeof(double));

    get_time_vector(time_v, duration);
    

    math2music(freqs, notes , notes_len);

    get_waves(waves, freqs, time_v, notes_len, duration);

    wave_header_t* header = make_wave_header(notes_len*samples_by_note, 1, samplerate, 8);
    write_wav("my.wav", waves, header, notes_len, samples_by_note);
    
    // no longer needed
    free(header);
    free(notes);
    free(freqs);
    free(time_v);
    free(waves);
    
    char* args[1];
    double* filter;
    args[0] = (char*)malloc(20);
    strcpy(args[0], "my.wav\0");

    init_interpreter();
    PyObject* module_play_wav;
    if((module_play_wav = init_module("filter_n_play")) != NULL)
        call_python_function(module_play_wav, "play_wav", args, 1);

    strcpy(args[0], "293.6\0");
    if(module_play_wav){
        filter = (double*)call_python_function(module_play_wav, "make_pass_band_filter", args, 1);
            for(int i = 0; i < 10; ++i)
                printf("[Main] Result: %.6e\n",filter[i]);
        end_module(module_play_wav);
    }
    end_interpreter();

    free(filter);
    free(args[0]);
    return 0;
}
