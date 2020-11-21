#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

extern void math2music(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);
extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);

int main(){
    char* notes = (char*)malloc(100);
    double* freqs = (double*)malloc(100*sizeof(double));
    strcpy(notes, "CD FfgDECDECD FfgDECD Ffg\0");
    int notes_len = strlen(notes);
    printf("Len = %d\n",notes_len);

    double duration = 0.5;
    int samplerate = 44100;
    int time_size = (int)(duration*samplerate);
    double* time_v = (double*)malloc(time_size*sizeof(double));
    double* waves = (double*)malloc((notes_len+1)*time_size*sizeof(double));

    get_time_vector(time_v, duration);

    /*for(int i = 0; i < 20; ++i)
        printf("%d = %.6e\n", i, time_v[i]);
    printf("%d = %.6e\n", (int)(duration*samplerate)-1, time_v[(int)(duration*samplerate)-1]);
    */

    math2music(freqs, notes , notes_len);

    get_waves(waves, freqs, time_v, notes_len, duration);
    for(int i = 0; i < 20; ++i)
        printf("%d = %.6e\n", i, waves[i]);

    free(notes);
    free(freqs);
    free(time_v);
    free(waves);

    return 0;
}
