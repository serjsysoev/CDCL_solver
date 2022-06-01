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

		explicit Variable(int id = -1, Value value = Value::Undefined) : id(id), value(value)
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

	class Clause
	{
	public:
		explicit Clause(std::vector<Literal> literals) : literals(std::move(literals)), isTrue(false)
		{}

		[[nodiscard]] CNF::Value get_value() const {
			for (const auto &lit : literals) {
				if (lit.get_value() == Value::Undefined) {
					return Value::Undefined;
				}
				if (lit.get_value() == Value::True) {
					return Value::True;
				}
			}
			return Value::False;
		}

		std::vector<Literal> literals;

		[[nodiscard]] utils::Maybe<CNF::Variable> get_maybe_updatable_variable_id() const;

		void update_clause_value();
	private:
		bool isTrue;
	};
}

std::ostream& operator<<(std::ostream& out, const CNF::Variable& var);
std::ostream& operator<<(std::ostream& out, const CNF::Literal& literal);
std::ostream& operator<<(std::ostream& out, const CNF::Clause& clause);

#endif //CDCL_SOLVER_CNF_H
