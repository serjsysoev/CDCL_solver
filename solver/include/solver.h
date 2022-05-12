#ifndef CDCL_SOLVER_SOLVER_H
#define CDCL_SOLVER_SOLVER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "CNF.h"


namespace CDCL
{
	int test();

	struct WeakLiteral
	{
		int id;
		bool hasNegate;

		WeakLiteral(int id, bool hasNegate) : id(id), hasNegate(hasNegate)
		{}
	};

	class Solver
	{
	public:
		Solver() = default;

		void add_clause(const std::vector<WeakLiteral> &clause);

		void print_clauses() const;

	private:
		std::vector<CNF::Clause> cnf;
		std::unordered_map<int, std::shared_ptr<CNF::Variable>> variables;
	};
}

#endif //CDCL_SOLVER_SOLVER_H
