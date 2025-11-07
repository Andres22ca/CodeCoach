#include "analyzer_client.h"
#include "logging/logger.h"

namespace cc::sdk {

    AnalyzerClient::AnalyzerClient(const std::string& baseUrl)
        : baseUrl_(baseUrl) {
        httpClient_.setTimeout(60000); // 60 segundos para el LLM
        httpClient_.setDefaultHeader("Content-Type", "application/json");
    }

    contracts::CoachFeedback AnalyzerClient::analyze(
        const std::string& code,
        const contracts::RunResult& evalResult,
        const std::string& problemId
    ) {
        std::string url = baseUrl_ + "/analyze";

        logging::Logger::info("Requesting code analysis");

        contracts::CoachFeedback feedback;
        feedback.algorithm.name = "Unknown";
        feedback.algorithm.confidence = 0;
        feedback.complexity.time = "O(?)";
        feedback.complexity.space = "O(?)";
        feedback.nextStep = "Analysis unavailable";
        feedback.commonMistake = "";

        try {
            // NOTA: En producción, crear JSON request con:
            // - code
            // - evalResult (serializado)
            // - problemId
            std::string jsonBody = "{}"; // Stub

            auto response = httpClient_.post(url, jsonBody);

            if (!response.isSuccess()) {
                logging::Logger::error("Analysis failed: " + std::to_string(response.statusCode));
                feedback.nextStep = "Analysis service error";
                return feedback;
            }

            // NOTA: En producción, parsear response como JSON y llenar feedback
            logging::Logger::info("Code analyzed successfully");

            return feedback;

        } catch (const std::exception& e) {
            logging::Logger::error("Exception in AnalyzerClient::analyze: " + std::string(e.what()));
            feedback.nextStep = "Exception during analysis";
            return feedback;
        }
    }

} // namespace cc::sdk