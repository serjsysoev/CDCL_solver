#include "dimacs_parser.h"
#include "solver.h"

#include <fstream>
#include <functional>
#include <sstream>

struct Header {
    int number_of_variables;
    int number_of_clauses;
};

bool string_starts_with(const std::string &str, const std::string &start) {
    auto str_it = str.begin();
    auto start_it = start.begin();

    while (str_it != str.end() && start_it != start.end()) {
        if (*str_it != *start_it) return false;
        ++str_it, ++start_it;
    }

    return start_it == start.end();
}

void handle_header_error() {
    throw std::runtime_error("Invalid DIMACS\n"
                             "DIMACS file should start with the problem line of form\n"
                             "p cnf {number of variables} {number of clauses}\n");
}

bool is_comment_or_empty(const std::string &line) {
    return line.empty() || string_starts_with(line, "c ");
}

void check_variable_validity(int number_of_variables, int absolute_variable) {
    if (absolute_variable > number_of_variables) {
        throw std::runtime_error("Variables should be <= number stated in the problem line");
    }
}

void process_lines(std::ifstream &input_file, const std::function<bool(std::istringstream &)> &handler) {
    for (std::string line; std::getline(input_file, line);) {
        if (is_comment_or_empty(line)) {
            continue;
        }
        std::istringstream iss(line);

        if (handler(iss)) {
            return;
        }
    }
}

Header read_header(std::ifstream &input_file) {
    Header header{};

    process_lines(input_file, [&header](std::istringstream &iss) {
        if (iss.get() == 'p' && iss.get() == ' ' && iss.good()) {
            iss >> header.number_of_variables >> header.number_of_clauses;
            if (iss.fail()) {
                handle_header_error();
            }
        } else {
            handle_header_error();
        }
        return true;
    });

    return header;
}

CDCL::Solver read_cnf(const Header &header, std::ifstream &input_file) {
    CDCL::Solver solver;
    std::vector<CDCL::WeakLiteral> clause;

    int number_of_clauses = 0;
    process_lines(input_file, [&header, &solver, &clause, &number_of_clauses](std::istringstream &iss) {
        int variable;
        while (!iss.eof() && iss >> variable) {
            if (variable == 0) {
                solver.add_clause(clause);
                clause.clear();
                ++number_of_clauses;
            } else {
                int absolute_variable = abs(variable);
                check_variable_validity(header.number_of_variables, absolute_variable);
                clause.emplace_back(absolute_variable, absolute_variable != variable);
            }
        }
        if (iss.fail()) {
            throw std::runtime_error("Error while parsing DIMACS file");
        }
        return false;
    });

    if (!clause.empty()) {
        throw std::runtime_error("Clauses should end with 0");
    }

    if (header.number_of_clauses != number_of_clauses) {
        throw std::runtime_error("Number of clauses should be equal to that in the problem line");
    }

    return solver;
}

CDCL::Solver DimacsParser::parse_dimacs_file(std::ifstream &input_file) {
    Header header = read_header(input_file);
    return read_cnf(header, input_file);
}