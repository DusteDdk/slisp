#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <print>

#include "toker.h"

using namespace std;

/*
    Tokens:
        Loop,
        Identifier,
        Number,
        String,
        IBegin,
        IEnd,
        LBegin,
        LEnd,
        Begin,
        End,
        Eof,
        SyntaxError,
        Name,
    Other:
        Check that it skips whitespaces (\r \n \t \space)
        Check nested block comments
 */


TEST_CASE( "Toker" ) {
    istringstream str(R"TOKER_TEST_STR({   // iList begin, this comment ignored.
someThing // Identifier
: // Name
12 // Number
(print "hello world")
someThing:: -.12 // Specific name, Number

)TOKER_TEST_STR");
    Toker toker(str);
    Token curToken;

    while( (curToken = toker.nextToken()) != Token::Eof) {
        //tstr = toker.str;

        REQUIRE( (int)curToken > 0 );

     //   println("Line:{} Column: {} {} (next:  {}) Str:{}", toker.line, toker.column, tokName(curToken), tokName(nextToken), tstr);
    }

}

/*
TEST_CASE( "TokenProvider" ) {

    istringstream strb(R"TOKER_TEST_STR({   // iList begin, this comment ignored.
someThing // Identifier
: // Name
12 // Number
(print "hello world")
someThing:: -.12 // Specific name, Number)TOKER_TEST_STR");

    TokenProvider tokpro(strb, "test.slisp");
    while( tokpro.advance() ) {
        println("{}", TokInfoStr(tokpro.curToken));
    }
}*/