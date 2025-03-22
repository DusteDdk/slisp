#include "repl.h"
#include "token.h"
#include "toker.h"
#include "parsey.h"
#include "interpreter.h"
#include "fundamental.h"
#include "ondemandistream.h"

#include <string>
#include <print>
#include <format>
#include <memory>
#include <functional>


#include <sstream>

const std::string replInitStr = R"REPL_HELP_STR({ help: "REPL help:
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

Repl::Repl() {}

void Repl::enter()
{
    OnDemandIStream input;
    input.setStdInEnabled(true);
    TokenProvider top(input, "stdin");
    input.setMidTokenIndicator(&top);
	Parsey parsey(top);
	NodeRef node;
    FundamentalRef head = std::make_shared<FundamentalEmpty>();
	Interpreter inter;
    inter.scopey.enterScope("repl");

    input.add(replInitStr);

	while(top.advance() && top.nxtToken.token != Token::Eof ) {
        node = parsey.parse(top.curToken);
        if (node->t == NodeType::Error) {
            continue;
        }
        if(top.curToken.token == Token::Eof) {
            break;
        }

        // This is to avoid modifying the interpreter for the "repl scope"
        if(node->t == NodeType::Call) {
            head = inter.doCall( std::dynamic_pointer_cast<NodeCall>(node), head );
        } else {
            head = inter.descend(node, head, false);
        }

        if(head->t == FType::Error) {
            std::println("\n{}", head->toString());
            input.reset();
        } else {
            std::println("\n$ {}", head->toString());
        }
	}

    std::println("Bye bye.");
}
