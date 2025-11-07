#pragma once
#include <string>
#include <vector>

namespace cc::contracts {

    struct CoachFeedback {
        std::string summary;              // texto breve con el diagnóstico
        std::vector<std::string> hints;   // lista de sugerencias/pistas
    };

} // namespace cc::contracts
