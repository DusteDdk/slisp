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
		std::println("No arguments provided, entering read,eval,print,loop.");
		
		if (!std::cin) {
			std::println("Error: Could not open stdin.");
			return 1;
		}

		std::println("A line with only the word exit will do just that.");

		Repl r;

		r.start();

		
	}
	else {
		std::ifstream ifs(argv[1], std::ifstream::in);
		Toker t(ifs);
		//Token tok;
		Parsey p(t);
		Interpreter i;
		NodeRef firstNode = p.parseNode( t.nextToken() );
		i.run(firstNode);
	}
	return 0;
}