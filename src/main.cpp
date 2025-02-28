#include <string>
#include <memory>
#include <sstream>
#include <print>
#include "token.h"
#include "toker.h"
#include "test.slisp.h"
#include "parsey.h"
#include "interpreter.h"

// Tokens ->(ast Nodes) -> Expr
#include <fstream>


int main(int argc, char* argv[]) {

	if (argc == 1) {
		std::println("No arguments provided, using builtin example:\n");
		std::istringstream debugStream(test_source);
		Toker t(debugStream);
		//Token tok;
		Parsey p(t);
		Interpreter i;
		i.run(p.program);
	}
	else {
		std::ifstream ifs(argv[1], std::ifstream::in);
		Toker t(ifs);
		//Token tok;
		Parsey p(t);
		Interpreter i;
		i.run(p.program);
	}
	return 0;
}