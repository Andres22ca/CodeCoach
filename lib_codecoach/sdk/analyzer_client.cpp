//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_ANALYZER_DTO_H
#define LIB_CODECOACH_ANALYZER_DTO_H

#include <string>
#include <vector>

namespace cc::contracts {

    struct AlgorithmGuess {
        std::string name;
        int confidence; // 0–100
    };

    struct ComplexityEstimate { // nombre con mayúscula consistente
        std::string time;   // e.g. "O(n log n)"
        std::string space;  // e.g. "O(n)"
    };

    struct CoachHint {
        std::string title;  // <- aquí estaba "tile" antes
        std::string body;
        int level;          // 1 = básico, 2 = intermedio, etc.
    };

    struct CoachFeedback {
        std::vector<CoachHint> hints; // máx. 3
        std::string nextStep;
        std::string commonMistake;
        ComplexityEstimate complexity;  // <- tipo correcto
        AlgorithmGuess     algorithm;
    };

} // namespace cc::contracts

#endif // LIB_CODECOACH_ANALYZER_DTO_H
