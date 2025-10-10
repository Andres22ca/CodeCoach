//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_ANALYZER_DTO_H
#define LIB_CODECOACH_ANALYZER_DTO_H
#include <string>
#include <vector>

struct AlgorithmGuess {
    std::string name;
    int confidence; // va de 0-100 y sirve para ver la seguridad del analizador

};

struct complexityEstimate { //para aplicar las formulas de medicion
    std::string time; //o(n log n)
    std::string space;//o(n)


};
struct CoachHint {
    std::string tile;
    std::string body;
    int level;

};

struct CoachFeedback {
    std::vector<CoachHint> hints; // máx. 3
    std::string nextStep;
    std::string commonMistake;
    ComplexityEstimate complexity;
    AlgorithmGuess algorithm;
};











#endif //LIB_CODECOACH_ANALYZER_DTO_H

// modelos de respuesta para el coach