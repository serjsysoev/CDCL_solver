#include <algorithm>
#include <cassert>
#include "solver.h"

namespace CDCL {
	int test() {
		return 5;
	}

	void Solver::add_clause(const std::vector<WeakLiteral> &clause) {
		std::vector<CNF::Literal> result_clause;
		for (const auto &literal: clause) {
			if (variables.find(literal.id) == variables.end()) {
				variables[literal.id] = std::make_shared<CNF::Variable>(CNF::Variable(literal.id));
			}
			result_clause.emplace_back(variables[literal.id], literal.has_negate);
		}
		cnf.emplace_back(result_clause);
	}

	void Solver::print_clauses() const {
		for (const auto &clause: cnf) {
			std::cout << clause << '\n';
		}
	}

	utils::Maybe<std::vector<VariableConfig>> Solver::solve() {
		std::vector<VariableValueDecision> current_variables_stack;
		set_same_sign_variables(current_variables_stack);

		while (true) {
			int i = 0;
			bool restart = false;
			for (auto &clause : cnf) {
				i++;
				if (clause.needs_attention()) {
					auto &literals = clause.literals;

					auto literal = literals.size() == 1 ? literals.front() : clause.get_watched_literals().front();
					if (handle_clause_needs_attention(literal, current_variables_stack)) {
						return {};
					}
					restart = true;
					break;
				}
			}
			if (restart) {
				continue;
			}
			if (current_variables_stack.size() == variables.size()) {
				break;
			}
			int next_variable_id = get_next_unassigned_variable();
			current_variables_stack.emplace_back(next_variable_id, false);
			variables[next_variable_id]->value = CNF::Value::False;
		}


		// constructing result
		std::vector<VariableConfig> result;
		for (const auto &var_with_decision: current_variables_stack) {
			int id = var_with_decision.var_id;
			result.emplace_back(id, variables[id]->value == CNF::Value::True);
		}

		return utils::Maybe(result);
	}

	bool Solver::handle_clause_needs_attention(CNF::Literal &literal,
											   std::vector<VariableValueDecision> &current_variables_stack) {
		if (literal.get_value() == CNF::Value::False) {
			if (!change_last_decision(current_variables_stack)) {
				return true;
			}
		} else {
			int id = literal.var->id;
			current_variables_stack.emplace_back(id, true);
			variables[id]->value = literal.has_negate ? CNF::Value::False : CNF::Value::True;
		}
		return false;
	}

	bool Solver::change_last_decision(std::vector<VariableValueDecision> &current_variables_stack) {
		// This is like we are looking for the least ancestor which has more "righter" path
		while (!current_variables_stack.empty()) {
			auto &stack_back = current_variables_stack.back();
			if (stack_back.is_automatically_determined) {
				variables[stack_back.var_id]->value = CNF::Value::Undefined;
				current_variables_stack.pop_back();
				continue;
			}
			if (variables[stack_back.var_id]->value == CNF::Value::True) {
				variables[stack_back.var_id]->value = CNF::Value::Undefined;
				current_variables_stack.pop_back();
				continue;
			}
			variables[stack_back.var_id]->value = CNF::Value::True;
			return true;
		}

		// if we found contradictions to all of our variables prefixes then this should be enough to prove UNSAT
		return false;
	}

	int Solver::get_next_unassigned_variable() const {
		for (const auto &[id, val]: variables) {
			if (val->value == CNF::Value::Undefined) {
				return id;
			}
		}
		return -1;
	}

	enum class VariableOccurrences {
		Never,
		OnlyNegated,
		OnlyNonNegated,
		Both
	};

	void Solver::set_same_sign_variables(std::vector<VariableValueDecision> &current_variables_stack) {
		std::unordered_map<int, VariableOccurrences> variable_sign;
		for (const CNF::Clause &clause: cnf) {
			for (const CNF::Literal &literal: clause.literals) {
				int id = literal.var->id;
				switch (variable_sign[id]) {
					case VariableOccurrences::Never:
						variable_sign[id] = literal.has_negate ? VariableOccurrences::OnlyNegated
															   : VariableOccurrences::OnlyNonNegated;
						break;
					case VariableOccurrences::OnlyNegated:
						if (!literal.has_negate) {
							variable_sign[id] = VariableOccurrences::Both;
						}
						break;
					case VariableOccurrences::OnlyNonNegated:
						if (literal.has_negate) {
							variable_sign[id] = VariableOccurrences::Both;
						}
						break;
					case VariableOccurrences::Both:
						break;
				}
			}
		}
		for (const auto [id, variable_occurrences]: variable_sign) {
			switch (variable_occurrences) {
				case VariableOccurrences::OnlyNegated:
					current_variables_stack.emplace_back(id, true);
					variables[id]->value = CNF::Value::False;
					break;
				case VariableOccurrences::OnlyNonNegated:
					current_variables_stack.emplace_back(id, true);
					variables[id]->value = CNF::Value::True;
					break;
				case VariableOccurrences::Never:
				case VariableOccurrences::Both:
					break;
			}
		}
	}
}
