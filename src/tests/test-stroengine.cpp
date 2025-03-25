#include <catch2/catch_test_macros.hpp>
#include "engine.h"

TEST_CASE( "StrOnlyEngine" ) {
        StrOnlySlispEngine e("test");

        REQUIRE( e.eval("(+ 1 2)") );
        REQUIRE( e.getHeadStr() == "3.000000" );
}