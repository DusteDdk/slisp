
#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <print>

#include "engine.h"

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


TEST_CASE( "Engine" ) {
        SlispEngine e;
        e.setFileName("evaltest.slisp");

        REQUIRE( e.eval("(+ 1 2)") );
        REQUIRE( e.head->t == FType::Number );
        REQUIRE( std::dynamic_pointer_cast<FundamentalNumber>( e.head )->n == 3 ) ;
}

TEST_CASE( "Engine fails unfinished input" ) {
        SlispEngine e;
        e.setFileName("evaltest.slisp");

        REQUIRE( !e.eval("(+ 1 2") );
        REQUIRE( e.eval("(+ 1 2) ") );
        REQUIRE( e.head->t == FType::Number );
        REQUIRE( std::dynamic_pointer_cast<FundamentalNumber>( e.head )->n == 3 ) ;
}

TEST_CASE( "Engine keeps head and scope across evaluations" ) {
        SlispEngine e;
        e.setFileName("scpopetest.slisp");

        REQUIRE( e.eval("{ thisIsATestVar: 1234 }") );
        REQUIRE( e.eval("thisIsATestVar") );
        REQUIRE( e.head->t == FType::Number );
        REQUIRE( std::dynamic_pointer_cast<FundamentalNumber>( e.head )->n == 1234 );
        REQUIRE( e.eval("(+ 1)") );
        REQUIRE( e.head->t == FType::Number );
        REQUIRE( std::dynamic_pointer_cast<FundamentalNumber>( e.head )->n == 1235 );
}

TEST_CASE( "Engine evaluates deep and wide" ) {
        SlispEngine e;
        e.setFileName("depthwidth.slisp");

        REQUIRE( e.eval("{ \"varTwoName\"}") );
        REQUIRE( e.eval("{ : 2 } (+ 1 (+ 1 2 ( * 2 varTwoName 4)))") );
        REQUIRE( e.eval("(+ 22)") );
        REQUIRE( e.head->t == FType::Number );
        REQUIRE( std::dynamic_pointer_cast<FundamentalNumber>( e.head )->n == 42 );

}