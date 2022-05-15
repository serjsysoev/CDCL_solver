#ifndef CDCL_SOLVER_DIMACS_PARSER_H
#define CDCL_SOLVER_DIMACS_PARSER_H

#include "solver.h"

#include <fstream>

namespace DimacsParser {
    CDCL::Solver parse_dimacs_file(std::ifstream &input_file);
}

#endif //CDCL_SOLVER_DIMACS_PARSER_H
