#include "solver.h"
#include "dimacs_parser.h"

#include <fstream>
#include <iostream>
#include <algorithm>

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
    auto solution = solver.solve();
    if (solution.has_value) {
        std::cout << "SATISFIABLE" << "\n";
        auto &variablesConfig = solution.value;
        std::sort(variablesConfig.begin(), variablesConfig.end(),
                  [](const CDCL::VariableConfig &l, const CDCL::VariableConfig &r) {
                      return l.id < r.id;
                  });
        for (auto &variableConfig : variablesConfig) {
            std::cout << variableConfig.id * (variableConfig.value ? 1 : -1) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "UNSATISFIABLE" << std::endl;
    }

    return 0;
}
