#ifndef CDCL_SOLVER_CNF_H
#define CDCL_SOLVER_CNF_H

#include <iostream>
#include <utility>
#include <memory>
#include <vector>

#include "utils.h"


namespace CNF
{
	enum class Value
	{
		False = 0,
		True = 1,
		Undefined = 2
	};

	struct Variable
	{
		int id;
		Value value;

		// TODO() feature: when there are no more literals in one of these classes then we can assign it
		// int count_in_literals_with_negate;
		// int count_in_literals_without_negate;

		Variable(int id, Value value = Value::Undefined) : id(id), value(value)
		{}
	};

	struct Literal
	{
		std::shared_ptr<Variable> var;
		bool has_negate;

		Literal(std::shared_ptr<Variable> var, bool has_negate) : var(std::move(var)), has_negate(has_negate)
		{}

		Value get_value() const;
	};

	struct Clause
	{
		explicit Clause(std::vector<Literal> literals) : literals(std::move(literals))
		{}

		std::vector<Literal> literals;

		[[nodiscard]] utils::Maybe<int> get_maybe_updatable_variable_id() const;
	};
}

std::ostream& operator<<(std::ostream& out, const CNF::Variable& var);
std::ostream& operator<<(std::ostream& out, const CNF::Literal& literal);
std::ostream& operator<<(std::ostream& out, const CNF::Clause& clause);

#endif //CDCL_SOLVER_CNF_H
