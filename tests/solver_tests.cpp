#include "solver.h"

#include "doctest/doctest.h"

namespace {
	std::pair<CNF::Clause, std::unordered_map<int, std::shared_ptr<CNF::Variable>>>
	get_clause(const std::vector<CDCL::WeakLiteral> &vec) {
		std::vector<CNF::Literal> result_clause;
		std::unordered_map<int, std::shared_ptr<CNF::Variable>> variables;
		for (const auto &literal: vec) {
			if (variables.find(literal.id) == variables.end()) {
				variables[literal.id] = std::make_shared<CNF::Variable>(CNF::Variable(literal.id));
			}
			result_clause.emplace_back(variables[literal.id], literal.has_negate);
		}
		return std::make_pair(CNF::Clause(result_clause), variables);
	}

	CNF::Clause create_clause_of_non_negated(std::vector<CNF::Value> values) {
		std::vector<CNF::Literal> literals;
		for (int i = 0; i < values.size(); i++) {
			literals.emplace_back(std::make_shared<CNF::Variable>(CNF::Variable(i, values[i])), false);
		}
		return CNF::Clause(literals);
	}
}

TEST_SUITE("CNF") {
	TEST_CASE("literal") {
		SUBCASE("true") {
			CNF::Variable var(0, CNF::Value::True);
			auto ptr = std::make_shared<CNF::Variable>(var);

			CHECK_EQ(CNF::Value::True, CNF::Literal(ptr, false).get_value());
		}
		SUBCASE("false has negate") {
			CNF::Variable var(0, CNF::Value::True);
			auto ptr = std::make_shared<CNF::Variable>(var);

			CHECK_EQ(CNF::Value::False, CNF::Literal(ptr, true).get_value());
		}
		SUBCASE("undefined has negate") {
			CNF::Variable var(0, CNF::Value::Undefined);
			auto ptr = std::make_shared<CNF::Variable>(var);

			CHECK_EQ(CNF::Value::Undefined, CNF::Literal(ptr, true).get_value());
		}
		SUBCASE("undefined hasn't negate") {
			CNF::Variable var(0, CNF::Value::Undefined);
			auto ptr = std::make_shared<CNF::Variable>(var);

			CHECK_EQ(CNF::Value::Undefined, CNF::Literal(ptr, false).get_value());
		}
	}


	TEST_CASE("clause get_value") {
		CNF::Clause clause;
		std::unordered_map<int, std::shared_ptr<CNF::Variable>> variables;

		std::vector<CDCL::WeakLiteral> vec = {
				{0, false},
				{1, false},
				{2, false},
				{3, false},
				{4, false},
		};

		std::tie(clause, variables) = get_clause(vec);
		SUBCASE("undefined") {
			CHECK_EQ(CNF::Value::Undefined, clause.get_value());
		}
		variables[3]->value = CNF::Value::False;
		SUBCASE("undefined") {
			CHECK_EQ(CNF::Value::Undefined, clause.get_value());
		}
		variables[0]->value = CNF::Value::False;
		variables[1]->value = CNF::Value::False;
		variables[2]->value = CNF::Value::False;
		variables[3]->value = CNF::Value::False;
		variables[4]->value = CNF::Value::False;
		SUBCASE("false") {
			CHECK_EQ(CNF::Value::False, clause.get_value());
		}

		variables[4]->value = CNF::Value::True;
		SUBCASE("true") {
			CHECK_EQ(CNF::Value::True, clause.get_value());
		}
	}

	TEST_CASE("clause needs_attention") {
		SUBCASE("literal size 1 true") {
			auto clause = create_clause_of_non_negated({CNF::Value::True});
			CHECK_FALSE(clause.needs_attention());
		}

		SUBCASE("literal size 1 false") {
			auto clause = create_clause_of_non_negated({CNF::Value::False});
			CHECK(clause.needs_attention());
		}

		SUBCASE("literal size 1 undefined") {
			auto clause = create_clause_of_non_negated({CNF::Value::Undefined});
			CHECK(clause.needs_attention());
		}

		SUBCASE("literal size 2 false false") {
			auto clause = create_clause_of_non_negated({CNF::Value::False, CNF::Value::False});
			CHECK(clause.needs_attention());
		}

		SUBCASE("literal size 2 false true") {
			auto clause = create_clause_of_non_negated({CNF::Value::False, CNF::Value::True});
			CHECK_FALSE(clause.needs_attention());
		}

		SUBCASE("literal size 2 false undefined") {
			auto clause = create_clause_of_non_negated({CNF::Value::False, CNF::Value::Undefined});
			CHECK(clause.needs_attention());
		}

		SUBCASE("literal size 2 true false") {
			auto clause = create_clause_of_non_negated({CNF::Value::True, CNF::Value::False});
			CHECK_FALSE(clause.needs_attention());
		}

		SUBCASE("literal size 2 true true") {
			auto clause = create_clause_of_non_negated({CNF::Value::True, CNF::Value::True});
			CHECK_FALSE(clause.needs_attention());
		}

		SUBCASE("literal size 2 true undefined") {
			auto clause = create_clause_of_non_negated({CNF::Value::True, CNF::Value::Undefined});
			CHECK_FALSE(clause.needs_attention());
		}

		SUBCASE("literal size 2 undefined false") {
			auto clause = create_clause_of_non_negated({CNF::Value::Undefined, CNF::Value::False});
			CHECK(clause.needs_attention());
		}

		SUBCASE("literal size 2 undefined true") {
			auto clause = create_clause_of_non_negated({CNF::Value::Undefined, CNF::Value::True});
			CHECK_FALSE(clause.needs_attention());
		}

		SUBCASE("literal size 2 undefined undefined") {
			auto clause = create_clause_of_non_negated({CNF::Value::Undefined, CNF::Value::Undefined});
			CHECK_FALSE(clause.needs_attention());
		}
	}
}