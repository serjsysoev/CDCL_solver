#ifndef CDCL_SOLVER_CNF_H
#define CDCL_SOLVER_CNF_H

#include <iostream>
#include <utility>
#include <memory>
#include <vector>
#include <array>

#include "utils.h"


namespace CNF {
	enum class Value {
		False = 0,
		True = 1,
		Undefined = 2
	};

	struct Variable {
		int id;
		Value value;

		// TODO() feature: when there are no more literals in one of these classes then we can assign it
		// int count_in_literals_with_negate;
		// int count_in_literals_without_negate;

		explicit Variable(int id = -1, Value value = Value::Undefined) : id(id), value(value) {}
	};

	struct Literal {
		std::shared_ptr<Variable> var;
		bool has_negate;

		Literal(std::shared_ptr<Variable> var, bool has_negate) : var(std::move(var)), has_negate(has_negate) {}

		[[nodiscard]] Value get_value() const;
	};

	class Clause {
	public:
		explicit Clause(std::vector<Literal> literals = {}) : literals(std::move(literals)) {}

		Clause(const Clause &other) {
			literals.clear();
			watched_literals = other.watched_literals;

			literals = other.literals;
		}

		Clause(Clause &&other) noexcept {
			literals.clear();
			watched_literals = other.watched_literals;

			literals = std::move(other.literals);
		}

		Clause &operator=(const Clause &other) {
			literals.clear();
			watched_literals = other.watched_literals;

			literals = other.literals;
			return *this;
		}

		Clause &operator=(Clause &&other) noexcept {
			literals.clear();
			watched_literals = other.watched_literals;

			literals = other.literals;

			return *this;
		}

		[[nodiscard]] CNF::Value get_value() const {
			bool has_undefined = false;
			for (const auto &lit: literals) {
				if (lit.get_value() == Value::Undefined) {
					has_undefined = true;
				}
				if (lit.get_value() == Value::True) {
					return Value::True;
				}
			}
			return has_undefined ? Value::Undefined : Value::False;
		}

		std::vector<Literal> literals;

		bool needs_attention();

		std::array<Literal, 2> get_watched_literals();

	private:
		std::array<int, 2> watched_literals = {0, 1};

		utils::Maybe<int> find_first_non_false_literal(int excluded_index);

	};
}

std::ostream &operator<<(std::ostream &out, const CNF::Variable &var);

std::ostream &operator<<(std::ostream &out, const CNF::Literal &literal);

std::ostream &operator<<(std::ostream &out, const CNF::Clause &clause);

#endif //CDCL_SOLVER_CNF_H
