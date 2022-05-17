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
		for (const auto &clause : cnf)
		{
			std::cout << clause << '\n';
		}
	}

	utils::Maybe<std::vector<VariableConfig>> Solver::solve()
	{
		std::vector<VariableValueDecision> current_variables_stack;
		while (current_variables_stack.size() != variables.size())
		{
			auto PropagatingStatus = unit_propagate(current_variables_stack);
			while (PropagatingStatus == Good) {
				PropagatingStatus = unit_propagate(current_variables_stack);
			}
			if (PropagatingStatus == Bad) {
				if (!change_last_decision(current_variables_stack)) {
					return {{}, false};
				}
				continue;
			}
			if (current_variables_stack.size() == variables.size())
				break;
			int next_variable_id = get_next_unassigned_variable();
			current_variables_stack.emplace_back(next_variable_id, false, false);
			variables[next_variable_id]->value = CNF::Value::False;
			updateClausesValue();

		}


		// constructing result
		std::vector<VariableConfig> result;
		for (const auto& var_with_decision : current_variables_stack) {
			result.emplace_back(var_with_decision.var_id, var_with_decision.cur_value);
		}

		return {result, true};
	}

	Solver::UnitPropagationStatus Solver::unit_propagate(std::vector<VariableValueDecision> &current_variables_stack)
	{
		// find all propagatable units
		std::vector<CNF::Variable> all_propagatable_variables;
		for (auto &clause : cnf)
		{
			auto maybe_var = clause.get_maybe_updatable_variable_id();
			if (!maybe_var.has_value)
			{
				continue;
			}
			all_propagatable_variables.push_back(maybe_var.value);
		}

		// checking
		std::unordered_map<int, CNF::Value> value_by_id;
		for (const auto&[id, val] : all_propagatable_variables)
		{
			if (value_by_id.find(id) == value_by_id.end())
				value_by_id[id] = val;
			if (value_by_id[id] != val)
				return Bad; // found contradiction
		}

		// apply changes
		apply_new_variables(current_variables_stack, value_by_id);
		return value_by_id.empty() ? NothingChanged : Good;
	}

	void Solver::apply_new_variables(std::vector<VariableValueDecision> &current_variables_stack,
	                                 const std::unordered_map<int, CNF::Value> &value_by_id)
	{
		// applying
		for (const auto &[id, val] : value_by_id)
		{
			current_variables_stack.emplace_back(id, val == CNF::Value::True ? CNF::Value::True : CNF::Value::False, true);
			variables[id]->value = val;
		}

		// updating clause's result
		updateClausesValue();
	}

	void Solver::updateClausesValue()
	{
		for (auto &clause : cnf)
		{
			clause.update_clause_value();
		}
	}

	bool Solver::change_last_decision(std::vector<VariableValueDecision> &current_variables_stack)
	{
		// This is like we are looking for the least ancestor which has more "righter" path
		while (!current_variables_stack.empty()) {
			if (current_variables_stack.back().isAutomaticallyDetermined) {
				variables[current_variables_stack.back().var_id]->value = CNF::Value::Undefined;
				current_variables_stack.pop_back();
				continue;
			}
			if (current_variables_stack.back().cur_value) {
				variables[current_variables_stack.back().var_id]->value = CNF::Value::Undefined;
				current_variables_stack.pop_back();
				continue;
			}
			variables[current_variables_stack.back().var_id]->value = CNF::Value::True;
			current_variables_stack.back().cur_value = true;
			updateClausesValue();
			return true;
		}

		// if we found contradictions to all of our variables prefixes then this should be enough to prove UNSAT
		return false;
	}

	int Solver::get_next_unassigned_variable() const
	{
		for (const auto&[id, val] : variables) {
			if (val->value == CNF::Value::Undefined) {
				return id;
			}
		}
		return -1;
	}
}
