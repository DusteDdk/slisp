#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <print>
#include "token.h"
#include "toker.h"
#include "parsey.h"
#include "interpreter.h"
#include "repl.h"
#include <fstream>


int main(int argc, char* argv[]) {

	if (argc == 1) {

		if (!std::cin) {
			std::println("Error: Could not open stdin.");
			return 1;
		}

		Repl r;
		r.enter();

	}
	else {
		std::string fName(argv[1]);
		std::ifstream ifs(fName, std::ifstream::in);
		TokenProvider top(ifs, fName);
		Parsey p(top);

		Interpreter i;
		if(! top.advanceSkipNoOp() ) {
			std::println("Error in input file.");
			return 1;
		}

		NodeRef firstNode = p.parse( top.curToken );
		i.run(firstNode);

	}
	return 0;
}