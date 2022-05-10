cmake_minimum_required(VERSION 3.21)
project(CDCL_solver)

set(CMAKE_CXX_STANDARD 17)

include_directories(solver/include)
add_library(solver OBJECT solver/src/solver.cpp solver/include/solver.h)

add_executable(CDCL_solver_test tests/doctest.h tests/solver_tests.cpp tests/test.cpp)
target_link_libraries(CDCL_solver_test PUBLIC solver)

add_executable(CDCL_solver src/main.cpp)
target_link_libraries(CDCL_solver PUBLIC solver)