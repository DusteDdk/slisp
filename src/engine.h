#ifndef SLISP_EMBED_INCLUDED
#define SLISP_EMBED_INCLUDED

#include "fundamental.h"
#include "toker.h"
#include "parsey.h"
#include "interpreter.h"
#include "ondemandistream.h"


class SlispEngine {
private:
    std::string fileName;
    OnDemandIStream input;
    TokenProvider top;
	Parsey parsey;
public:
    SlispEngine();
    FundamentalRef head;
	Interpreter inter;

    void reset();
    void setFileName(std::string newFileName);
    void setStdInEnabled(bool useStdIn);
    bool eval(std::string prg);
};

class StrOnlySlispEngine {
    private:
    SlispEngine engine;
    public:
    StrOnlySlispEngine(std::string fn);
    std::string getHeadStr();
    bool eval(std::string prg);
    void reset();
};

#endif