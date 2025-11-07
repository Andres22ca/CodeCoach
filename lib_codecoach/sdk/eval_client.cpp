//
// Created by andres on 5/10/25.
//

#include "eval_client.h"
#include "logging/logger.h"

namespace cc::sdk {

EvalClient::EvalClient(const std::string& baseUrl)
    : baseUrl_(baseUrl) {
    httpClient_.setTimeout(30000); // 30 segundos para evaluación
    httpClient_.setDefaultHeader("Content-Type", "application/json");
}

contracts::RunResult EvalClient::submit(const contracts::RunRequest& request) {
    std::string url = baseUrl_ + "/evaluate";

    logging::Logger::info("Submitting code for evaluation");

    contracts::RunResult result;
    result.passed = false;
    result.timeMs = 0;
    result.memoryKB = 0;
    result.exitCode = -1;

    try {
        // NOTA: En producción, serializar request a JSON con nlohmann/json
        // json requestBody;
        // requestBody["code"] = request.code;
        // requestBody["problemId"] = request.problemId;
        // requestBody["stdin"] = request.stdin;
        // std::string jsonBody = requestBody.dump();

        std::string jsonBody = "{}"; // Stub

        auto response = httpClient_.post(url, jsonBody);

        if (!response.isSuccess()) {
            logging::Logger::error("Evaluation failed: " + std::to_string(response.statusCode));
            result.stderr = "Evaluation service error: " + std::to_string(response.statusCode);
            return result;
        }

        // NOTA: En producción, parsear response.body como JSON
        // auto responseJson = json::parse(response.body);
        // result.passed = responseJson["passed"];
        // result.timeMs = responseJson["timeMs"];
        // result.memoryKB = responseJson["memoryKB"];
        // result.exitCode = responseJson["exitCode"];
        // result.stdout = responseJson["stdout"];
        // result.stderr = responseJson["stderr"];
        //
        // for (const auto& caseJson : responseJson["cases"]) {
        //     contracts::RunCaseResult caseResult;
        //     caseResult.input = caseJson["input"];
        //     caseResult.output = caseJson["output"];
        //     caseResult.expected = caseJson["expected"];
        //     caseResult.passed = caseJson["passed"];
        //     caseResult.timeMs = caseJson["timeMs"];
        //     caseResult.memoryKB = caseJson["memoryKB"];
        //     result.cases.push_back(caseResult);
        // }

        logging::Logger::info("Code evaluated successfully");

        return result;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in EvalClient::submit: " + std::string(e.what()));
        result.stderr = "Exception: " + std::string(e.what());
        return result;
    }
}

std::optional<contracts::RunResult> EvalClient::getResult(const std::string& submissionId) {
    std::string url = baseUrl_ + "/results/" + submissionId;

    logging::Logger::debug("Fetching evaluation result: " + submissionId);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            logging::Logger::warning("Result not found: " + submissionId);
            return std::nullopt;
        }

        // NOTA: En producción, parsear JSON y retornar RunResult
        // auto responseJson = json::parse(response.body);
        // contracts::RunResult result;
        // // ... llenar campos desde responseJson
        // return result;

        logging::Logger::info("Result fetched: " + submissionId);
        return std::nullopt; // Stub

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in EvalClient::getResult: " + std::string(e.what()));
        return std::nullopt;
    }
}

} // namespace cc::sdk