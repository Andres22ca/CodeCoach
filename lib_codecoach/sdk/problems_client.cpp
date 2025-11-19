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
            logging::Logger::error("Failed to fetch problems: "
                                   + std::to_string(response.statusCode));
            return {};
        }

        // STUB
        std::vector<contracts::ProblemSummary> problems;
        logging::Logger::info("Problems fetched successfully (stub)");
        return problems;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::list: "
                               + std::string(e.what()));
        return {};
    }
}

std::optional<contracts::ProblemDetail> ProblemsClient::get(const std::string& id) {
    std::string url = baseUrl_ + "/problems/" + id;

    logging::Logger::debug("Fetching problem detail: " + id);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            // 🔧 FIX: Logger::warning → Logger::warn
            logging::Logger::warn("Problem not found: " + id);
            return std::nullopt;
        }

        // STUB
        logging::Logger::info("Problem detail fetched (stub): " + id);
        return std::nullopt;

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::get: "
                               + std::string(e.what()));
        return std::nullopt;
    }
}

std::string ProblemsClient::create(const contracts::ProblemDetail& problem) {
    std::string url = baseUrl_ + "/problems";

    logging::Logger::info("Creating new problem: " + problem.title);

    try {
        std::string body = "{}"; // Stub

        auto response = httpClient_.post(url, body);

        if (response.isSuccess()) {
            logging::Logger::info("Problem created successfully (stub)");
            return "new-problem-id"; // Stub
        }

        logging::Logger::error("Failed to create problem: "
                               + std::to_string(response.statusCode));
        return "";

    } catch (const std::exception& e) {
        logging::Logger::error("Exception in ProblemsClient::create: "
                               + std::string(e.what()));
        return "";
    }
}

bool ProblemsClient::update(const std::string& id,
                            const contracts::ProblemDetail& problem) {

    // (Opcional) eliminar warning de parámetro sin usar:
    // (void)problem;

    std::string url = baseUrl_ + "/problems/" + id;

    logging::Logger::info("Updating problem: " + id);

    try {
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
        logging::Logger::error("Exception in ProblemsClient::update: "
                               + std::string(e.what()));
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
        logging::Logger::error("Exception in ProblemsClient::remove: "
                               + std::string(e.what()));
        return false;
    }
}

} // namespace cc::sdk
