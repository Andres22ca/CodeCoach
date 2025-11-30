
#include "eval_client.h"
#include "logging/logger.h"

#include <exception>
#include <nlohmann/json.hpp>

namespace cc::sdk {

using nlohmann::json;
using cc::contracts::RunRequest;
using cc::contracts::RunResult;
using cc::contracts::RunCaseResult;

EvalClient::EvalClient(const std::string& baseUrl)
    : baseUrl_(baseUrl)
{
    httpClient_.setTimeout(30000); // 30 segundos para evaluación
    httpClient_.setDefaultHeader("Content-Type", "application/json");
}

static json to_json(const RunRequest& req) {
    json j;
    j["code"]      = req.code;
    j["problemId"] = req.problemId;
    j["stdin"]     = req.stdin;
    return j;
}

static RunResult runresult_from_json(const json& j) {
    RunResult result;

    result.passed   = j.value("passed", false);
    result.timeMs   = j.value("timeMs", 0);
    result.memoryKB = j.value("memoryKB", 0);
    result.exitCode = j.value("exitCode", 0);
    result.stdout   = j.value("stdout", std::string{});
    result.stderr   = j.value("stderr", std::string{});

    if (j.contains("cases") && j["cases"].is_array()) {
        for (const auto& cj : j["cases"]) {
            RunCaseResult c;
            c.input    = cj.value("input",    std::string{});
            c.output   = cj.value("output",   std::string{});
            c.expected = cj.value("expected", std::string{});
            c.passed   = cj.value("passed",   false);
            c.timeMs   = cj.value("timeMs",   0);
            c.memoryKB = cj.value("memoryKB", 0);
            result.cases.push_back(std::move(c));
        }
    }

    return result;
}

RunResult EvalClient::submit(const RunRequest& request) {
    const std::string url = baseUrl_ + "/evaluate";

    logging::Logger::info("Submitting code for evaluation");

    RunResult fallback;
    fallback.passed   = false;
    fallback.timeMs   = 0;
    fallback.memoryKB = 0;
    fallback.exitCode = -1;

    try {
        json requestBody = to_json(request);
        std::string jsonBody = requestBody.dump();

        auto response = httpClient_.post(url, jsonBody);

        if (!response.isSuccess()) {
            logging::Logger::error(
                "Evaluation failed: HTTP " + std::to_string(response.statusCode)
            );
            fallback.stderr =
                "Evaluation service error: HTTP " + std::to_string(response.statusCode);
            return fallback;
        }

        auto responseJson = json::parse(response.body);
        auto result       = runresult_from_json(responseJson);

        logging::Logger::info("Code evaluated successfully");
        return result;

    } catch (const std::exception& e) {
        logging::Logger::error(
            "Exception in EvalClient::submit: " + std::string(e.what())
        );
        fallback.stderr = "Exception: " + std::string(e.what());
        return fallback;
    }
}

std::optional<RunResult>
EvalClient::getResult(const std::string& submissionId) {
    const std::string url = baseUrl_ + "/results/" + submissionId;

    logging::Logger::debug("Fetching evaluation result: " + submissionId);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            logging::Logger::warn("Result not found or error: HTTP "
                                  + std::to_string(response.statusCode));
            return std::nullopt;
        }

        auto j = json::parse(response.body);
        RunResult result = runresult_from_json(j);

        logging::Logger::info("Result fetched successfully");
        return result;

    } catch (const std::exception& e) {
        logging::Logger::error(
            "Exception in EvalClient::getResult: " + std::string(e.what())
        );
        return std::nullopt;
    }
}

} // namespace cc::sdk
