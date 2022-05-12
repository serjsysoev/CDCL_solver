cmake_minimum_required(VERSION 3.17)
project(CDCL_solver)

set(CMAKE_CXX_STANDARD 17)

include_directories(solver/include)
add_library(solver OBJECT solver/src/solver.cpp solver/include/solver.h solver/src/CNF.cpp solver/include/CNF.h solver/include/utils.h)

enable_testing()

include(cmake/CPM.cmake)
CPMAddPackage(
        GITHUB_REPOSITORY doctest/doctest
        GIT_TAG 2.4.5
)

add_executable(CDCL_solver_test tests/solver_tests.cpp tests/test.cpp)
target_link_libraries(CDCL_solver_test PUBLIC solver)
target_link_libraries(CDCL_solver_test PRIVATE doctest)
add_test(NAME CDCL_solver_test COMMAND CDCL_solver_test)

add_executable(CDCL_solver src/main.cpp)
target_link_libraries(CDCL_solver PUBLIC solver)