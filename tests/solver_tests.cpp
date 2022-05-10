#include "solver.h"

#include "doctest/doctest.h"

using namespace solver;

TEST_SUITE("solver") {
    TEST_CASE("test") {
        SUBCASE("==5") {
            CHECK_EQ(4, test());
        }
    }
}