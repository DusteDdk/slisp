#ifndef SDLFUNCS_INCLUDED
#define SDLFUNCS_INCLUDED

#include "interpreter.h"

void registerSDLFunc(Interpreter& inter);

#ifdef __EMSCRIPTEN__
    void registerSDLFunc(Interpreter& inter) {}
#endif


#endif