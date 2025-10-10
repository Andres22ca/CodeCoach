//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_PROBLEM_DT_H
#define LIB_CODECOACH_PROBLEM_DT_H
#include <string>
#include <vector>

namespace cc::contracts {
    struct Sample {
        std::string input;
        std::string output;
    };



    struct  ProblemSummary{
        std::string id;
        std::string title ;
        std::vector <std::string > tags; // para arrays y dp
        std::string difficulty;

    };


    struct ProblemDetail : public ProblemSummary {
        std::string statement;
        std::vector<Sample> samples;
    };
}






#endif //LIB_CODECOACH_PROBLEM_DT_H

//Para definir los problemas con su input