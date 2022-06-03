#ifndef CDCL_SOLVER_SOLVER_H
#define CDCL_SOLVER_SOLVER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "CNF.h"


namespace CDCL {
	int test();

	struct WeakLiteral {
		int id;
		bool has_negate;

		WeakLiteral(int id, bool has_negate) : id(id), has_negate(has_negate) {}
	};

	struct VariableConfig {
		int id;
		bool value;

		VariableConfig(int id, bool value) : id(id), value(value) {}
	};

	class Solver {
	public:
		Solver() = default;

		Solver &operator=(Solver &&other) noexcept {
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

		struct VariableValueDecision {
			int var_id;
			bool is_automatically_determined;

			VariableValueDecision(int var_id, bool is_automatically_determined)
					: var_id(var_id), is_automatically_determined(is_automatically_determined) {}
		};

		bool change_last_decision(std::vector<VariableValueDecision> &current_variables_stack);

		std::vector<CNF::Clause> cnf;
		std::unordered_map<int, std::shared_ptr<CNF::Variable>> variables;

        void set_same_sign_variables(std::vector<VariableValueDecision> &current_variables_stack);

		int get_next_unassigned_variable() const;
	};
}

#endif //CDCL_SOLVER_SOLVER_H
