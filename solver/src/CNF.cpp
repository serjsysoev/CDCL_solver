#include "CNF.h"

utils::Maybe<int> CNF::Clause::get_maybe_updatable_variable_id() const
{
	int var_id = -1;
	for (const auto &literal : literals)
	{
		if (literal.get_value() == Value::Undefined)
		{
			if (var_id < 0)
			{
				var_id = literal.var->id;
				continue;
			}
			return utils::Maybe<int>(-1, false);
		}
	}

	return utils::Maybe<int>(var_id, var_id >= 0);
}

CNF::Value CNF::Literal::get_value() const
{
	if (var->value == Value::Undefined)
	{
		return Value::Undefined;
	}
	if (has_negate)
	{
		return var->value == Value::True ? Value::False : Value::True;
	}
	return var->value;
}

std::ostream &operator<<(std::ostream &out, const CNF::Variable &var)
{
	out << var.id << " = ";
	switch (var.value)
	{

		case CNF::Value::False:
			out << 0;
			break;
		case CNF::Value::True:
			out << 1;
			break;
		case CNF::Value::Undefined:
			out << "undefined";
			break;
	}
	return out;
}

std::ostream &operator<<(std::ostream &out, const CNF::Literal &literal)
{
	if (literal.has_negate) {
		out << '-';
	}
	out << literal.var->id;
	return out;
}

std::ostream &operator<<(std::ostream &out, const CNF::Clause &clause)
{
	for (const auto &literal : clause.literals) {
		out << literal << ' ';
	}
	return out;
}
