#include <algorithm>
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
		while (current_variables_stack.size() != variables.size()) {
			UnitPropagationStatus propagating_status;
			do {
				propagating_status = unit_propagate(current_variables_stack);
			} while (propagating_status == Good);
			if (propagating_status == Bad || !checkAllClauses(current_variables_stack)) {
				if (!change_last_decision(current_variables_stack)) {
					return {};
				}
				continue;
			}
			if (current_variables_stack.size() == variables.size())
				break;
			int next_variable_id = get_next_unassigned_variable();
			current_variables_stack.emplace_back(next_variable_id, false, false);
			variables[next_variable_id]->value = CNF::Value::False;
			update_clauses_value();
		}


		// constructing result
		std::vector<VariableConfig> result;
		for (const auto &var_with_decision: current_variables_stack) {
			result.emplace_back(var_with_decision.var_id, var_with_decision.cur_value);
		}

		return utils::Maybe(result);
	}

	Solver::UnitPropagationStatus Solver::unit_propagate(std::vector<VariableValueDecision> &current_variables_stack) {
		// find all propagatable units
		std::vector<CNF::Variable> all_propagatable_variables;
		for (auto &clause: cnf) {
			auto maybe_var = clause.get_maybe_updatable_variable_id();
			if (!maybe_var.has_value) {
				continue;
			}
			all_propagatable_variables.push_back(maybe_var.value);
		}

		// checking
		std::unordered_map<int, CNF::Value> value_by_id;
		for (const auto &[id, val]: all_propagatable_variables) {
			if (value_by_id.find(id) == value_by_id.end())
				value_by_id[id] = val;
			if (value_by_id[id] != val)
				return Bad; // found contradiction
		}

		// apply changes
		apply_new_variables(current_variables_stack, value_by_id.begin(), value_by_id.end());
		return value_by_id.empty() ? NothingChanged : Good;
	}

	template<typename T>
	void Solver::apply_new_variables(std::vector<VariableValueDecision> &current_variables_stack,
									 T id_and_value_begin,
									 T id_and_value_end) {
		// applying
		for (auto it = id_and_value_begin; it != id_and_value_end; ++it) {
			auto[id, val] = *it;
			current_variables_stack.emplace_back(id, val == CNF::Value::True, true);
			variables[id]->value = val;
		}

		// updating clause's result
		update_clauses_value();
	}

	void Solver::update_clauses_value() {
		for (auto &clause: cnf) {
			clause.update_clause_value();
		}
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
			if (stack_back.cur_value) {
				variables[stack_back.var_id]->value = CNF::Value::Undefined;
				current_variables_stack.pop_back();
				continue;
			}
			variables[stack_back.var_id]->value = CNF::Value::True;
			stack_back.cur_value = true;
			update_clauses_value();
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

	bool Solver::checkAllClauses(const std::vector<VariableValueDecision> &current_variables_stack) const {
		for (auto &clause: cnf) {
			if (clause.get_value() == CNF::Value::False) {
				return false;
			}
		}

		return true;

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
		std::vector<std::pair<int, CNF::Value>> id_and_value;
		for (const auto [id, variable_occurrences]: variable_sign) {
			if (variable_occurrences == VariableOccurrences::OnlyNegated ||
				variable_occurrences == VariableOccurrences::OnlyNonNegated) {
				id_and_value.emplace_back(
						id,
						variable_occurrences == VariableOccurrences::OnlyNegated ? CNF::Value::False : CNF::Value::True
				);
			}
		}
		apply_new_variables(current_variables_stack, id_and_value.begin(), id_and_value.end());
	}
}
