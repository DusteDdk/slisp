#include "repl.h"
#include "token.h"
#include "toker.h"
#include "parsey.h"
#include "interpreter.h"
#include "fundamental.h"

#include <string>
#include <iostream>
#include <print>
#include <format>
#include <iostream>
#include <streambuf>
#include <string>
#include <memory>
#include <functional>
#include <queue>


using DataCallback = std::function<std::string()>;

#include <sstream>

const std::string replInitStr = R"REPL_HELP_STR({ help: "
REPL help:
    Tip: Run in rlwrap.
    exit - Exits the repl
    ctrl + c / ctrl + d - Exits the repl
    (print \"Hello World\" nl)
    First level calls are in \"repl\" scope:
    { var: 1 }
    (+ var 1 )
    (print \"Var is: \" var nl)"
"Welcome to the REPL. Typing help might do that."
}
)REPL_HELP_STR";


class OnDemandStreamBuf : public std::streambuf
{
public:
    // The callback (or data source) you want to pull data from

    explicit OnDemandStreamBuf(DataCallback callback)
        : callback_(std::move(callback)) {
        // We start with an empty buffer. Underflow/uflow will fetch.
        setg(nullptr, nullptr, nullptr);
    }

protected:
    // Called by the stream to get more data when the buffer is exhausted
    int underflow() override
    {

        // If we still have chars left in the buffer, just return.
        if (gptr() < egptr())
            return traits_type::to_int_type(*gptr());

        // We need more data, ask the user-supplied function, if it returns empty, we provide EOF.
        buffer_ = callback_();

        if (buffer_.empty()) {
            // No more data? That is effectively EOF.
            return traits_type::eof();
        }

        // Hand off our newly filled buffer to the base class internals.
        // setg() wants pointers into a char array. We can use &buffer_[0].
        setg(&buffer_[0], &buffer_[0], &buffer_[0] + buffer_.size());

        // Return current character.
        return traits_type::to_int_type(*gptr());
    }

private:
    std::string buffer_;
    DataCallback callback_;
};


std::queue<std::string> cmds;

void inject(std::string cmd) {
    cmds.push(cmd + END_OF_TEXT);
}

std::string lineProducer()
{

    if(!cmds.empty()) {
        auto cmd = cmds.front();
        cmds.pop();
        return cmd;
    }

    std::print("~ ");
    std::string line="";

    if(std::cin && std::getline(std::cin, line)) {

        if (line == "exit") {
            return "";
        }
        std::cin.clear();
        return line + "\n"+END_OF_TEXT ;
    }

    return line;
}

Repl::Repl() {}


void Repl::enter()
{

    OnDemandStreamBuf inBuf(lineProducer);
    std::istream input(&inBuf);
    TokenProvider top(input, "stdin");
	Parsey parsey(top);
	NodeRef node;
    FundamentalRef head = std::make_shared<FundamentalEmpty>();
	Interpreter inter;
    inter.scopey.enterScope("repl");

    inject(replInitStr);

	while(top.advance() && top.nxtToken.token != Token::Eof ) {
        node = parsey.parse(top.curToken);

        if(top.curToken.token == Token::Eof) {
            std::println("");
            break;
        }

        if (node->t == NodeType::Error) {
            std::println("\n{}\n", node->toString());
            continue;
        }

        // This is to avoid modifying the interpreter for the "repl scope"
        if(node->t == NodeType::Call) {
            head = inter.doCall( std::dynamic_pointer_cast<NodeCall>(node), head );
        } else {
            head = inter.descend(node, head, false);
        }

        if(head->t == FType::Error) {
            std::println("\n{}\n", head->toString());
            inject("0");
        } else {
            std::println("\n$ {}", head->toString());
        }
	}

    std::println("Bye bye.");
}
