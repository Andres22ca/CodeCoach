
#ifndef LIB_CODECOACH_ANALYZER_DTO_H
#define LIB_CODECOACH_ANALYZER_DTO_H

#include <string>
#include <vector>

namespace cc::contracts {

    struct AlgorithmGuess {
        std::string name;
        int confidence{0}; // 0–100
    };

    struct ComplexityEstimate {
        std::string time;   // e.g. "O(n log n)"
        std::string space;  // e.g. "O(n)"
    };

    struct CoachHint {
        std::string title;
        std::string body;
        int level{0};      // 1 = básico, 2 = intermedio, etc.
    };

    struct CoachFeedback {
        std::vector<CoachHint> hints; // máx. 3
        std::string nextStep;
        std::string commonMistake;
        ComplexityEstimate complexity;
        AlgorithmGuess     algorithm;
    };

} // namespace cc::contracts

#endif // LIB_CODECOACH_ANALYZER_DTO_H
