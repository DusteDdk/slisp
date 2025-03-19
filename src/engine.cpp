#include "engine.h"
#include <print>

#define DEFAULT_SLISP_ENG_FN "SlispEngine"

SlispEngine::SlispEngine():
    fileName(DEFAULT_SLISP_ENG_FN),
    input(),
    top(input, fileName),
     parsey(top),
     head(std::make_shared<FundamentalEmpty>())
     {
        inter.scopey.enterScope("dummy");
        reset();
}


void SlispEngine::reset()
{
    input.reset();
    inter.scopey.exitScope();
    inter.scopey.enterScope("SL");
}

void SlispEngine::setFileName(std::string newFileName)
{
    fileName = newFileName;
    top.reset(fileName);
}

void SlispEngine::setStdInEnabled(bool useStdIn)
{
    input.setStdInEnabled(useStdIn);
}

// Eval returns false if evaluation didn't yield a value.
bool SlispEngine::eval(std::string prg)
{
    top.reset(fileName);
    input.reset();
    input.addLine(prg);

    bool ev = false;;
    while(top.advance() && top.nxtToken.token != Token::Eof ) {
            NodeRef node = parsey.parse(top.curToken);
            ev = (top.curToken.token != Token::Eof);

            if(node->t == NodeType::Call) {
                head = inter.doCall( std::dynamic_pointer_cast<NodeCall>(node), head );
            } else {
                head = inter.descend(node, head, false);
            }
    }

    return ev;
}