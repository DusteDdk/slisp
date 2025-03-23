#include "toker.h"
#include "ondemandistream.h"

#include <print>

OnDemandStreamBuf::OnDemandStreamBuf(bool _useStdin): std::streambuf(), useStdIn(_useStdin) {
    // We start with an empty buffer. Underflow/uflow will fetch.
    setg(nullptr, nullptr, nullptr);
}


void OnDemandStreamBuf::add(std::string cmd) {
    cmds.push(cmd);
}

void OnDemandStreamBuf::reset() {
    std::queue<std::string> empty;
    buffer_ = "";
    std::swap( cmds, empty );
}

int OnDemandStreamBuf::underflow() {
    // If we still have chars left in the buffer, just return.
    if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    // We end up here if the buffer is drained, acquire new characters!
    buffer_="";

    if(!cmds.empty()) {
        buffer_ = cmds.front();
        cmds.pop();
        // Add an END_OF_TEXT only when it's the last cmd.
        if(cmds.empty()) {
            buffer_ += END_OF_TEXT;
        }
    } else if(useStdIn && std::cin) {


        if(top->getBlockReason() != BlockReason::Waiting) {
            std::print("{} ", BlockReasonStr(top->getBlockReason()));
        }
        std::print("~ ");

        std::string line("");

        if(std::getline(std::cin, line)) {
            if( line != "exit" ) {
                std::cin.clear();
                buffer_ = line + "\n" + END_OF_TEXT;
            }
        }
    }

    if (buffer_.empty()) {
        // No more data? That is EOF.
        return traits_type::eof();
    }

    // Hand off our newly filled buffer to the base class internals.
    // setg() wants pointers into a char array. We can use &buffer_[0].
    setg(&buffer_[0], &buffer_[0], &buffer_[0] + buffer_.size());

    // Return current character.
    return traits_type::to_int_type(*gptr());
}


OnDemandIStream::OnDemandIStream(): std::istream(&buf), buf(false) {}

void OnDemandIStream::setMidTokenIndicator(TokenProvider* top)
{
    buf.top=top;
}


void OnDemandIStream::setStdInEnabled(bool useStdIn)
{
    buf.useStdIn=useStdIn;
}

void OnDemandIStream::reset()
{
    buf.reset();
    clear(); // Also reset the pointers
}

void OnDemandIStream::add(std::string cmd) {
    buf.add(cmd);
}
