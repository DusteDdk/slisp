#ifndef SLISP_ONDEMAND_INPUT_STREAM
#define SLISP_ONDEMAND_INPUT_STREAM

#include <iostream>
#include <streambuf>
#include <queue>
#include "toker.h"

class OnDemandStreamBuf : public std::streambuf
{
private:
    std::string buffer_ = "";
    std::queue<std::string> cmds;
public:
    explicit OnDemandStreamBuf(bool _useStdin);
    bool useStdIn;
    void add(std::string cmd);
    TokenProvider* top=nullptr;

    void reset();

protected:
    // Called by the stream to get more data when the buffer is exhausted
    int underflow() override;
};

class OnDemandIStream : public std::istream {
private:
    OnDemandStreamBuf buf;
public:
    OnDemandIStream();
    void setMidTokenIndicator(TokenProvider* top);
    int busyLevel();
    void setStdInEnabled(bool useStdIn);
    void reset();
    void add(std::string cmd);
};


#endif

