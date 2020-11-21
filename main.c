#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

extern void math2music(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);

int main(){
    char* buffer = (char*)malloc(100);
    double* freqs = (double*)malloc(100*sizeof(double));
    strcpy(buffer, "CD FfgDECD FfgDECD FfgDECD Ffg\0");

    double duration = 0.5;
    int samplerate = 44100;
    double* time_v = (double*)malloc((int)(duration*samplerate)*sizeof(double));

    get_time_vector(time_v, duration);

    for(int i = 0; i < 20; ++i)
        printf("%d = %.6e\n", i, time_v[i]);
    printf("%d = %.6e\n", (int)(duration*samplerate)-1, time_v[(int)(duration*samplerate)-1]);

    /* 
    math2music(freqs, buffer, strlen(buffer));

    for(int i = 0; i < strlen(buffer); ++i)
        printf("%c = %f\n", buffer[i], freqs[i]);
    */
    free(buffer);
    free(freqs);

    return 0;
}
