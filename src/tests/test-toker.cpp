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

istringstream tokerTestStream(R"TOKER_TEST_STR({
    directlyNamedVar: "testString"
    : 123 // Indirectly named var (should be named testString)
    directlyNamedKnownVar:: 123
    directlyNamedQuery:?
    :? // Ask if a variable named 123 exists
    (print (+ 1 (+ 2 3)))
    aList: [1 2 3 4 5]
    1
    -1
    -.1
    0.1
    -0.1
    (print "An \"escaped\" string")
    @
})TOKER_TEST_STR");

TEST_CASE( "TokenProvider" ) {

    TokenProvider tokpro(tokerTestStream, "test.slisp");
    while( tokpro.advance() ) {
        println("{}", TokInfoStr(tokpro.curToken));
    }
}
