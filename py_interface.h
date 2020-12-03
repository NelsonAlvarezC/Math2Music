// Author: Nelson Alvarez
#pragma once

#ifndef PY_SSIZE_T_CLEAN
# define PY_SSIZE_T_CLEAN
#endif

#include <Python.h>
#include <unistd.h>

// Initializes the python interpreter
void init_interpreter();

// Finalizes the python interpreter
void end_interpreter();

// Imports a module and returns it has PyObject*
PyObject* init_module(char*);

// Decrement the counter of a module
void end_module(PyObject*);

// Calls a python function using a pModule in first parameter
void* call_python_function(PyObject*, char*,char* [], int, unsigned int*);
