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

void create_song(char* notes, char* output_name){
    double* freqs = (double*)malloc(INPUT_SIZE*sizeof(double));
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
    write_wav(output_name, waves, header, notes_len, samples_by_note);
   
    free(header);
    free(freqs);
    free(time_v);
    free(waves);
}

int main(){
    char* buffer = (char*)malloc(INPUT_SIZE);
    
    char* name;
    init_interpreter();
    PyObject* module_wav = init_module("filter_n_play");
    char* args[1];
    args[0] = (char*)malloc(50);

    while(!get_input_notes(buffer, INPUT_SIZE)){
        name = get_output_name();
        create_song(buffer, name);
        if(ask_play_song()){
            if(module_wav){
                strcpy(args[0], name);
                call_python_function(module_wav, "play_wav", args, 1);
            }
            else
                fprintf(stderr, "[Play_song] Could not load Module\n");
        }
        free(name);
    }
    if(module_wav)
        end_module(module_wav);

    end_interpreter();
    free(buffer);
    free(args[0]);

    return 0;
}
        //filter = (double*)call_python_function(module_play_wav, "make_pass_band_filter", args, 1);
            //for(int i = 0; i < 10; ++i)
                //printf("[Main] Result: %.6e\n",filter[i]);
