#include "solver.h"
#include <iostream>

int main() {
    std::cout << "The answer is: " << CDCL::test() << std::endl;

    CDCL::Solver solver;

    std::vector<CDCL::WeakLiteral> a;
	a.push_back(CDCL::WeakLiteral(1, true));
	a.push_back(CDCL::WeakLiteral(2, false));
	a.push_back(CDCL::WeakLiteral(24, true));
	a.push_back(CDCL::WeakLiteral(112, false));
	a.push_back(CDCL::WeakLiteral(5, true));

	solver.add_clause(a);
	solver.print_clauses();

    return 0;
}
