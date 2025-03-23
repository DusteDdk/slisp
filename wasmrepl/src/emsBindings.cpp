#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include "../../src/engine.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(slisp_module) {
    class_<StrOnlySlispEngine>("StrOnlySlispEngine")
        .constructor<std::string>()
        .function("getHeadStr", &StrOnlySlispEngine::getHeadStr)
        .function("eval", &StrOnlySlispEngine::eval)
        .function("reset", &StrOnlySlispEngine::reset);
}
#endif