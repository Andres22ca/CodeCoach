// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_ANALYZER_CLIENT_H
#define LIB_CODECOACH_ANALYZER_CLIENT_H

#include "contracts/analyzer_dto.h"
#include "contracts/eval_dto.h"
#include "contracts/problem_dto.h"
#include "http/http_client.h"

#include <string>

namespace cc::sdk {

    class AnalyzerClient {
    private:
        http::HttpClient httpClient_;
        std::string      baseUrl_;

    public:
        explicit AnalyzerClient(const std::string& baseUrl);

        // Analizar código y obtener feedback del analizador/LLM
        cc::contracts::CoachFeedback analyze(
            const std::string&              code,
            const cc::contracts::RunResult& evalResult,
            const std::string&              problemId
        );
    };

} // namespace cc::sdk

#endif // LIB_CODECOACH_ANALYZER_CLIENT_H
