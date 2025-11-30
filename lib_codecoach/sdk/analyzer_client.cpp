// Created by andres on 5/10/25.
//

#include "analyzer_client.h"
#include "logging/logger.h"

#include <exception>
#include <nlohmann/json.hpp>

namespace cc::sdk {

using nlohmann::json;
using cc::contracts::RunResult;
using cc::contracts::CoachFeedback;
using cc::contracts::CoachHint;
using cc::contracts::ComplexityEstimate;
using cc::contracts::AlgorithmGuess;

AnalyzerClient::AnalyzerClient(const std::string& baseUrl)
    : baseUrl_(baseUrl)
{
    httpClient_.setTimeout(60000); // 60s para análisis
    httpClient_.setDefaultHeader("Content-Type", "application/json");
}

static json runresult_to_json(const RunResult& r) {
    json j;
    j["passed"]   = r.passed;
    j["timeMs"]   = r.timeMs;
    j["memoryKB"] = r.memoryKB;
    j["exitCode"] = r.exitCode;
    j["stdout"]   = r.stdout;
    j["stderr"]   = r.stderr;

    j["cases"] = json::array();
    for (const auto& c : r.cases) {
        json cj;
        cj["input"]    = c.input;
        cj["output"]   = c.output;
        cj["expected"] = c.expected;
        cj["passed"]   = c.passed;
        cj["timeMs"]   = c.timeMs;
        cj["memoryKB"] = c.memoryKB;
        j["cases"].push_back(std::move(cj));
    }

    return j;
}

static CoachFeedback feedback_from_json(const json& j) {
    CoachFeedback f;

    if (j.contains("hints") && j["hints"].is_array()) {
        for (const auto& hj : j["hints"]) {
            CoachHint h;
            h.title = hj.value("title", std::string{});
            h.body  = hj.value("body",  std::string{});
            h.level = hj.value("level", 0);
            f.hints.push_back(std::move(h));
        }
    }

    f.nextStep      = j.value("nextStep",      std::string{});
    f.commonMistake = j.value("commonMistake", std::string{});

    if (j.contains("complexity")) {
        const auto& cj = j["complexity"];
        f.complexity.time  = cj.value("time",  std::string{});
        f.complexity.space = cj.value("space", std::string{});
    }

    if (j.contains("algorithm")) {
        const auto& aj = j["algorithm"];
        f.algorithm.name       = aj.value("name",       std::string{});
        f.algorithm.confidence = aj.value("confidence", 0);
    }

    return f;
}

cc::contracts::CoachFeedback AnalyzerClient::analyze(
    const std::string& code,
    const RunResult&   evalResult,
    const std::string& problemId
) {
    const std::string url = baseUrl_ + "/analyze";

    logging::Logger::info("Calling analyzer service for problem: " + problemId);

    CoachFeedback fallback;
    fallback.nextStep = "No se pudo obtener feedback del analizador.";
    fallback.commonMistake.clear();

    try {
        json body;
        body["code"]      = code;
        body["problemId"] = problemId;
        body["eval"]      = runresult_to_json(evalResult);

        std::string jsonBody = body.dump();
        auto response        = httpClient_.post(url, jsonBody);

        if (!response.isSuccess()) {
            logging::Logger::error("Analyzer service error: HTTP "
                                   + std::to_string(response.statusCode));
            return fallback;
        }

        auto j  = json::parse(response.body);
        auto fb = feedback_from_json(j);

        logging::Logger::info("Analyzer feedback received");
        return fb;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in AnalyzerClient::analyze: "
                               + std::string(e.what()));
        return fallback;
    }
}

} // namespace cc::sdk
