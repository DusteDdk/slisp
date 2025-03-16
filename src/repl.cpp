
#include <string>
#include <iostream>
#include <print>
#include <format>

#include "repl.h"
#include "token.h"
#include "toker.h"
#include "parsey.h"
#include "fundamental.h"
#include "interpreter.h"

#include <iostream>
#include <streambuf>
#include <string>
#include <memory>
#include <functional>


using DataCallback = std::function<std::string()>;

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

        // We need more date, ask the user-supplied function, if it returns empty, we provide EOF.
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

std::string lineProducer()
{
    std::print("> ");
    std::string line="(= 1 0)";
    if(std::cin) {
        if (std::getline(std::cin, line)) {
            if (line == "exit") {
                std::println("Bye bye!");
                return "";
            }

            std::cin.clear();
        } else {
            return "";
        }
    }

    return line.length() ? std::format("{}\n", line): "\n";
}


void Repl::start() {

    OnDemandStreamBuf inBuf(lineProducer);
    std::istream input(&inBuf);

	Toker toker(input);
	Parsey parsey(toker);
	NodeRef nextNode;
	Token nextToken;
	Interpreter inter;
	FundamentalRef head = std::make_shared<FundamentalEmpty>();
	inter.scopey.enterScope("repl");

	while( true ) {
		nextToken = toker.nextToken();

        if(nextToken == Token::Eof) {
            std::println("\nToken::Eof: Input closed.");
            break;
        }

		nextNode = parsey.parseNode(nextToken);
        if (nextNode->t == NodeType::Stop) {
            break;
        }

        if (nextNode->t == NodeType::Error) {
            std::println("{}", nextNode->toString());
            continue;
        }
		head = inter.descend(nextNode, head, false, true);
		std::println("{} ==> {}", nextNode->toString(), head->toString());
	}

}
