#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "filewave.h"

extern void math2music(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);
extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);

int main(){
    char* notes = (char*)malloc(100);
    double* freqs = (double*)malloc(100*sizeof(double));
    strcpy(notes, "CD FfgDECDECD FfgDECD Ffg\0");
    unsigned int notes_len = strlen(notes);
    printf("Len = %d\n",notes_len);

    double duration = 0.5;
    unsigned int samplerate = 44100;
    unsigned int samples = (unsigned int)(duration*samplerate);
    double* time_v = (double*)malloc(samples*sizeof(double));
    double* waves = (double*)malloc((notes_len+1)*samples*sizeof(double));

    get_time_vector(time_v, duration);

    for(int i = 0; i < 20; ++i)
        printf("%d = %.6e\n", i, time_v[i]);
    printf("%d = %.6e\n", (int)(duration*samplerate)-1, time_v[(int)(duration*samplerate)-1]);
    

    math2music(freqs, notes , notes_len);

    get_waves(waves, freqs, time_v, notes_len, duration);
    for(int i = 0; i < 20; ++i)
        printf("%d = %.6e\n", i, waves[i]);

    wave_header_t* header = make_wave_header(notes_len*samples, 1, samplerate, 8);

    FILE* file;
    if((file = fopen("my.wav", "wb"))){
        fwrite(header, sizeof(wave_header_t), 1, file);
        fwrite(waves, sizeof(double), samples*notes_len, file);
        fclose(file);
    }
    else
        printf("Couldn't create file\n");

    free(header);

    free(notes);
    free(freqs);
    free(time_v);
    free(waves);

    return 0;
}
