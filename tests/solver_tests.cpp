#include "solver.h"

#include "doctest/doctest.h"

namespace {
	std::pair<CNF::Clause, std::unordered_map<int, std::shared_ptr<CNF::Variable>>> get_clause(const std::vector<CDCL::WeakLiteral> &vec) {
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

	TEST_CASE("clause get_maybe") {
		CNF::Clause clause;
		std::unordered_map<int, std::shared_ptr<CNF::Variable>> variables;

		std::vector<CDCL::WeakLiteral> vec = {
				{0, false},
				{1, false},
				{2, false}
		};

		std::tie(clause, variables) = get_clause(vec);

		variables[1]->value = CNF::Value::False;
		clause.update_clause_value();
		SUBCASE("only 1 false assigned of 3 variables") {
			auto val = clause.get_maybe_updatable_variable_id();
			CHECK_EQ(false, val.has_value);
		}

		variables[0]->value = CNF::Value::False;
		clause.update_clause_value();
		SUBCASE("only 1 non assigned var left") {
			auto val = clause.get_maybe_updatable_variable_id();
			CHECK_EQ(true, val.has_value);
			CHECK_EQ(CNF::Value::True, val.value.value);
			CHECK_EQ(2, val.value.id);

		}

		variables[0]->value = CNF::Value::True;
		clause.update_clause_value();
		SUBCASE("true assigned") {
			auto val = clause.get_maybe_updatable_variable_id();
			CHECK_EQ(false, val.has_value);
		}

		variables[0]->value = CNF::Value::False;
		variables[2]->value = CNF::Value::False;
		clause.update_clause_value();
		SUBCASE("all assigned to false") {
			auto val = clause.get_maybe_updatable_variable_id();
			CHECK_EQ(false, val.has_value);
		}

	}
}