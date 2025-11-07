//
// Created by andres on 5/10/25.
//

#include "problems_client.h"
#include "logging/logger.h"
#include <sstream>

namespace cc::sdk {

ProblemsClient::ProblemsClient(const std::string& baseUrl)
    : baseUrl_(baseUrl) {
    httpClient_.setTimeout(5000);
    httpClient_.setDefaultHeader("Content-Type", "application/json");
}

std::vector<contracts::ProblemSummary> ProblemsClient::list(
    const std::string& category,
    const std::string& difficulty
) {
    std::string url = baseUrl_ + "/problems";

    // Agregar query params si existen
    bool hasParams = false;
    if (!category.empty()) {
        url += hasParams ? "&" : "?";
        url += "category=" + category;
        hasParams = true;
    }
    if (!difficulty.empty()) {
        url += hasParams ? "&" : "?";
        url += "difficulty=" + difficulty;
    }

    logging::Logger::debug("Fetching problems from: " + url);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            logging::Logger::error("Failed to fetch problems: " + std::to_string(response.statusCode));
            return {};
        }

        // NOTA: En producción, parsear JSON con nlohmann/json
        /*
        auto jsonData = json::parse(response.body);
        std::vector<contracts::ProblemSummary> problems;

        for (const auto& item : jsonData["problems"]) {
            contracts::ProblemSummary p;
            p.id = item["id"];
            p.title = item["title"];
            p.difficulty = item["difficulty"];

            if (item.contains("tags") && item["tags"].is_array()) {
                for (const auto& tag : item["tags"]) {
                    p.tags.push_back(tag);
                }
            }

            problems.push_back(p);
        }

        logging::Logger::info("Fetched " + std::to_string(problems.size()) + " problems");
        return problems;
        */

        std::vector<contracts::ProblemSummary> problems;
        logging::Logger::info("Problems fetched successfully (stub)");
        return problems;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::list: " + std::string(e.what()));
        return {};
    }
}

std::optional<contracts::ProblemDetail> ProblemsClient::get(const std::string& id) {
    std::string url = baseUrl_ + "/problems/" + id;

    logging::Logger::debug("Fetching problem detail: " + id);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            logging::Logger::warning("Problem not found: " + id);
            return std::nullopt;
        }

        // NOTA: En producción, parsear JSON
        /*
        auto jsonData = json::parse(response.body);

        contracts::ProblemDetail problem;
        problem.id = jsonData["id"];
        problem.title = jsonData["title"];
        problem.difficulty = jsonData["difficulty"];
        problem.statement = jsonData["statement"];

        if (jsonData.contains("tags")) {
            for (const auto& tag : jsonData["tags"]) {
                problem.tags.push_back(tag);
            }
        }

        if (jsonData.contains("samples")) {
            for (const auto& sample : jsonData["samples"]) {
                contracts::Sample s;
                s.input = sample["input"];
                s.output = sample["output"];
                problem.samples.push_back(s);
            }
        }

        logging::Logger::info("Problem detail fetched: " + id);
        return problem;
        */

        logging::Logger::info("Problem detail fetched (stub): " + id);
        return std::nullopt; // Stub

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::get: " + std::string(e.what()));
        return std::nullopt;
    }
}

std::string ProblemsClient::create(const contracts::ProblemDetail& problem) {
    std::string url = baseUrl_ + "/problems";

    logging::Logger::info("Creating new problem: " + problem.title);

    try {
        // NOTA: En producción, serializar problem a JSON
        /*
        json jsonBody;
        jsonBody["title"] = problem.title;
        jsonBody["difficulty"] = problem.difficulty;
        jsonBody["statement"] = problem.statement;
        jsonBody["tags"] = problem.tags;

        json samplesJson = json::array();
        for (const auto& sample : problem.samples) {
            samplesJson.push_back({
                {"input", sample.input},
                {"output", sample.output}
            });
        }
        jsonBody["samples"] = samplesJson;

        std::string body = jsonBody.dump();
        */

        std::string body = "{}"; // Stub

        auto response = httpClient_.post(url, body);

        if (response.isSuccess()) {
            // NOTA: En producción, extraer ID del response
            /*
            auto respJson = json::parse(response.body);
            std::string newId = respJson["id"];
            logging::Logger::info("Problem created with ID: " + newId);
            return newId;
            */

            logging::Logger::info("Problem created successfully (stub)");
            return "new-problem-id"; // Stub
        }

        logging::Logger::error("Failed to create problem: " + std::to_string(response.statusCode));
        return "";

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::create: " + std::string(e.what()));
        return "";
    }
}

bool ProblemsClient::update(const std::string& id, const contracts::ProblemDetail& problem) {
    std::string url = baseUrl_ + "/problems/" + id;

    logging::Logger::info("Updating problem: " + id);

    try {
        // NOTA: En producción, serializar problem a JSON
        /*
        json jsonBody;
        jsonBody["title"] = problem.title;
        jsonBody["difficulty"] = problem.difficulty;
        jsonBody["statement"] = problem.statement;
        jsonBody["tags"] = problem.tags;

        json samplesJson = json::array();
        for (const auto& sample : problem.samples) {
            samplesJson.push_back({
                {"input", sample.input},
                {"output", sample.output}
            });
        }
        jsonBody["samples"] = samplesJson;

        std::string body = jsonBody.dump();
        */

        std::string body = "{}"; // Stub

        auto response = httpClient_.put(url, body);
        bool success = response.isSuccess();

        if (success) {
            logging::Logger::info("Problem updated: " + id);
        } else {
            logging::Logger::error("Failed to update problem: " + id +
                                  " (status: " + std::to_string(response.statusCode) + ")");
        }

        return success;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::update: " + std::string(e.what()));
        return false;
    }
}

bool ProblemsClient::remove(const std::string& id) {
    std::string url = baseUrl_ + "/problems/" + id;

    logging::Logger::info("Deleting problem: " + id);

    try {
        auto response = httpClient_.del(url);
        bool success = response.isSuccess();

        if (success) {
            logging::Logger::info("Problem deleted: " + id);
        } else {
            logging::Logger::error("Failed to delete problem: " + id +
                                  " (status: " + std::to_string(response.statusCode) + ")");
        }

        return success;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::remove: " + std::string(e.what()));
        return false;
    }
}

} // namespace cc::sdk