#include "CNF.h"

utils::Maybe<CNF::Variable> CNF::Clause::get_maybe_updatable_variable_id() const
{
	if (isTrue) {
		return utils::Maybe<CNF::Variable>(Variable(), false);
	}
	CNF::Variable var_with_val(-1, Value::Undefined);
	for (const auto &literal : literals)
	{
		if (literal.get_value() == Value::Undefined)
		{
			if (var_with_val.value == Value::Undefined)
			{
				var_with_val.id = literal.var->id;
				if (literal.has_negate) {
					var_with_val.value = Value::False;
				} else {
					var_with_val.value = Value::True;
				}
				continue;
			}
			return utils::Maybe<Variable>(Variable(), false);
		}
	}

	return utils::Maybe<CNF::Variable>(var_with_val, var_with_val.value != Value::Undefined);
}

void CNF::Clause::update_clause_value()
{
	isTrue = false;
	for (const auto &literal : literals) {
		if (literal.get_value() == Value::True) {
			isTrue = true;
		}
	}
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
