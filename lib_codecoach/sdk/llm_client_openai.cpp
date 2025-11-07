//
// Created by andres on 5/10/25.
//

#include "llm_client_openai.h"
#include "logging/logger.h"

namespace cc::sdk {

OpenAIClient::OpenAIClient(const std::string& apiKey, const std::string& model)
    : apiKey_(apiKey), model_(model) {
    httpClient_.setTimeout(60000); // 60 segundos
    httpClient_.setDefaultHeader("Content-Type", "application/json");
    httpClient_.setDefaultHeader("Authorization", "Bearer " + apiKey);
}

std::string OpenAIClient::complete(const std::string& prompt,
                                   const std::string& systemPrompt) {
    std::string url = "https://api.openai.com/v1/chat/completions";

    logging::Logger::info("Calling OpenAI API with model: " + model_);

    try {
        // NOTA: En producción, construir JSON request con nlohmann/json:
        /*
        json requestBody;
        requestBody["model"] = model_;
        requestBody["temperature"] = 0.7;
        requestBody["max_tokens"] = 1500;

        if (!systemPrompt.empty()) {
            requestBody["messages"][0]["role"] = "system";
            requestBody["messages"][0]["content"] = systemPrompt;
            requestBody["messages"][1]["role"] = "user";
            requestBody["messages"][1]["content"] = prompt;
        } else {
            requestBody["messages"][0]["role"] = "user";
            requestBody["messages"][0]["content"] = prompt;
        }

        std::string jsonBody = requestBody.dump();
        */

        std::string jsonBody = "{}"; // Stub

        auto response = httpClient_.post(url, jsonBody);

        if (!response.isSuccess()) {
            logging::Logger::error("OpenAI API error: " + std::to_string(response.statusCode));
            logging::Logger::error("Response body: " + response.body);
            return "";
        }

        // NOTA: En producción, parsear response JSON:
        /*
        auto responseJson = json::parse(response.body);

        if (responseJson.contains("choices") && !responseJson["choices"].empty()) {
            return responseJson["choices"][0]["message"]["content"];
        }

        logging::Logger::error("Unexpected OpenAI response format");
        return "";
        */

        logging::Logger::info("OpenAI API call successful");
        return ""; // Stub

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in OpenAIClient: " + std::string(e.what()));
        return "";
    }
}

bool OpenAIClient::isAvailable() {
    return !apiKey_.empty();
}

} // namespace cc::sdk