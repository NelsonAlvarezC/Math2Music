#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "filewave.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern void math2music(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);
extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);

int get_python_filter(){
    Py_Initialize();
    PyRun_SimpleString ("import sys; sys.path.insert(0, '/home/kali/Documents/Assembly/Tarea3')");
    PyObject* pName = PyUnicode_DecodeFSDefault((char*)"test");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if(pModule != NULL){
        PyObject* pFunc = PyObject_GetAttrString(pModule, (char*)"make_pass_band_filter");

        if(pFunc && PyCallable_Check(pFunc)){
            PyObject* pArgs = PyTuple_New(1);
            PyObject* pValue = PyFloat_FromDouble(290.6);
            if(!pValue){
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                fprintf(stderr, "Cannot do conversion from Double to PyFloat\n");
                return 1;
            }
            PyTuple_SetItem(pArgs, 0, pValue);
        
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if(PyList_Check(pValue)){
                PyObject *pTemp;
                double temp[(int)PyList_Size(pValue)];
                printf("Size: %d\n", (int)PyList_Size(pValue));
                for(int i = 0; i < (int)PyList_Size(pValue);++i){
                    pTemp = PyList_GetItem(pValue, i);
                    temp[i] = PyFloat_AS_DOUBLE(pTemp);
                }
                Py_DECREF(pTemp);
                printf("Result: %.6e\n",temp[0]);
            }
            Py_DECREF(pValue);
        }
        else{
            if(PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", "make_pass_band_filter");
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else{
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n","test");
        return 1;
    }
    Py_FinalizeEx();
    return 0;
}



int main(){
    get_python_filter();
    return 0;
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
