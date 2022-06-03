#include "CNF.h"

utils::Maybe<int> CNF::Clause::find_first_non_false_literal(int excluded_index = -1) {
	for (int literal_index = 0; literal_index < literals.size(); ++literal_index) {
		if (literal_index == excluded_index) continue;
		auto &literal = literals[literal_index];
		if (literal.get_value() != CNF::Value::False) {
			return utils::Maybe(literal_index);
		}
	}
	return {};
}

bool CNF::Clause::needs_attention() {
	if (literals.size() == 1) {
		return literals.front().get_value() != CNF::Value::True;
	}
	if (literals[watched_literals[0]].get_value() == Value::False) {
		std::swap(watched_literals[0], watched_literals[1]);
	}

	if (literals[watched_literals[0]].get_value() == Value::False) {
		auto result = find_first_non_false_literal();
		if (result.has_value) {
			watched_literals[0] = result.value;
		} else {
			return true;
		}
	}
	if (literals[watched_literals[0]].get_value() == Value::True) {
		return false;
	}

	if (literals[watched_literals[1]].get_value() == Value::False) {
		auto result = find_first_non_false_literal(watched_literals[0]);
		if (result.has_value) {
			watched_literals[0] = result.value;
		}
		return !result.has_value;
	}
	return false;
}

std::array<CNF::Literal, 2> CNF::Clause::get_watched_literals() {
	return {literals[watched_literals[0]], literals[watched_literals[1]]};
}

CNF::Value CNF::Literal::get_value() const {
	auto value = var->value;
	if (value == Value::Undefined) {
		return Value::Undefined;
	}
	if (has_negate) {
		return value == Value::True ? Value::False : Value::True;
	}
	return value;
}

std::ostream &operator<<(std::ostream &out, const CNF::Variable &var) {
	out << var.id << " = ";
	switch (var.value) {
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

std::ostream &operator<<(std::ostream &out, const CNF::Literal &literal) {
	if (literal.has_negate) {
		out << '-';
	}
	out << literal.var->id;
	return out;
}

std::ostream &operator<<(std::ostream &out, const CNF::Clause &clause) {
	for (const auto &literal: clause.literals) {
		out << literal << ' ';
	}
	return out;
}
