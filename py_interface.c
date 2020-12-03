#include "py_interface.h"


void init_interpreter(){
    if(!Py_IsInitialized()){
        char* cwd = (char*)malloc(100);
        if(getcwd(cwd, 100) == NULL){
            fprintf(stderr, "Could not find current working directory\n");
            free(cwd);
            return;
        }
        
        Py_Initialize();

        // Needed to add path of correct directory
        char* sys_path_command = (char*)malloc(150);
        strcpy(sys_path_command, "import sys; sys.path.insert(0, \'");
        strcat(sys_path_command,cwd);
        strcat(sys_path_command, "\')\0");
    
        PyRun_SimpleString (sys_path_command);
    
        free(cwd);
        free(sys_path_command);
    }
}
void end_interpreter(){
    if(Py_IsInitialized())
        Py_FinalizeEx();
    else
        fprintf(stderr, "[End_interpreter] Interpreter not initialized\n");
}

PyObject* init_module(char* name){
    PyObject* pName = PyUnicode_DecodeFSDefault(name);
    PyObject* pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if(!pModule)
        fprintf(stderr,"[Init_module] Module could not be loaded\n");
    return pModule;
}

void end_module(PyObject* pModule){
    if(PyModule_Check(pModule))
        Py_DECREF(pModule);
    else
        fprintf(stderr,"[Clear_module] Not a module\n");
}

// This code does not support multiple return values as Python does
void* call_python_function(PyObject* pModule,char* func_name,char* params[], int params_count, unsigned int* array_size){
    void* return_value = NULL;

    if(pModule != NULL){
        // gets the python function to call
        PyObject* pFunc = PyObject_GetAttrString(pModule, func_name);

        if(pFunc && PyCallable_Check(pFunc)){
            PyObject* pArgs = PyTuple_New(params_count);    // creates tuple for parameters
            PyObject* pValue;                               // used to convert values
            char *ptr;                                      // needed for strtod
            for(int i = 0; i < params_count; ++i){
                if(strtod(params[i], &ptr))
                    pValue = PyFloat_FromDouble(strtod(params[i], &ptr));   // It's double
                else
                    pValue = PyUnicode_DecodeFSDefault(params[i]);          // Use it as a string

                if(!pValue){
                    Py_DECREF(pArgs);                                       // Free memory
                    Py_DECREF(pModule);                                     //   "    "
                    fprintf(stderr, "[Call_python_function] Parameters conversion error\n");
                    return NULL;
                }
                PyTuple_SetItem(pArgs, i, pValue);
            }
        
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if(PyList_Check(pValue)){
                PyObject *pTemp;
                double* temp = (double*)malloc(sizeof(double)*(int)(PyList_Size(pValue)));
                (*array_size) = (unsigned int)PyList_Size(pValue);
                for(int i = 0; i < (int)PyList_Size(pValue);++i){
                    pTemp = PyList_GetItem(pValue, i);
                    temp[i] = PyFloat_AS_DOUBLE(pTemp);
                }
                Py_DECREF(pTemp);
                return_value = (void*)temp;
            }
            Py_DECREF(pValue);
        }
        else{
            if(PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "[Call_python_function] Cannot find function \"%s\"\n", func_name);
        }
        Py_XDECREF(pFunc);
    }
    else{
        PyErr_Print();
        fprintf(stderr, "[Call_python_function] Module failed\n");
    }
    return return_value;
}
