#include "solver.h"
#include "dimacs_parser.h"

#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./CDCL_solver path/to/dimacs/file";
        exit(1);
    }

    std::string input_file_name = argv[1];

    CDCL::Solver solver;
    try {
        std::ifstream input_file(input_file_name);
        solver = DimacsParser::parse_dimacs_file(input_file);
        input_file.close();
    } catch (const std::runtime_error &err) {
        std::cerr << err.what();
        exit(1);
    }
    solver.print_clauses();

    return 0;
}
