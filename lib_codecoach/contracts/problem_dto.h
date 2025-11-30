// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_PROBLEM_DTO_H
#define LIB_CODECOACH_PROBLEM_DTO_H

#include <string>
#include <vector>

namespace cc::contracts {

    struct Sample {
        std::string input;
        std::string output;
    };

    struct ProblemSummary {
        std::string id;
        std::string title;
        std::vector<std::string> tags; // p.ej. ["arrays", "dp"]
        std::string difficulty;        // p.ej. "easy", "medium", "hard"
    };

    struct ProblemDetail : public ProblemSummary {
        std::string           statement; // enunciado completo (HTML o texto)
        std::vector<Sample>   samples;   // ejemplos de entrada/salida
    };

} // namespace cc::contracts

#endif // LIB_CODECOACH_PROBLEM_DTO_H
