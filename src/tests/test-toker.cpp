#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <print>

#include "toker.h"

using namespace std;


TEST_CASE( "Toker" ) {
    istringstream str(R"TOKER_TEST_STR({   // iList begin, this comment ignored.
someThing // Identifier
: // Name
12 // Number

someThing:: -.12 // Specific name, Number

)TOKER_TEST_STR");
    Toker toker(str);
    Token curToken;//, nextToken;

    while( (curToken = toker.nextToken()) != Token::Eof) {
        //nextToken = toker.peek();

        println("Line:{} Column: {} {} Str:{}", toker.line, toker.column, tokName(curToken), toker.str);
    }
}