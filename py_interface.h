#pragma once

#ifndef PY_SSIZE_T_CLEAN
# define PY_SSIZE_T_CLEAN
#endif

#include <Python.h>
#include <unistd.h>

void init_interpreter();
void end_interpreter();
PyObject* init_module(char*);
void end_module(PyObject*);
void* call_python_function(PyObject*, char*,char* [], int, unsigned int*);
