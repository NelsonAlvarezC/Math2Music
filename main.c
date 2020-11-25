#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <unistd.h>
#include "filewave.h"

#define PY_SSIZE_T_CLEAN
#include <Python.h>

extern void math2music(double* dst, char* src, int size);
extern void get_time_vector(double* dst, double duration);
extern void get_waves(double* dst, double* freqs, double* time_v, int size, double duration);

void* call_python_function(char* func_name, char* params[], int params_count){

    void* return_value = NULL;
    char* cwd = (char*)malloc(100);
    if(getcwd(cwd, 100) == NULL){
        fprintf(stderr, "Could not find current working directory\n");
        free(cwd);
        return NULL;
    }

    char* sys_path_command = (char*)malloc(150);
    strcpy(sys_path_command, "import sys; sys.path.insert(0, \'");
    strcat(sys_path_command,cwd);
    strcat(sys_path_command, "\')\0");

    Py_Initialize();
    PyRun_SimpleString (sys_path_command);

    //Py_Initialize();
    //PyRun_SimpleString ("import sys; sys.path.insert(0, '/home/kali/Documents/Assembly/Tarea3')");

    free(cwd);
    free(sys_path_command);

    PyObject* pName = PyUnicode_DecodeFSDefault((char*)"test");
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if(pModule != NULL){
        PyObject* pFunc = PyObject_GetAttrString(pModule, func_name);

        if(pFunc && PyCallable_Check(pFunc)){
            PyObject* pArgs = PyTuple_New(params_count);    // creates tuple for parameters
            PyObject* pValue;                               // used to convert values
            char *ptr;
            for(int i = 0; i < params_count; ++i){
                if(strtod(params[i], &ptr))
                    pValue = PyFloat_FromDouble(strtod(params[i], &ptr));
                else
                    pValue = PyUnicode_DecodeFSDefault(params[i]);

                if(!pValue){
                    Py_DECREF(pArgs);
                    Py_DECREF(pModule);
                    fprintf(stderr, "Parameters conversion error\n");
                    return NULL;
                }
                PyTuple_SetItem(pArgs, i, pValue);
            }
        
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
            fprintf(stderr, "Cannot find function \"%s\"\n", func_name);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else{
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n","test");
        return NULL;
    }
    Py_FinalizeEx();
    return return_value;
}



int main(){
    char* args[1];
    args[0] = (char*)malloc(20);
    strcpy(args[0], "my.wav\0");

    call_python_function("play_wav", args, 1); 

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
