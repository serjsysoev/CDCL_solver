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

	struct VariableConfig
	{
		int id;
		bool value;

		VariableConfig(int id, bool value) : id(id), value(value)
		{}
	};

	class Solver
	{
	public:
		Solver() = default;

		Solver& operator=(Solver&& other) noexcept
		{
			if (this == &other)
				return *this;

			cnf = std::move(other.cnf);
			variables = std::move(other.variables);
			return *this;
		}

		Solver(const Solver &other) {
			if (this == &other) {
				return;
			}

			cnf = other.cnf;
			variables = other.variables;
		}

		void add_clause(const std::vector<WeakLiteral> &clause);

		[[nodiscard]] utils::Maybe<std::vector<VariableConfig>> solve();

		void print_clauses() const;

	private:
		struct VariableValueDecision
		{
			int var_id;
			bool cur_value;
			bool isAutomaticallyDetermined;

			VariableValueDecision(
					int varId,
					bool curValue,
					bool isAutomaticallyDetermined
			) : var_id(varId), cur_value(curValue),
			    isAutomaticallyDetermined(isAutomaticallyDetermined)
			{}
		};

		enum UnitPropagationStatus {
			Bad,
			Good,
			NothingChanged
		};

		int get_next_unassigned_variable() const;

		bool change_last_decision(std::vector<VariableValueDecision> &current_variables_stack);

		UnitPropagationStatus unit_propagate(std::vector<VariableValueDecision> &current_variables_stack);
		void apply_new_variables(std::vector<VariableValueDecision> &current_variables_stack,
		                         const std::unordered_map<int, CNF::Value> &value_by_id);;

		void updateClausesValue();

		std::vector<CNF::Clause> cnf;
		std::unordered_map<int, std::shared_ptr<CNF::Variable>> variables;
	};
}

#endif //CDCL_SOLVER_SOLVER_H
