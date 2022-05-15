#include "solver.h"

namespace CDCL
{
	int test()
	{
		return 5;
	}

	void Solver::add_clause(const std::vector<WeakLiteral> &clause)
	{
		std::vector<CNF::Literal> result_clause;
		for (const auto &literal : clause)
		{
			if (variables.find(literal.id) == variables.end())
			{
				variables[literal.id] = std::make_shared<CNF::Variable>(CNF::Variable(literal.id));
			}
			result_clause.emplace_back(variables[literal.id], literal.hasNegate);
		}
		cnf.emplace_back(result_clause);
	}

	void Solver::print_clauses() const
	{
		for (const auto &clause : cnf) {
			std::cout << clause << '\n';
		}
	}
}
