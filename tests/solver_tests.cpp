#include "solver.h"

#include "doctest/doctest.h"


TEST_SUITE("solver") {
    TEST_CASE("test") {
        SUBCASE("==5") {
            CHECK_EQ(5, CDCL::test());
        }
    }
}