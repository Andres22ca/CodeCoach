// sdk/problems_client.cpp
//
// Cliente REST para el microservicio de problemas de CodeCoach.
//

#include "sdk/problems_client.h"
#include "logging/logger.h"

#include <sstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace cc::sdk {

using cc::logging::Logger;
using json = nlohmann::json;

ProblemsClient::ProblemsClient(const std::string& baseUrl)
    : baseUrl_(baseUrl)
{
    httpClient_.setTimeout(5000);
    httpClient_.setDefaultHeader("Content-Type", "application/json");
}

// ---------------------------------------------------------
// Helpers locales para mapear JSON <-> DTOs
// ---------------------------------------------------------

static cc::contracts::ProblemSummary from_json_summary(const json& j) {
    cc::contracts::ProblemSummary s;
    s.id         = j.value("id", "");
    s.title      = j.value("title", "");
    s.difficulty = j.value("difficulty", "");

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& t : j["tags"]) {
            if (t.is_string()) {
                s.tags.push_back(t.get<std::string>());
            }
        }
    }
    return s;
}

static cc::contracts::ProblemDetail from_json_detail(const json& j) {
    cc::contracts::ProblemDetail d;
    d.id         = j.value("id", "");
    d.title      = j.value("title", "");
    d.difficulty = j.value("difficulty", "");
    d.statement  = j.value("statement", "");
    //d.statementHtml = j.value("statementHtml", "");

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& t : j["tags"]) {
            if (t.is_string()) {
                d.tags.push_back(t.get<std::string>());
            }
        }
    }

    // Si tu DTO tiene más campos (constraints, inputFormat, etc),
    // puedes mapearlos aquí de forma similar con j.value("campo", "").

    return d;
}

static json to_json_detail(const cc::contracts::ProblemDetail& p) {
    json j;
    j["id"]            = p.id;
    j["title"]         = p.title;
    j["difficulty"]    = p.difficulty;
    j["statement"]     = p.statement;
 //   j["statementHtml"] = p.statementHtml;
    j["tags"]          = p.tags;

    // Igual que arriba: si el DTO tiene más campos, agrégalos:
    // j["constraints"] = p.constraints;
    // ...

    return j;
}

// ---------------------------------------------------------
// list()
// ---------------------------------------------------------

std::vector<cc::contracts::ProblemSummary> ProblemsClient::list(
    const std::string& category,
    const std::string& difficulty
) {
    std::string url = baseUrl_ + "/problems";

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

    Logger::debug("Fetching problems from: " + url);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            Logger::error("Failed to fetch problems: HTTP "
                          + std::to_string(response.statusCode));
            return {};
        }

        std::vector<cc::contracts::ProblemSummary> problems;

        try {
            auto j = json::parse(response.body);

            // Puede ser un array directo: [ {...}, {...} ]
            if (j.is_array()) {
                for (const auto& item : j) {
                    if (item.is_object()) {
                        problems.push_back(from_json_summary(item));
                    }
                }
            }
            // O envuelto: { "items": [ ... ] }
            else if (j.is_object() && j.contains("items") && j["items"].is_array()) {
                for (const auto& item : j["items"]) {
                    if (item.is_object()) {
                        problems.push_back(from_json_summary(item));
                    }
                }
            } else {
                Logger::warn("Unexpected JSON format in ProblemsClient::list");
            }

        } catch (const json::parse_error& e) {
            Logger::error(std::string("JSON parse error in ProblemsClient::list: ") + e.what());
            return {};
        }

        Logger::info("Problems fetched successfully: " + std::to_string(problems.size()));
        return problems;

    } catch (const std::exception& e) {
        Logger::error("Exception in ProblemsClient::list: " + std::string(e.what()));
        return {};
    }
}

// ---------------------------------------------------------
// get(id)
// ---------------------------------------------------------

std::optional<cc::contracts::ProblemDetail>
ProblemsClient::get(const std::string& id) {
    std::string url = baseUrl_ + "/problems/" + id;

    Logger::debug("Fetching problem detail: " + id);

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            Logger::warn("Problem not found or HTTP error: " + id +
                         " (status " + std::to_string(response.statusCode) + ")");
            return std::nullopt;
        }

        try {
            auto j = json::parse(response.body);
            if (!j.is_object()) {
                Logger::warn("Unexpected JSON format in ProblemsClient::get");
                return std::nullopt;
            }
            auto d = from_json_detail(j);
            Logger::info("Problem detail fetched: " + d.id);
            return d;
        } catch (const json::parse_error& e) {
            Logger::error(std::string("JSON parse error in ProblemsClient::get: ") + e.what());
            return std::nullopt;
        }

    } catch (const std::exception& e) {
        Logger::error("Exception in ProblemsClient::get: " + std::string(e.what()));
        return std::nullopt;
    }
}

// ---------------------------------------------------------
// create()
// ---------------------------------------------------------

std::string ProblemsClient::create(const cc::contracts::ProblemDetail& problem) {
    std::string url = baseUrl_ + "/problems";

    Logger::info("Creating new problem: " + problem.title);

    try {
        json bodyJson = to_json_detail(problem);
        std::string body = bodyJson.dump();

        auto response = httpClient_.post(url, body);

        if (!response.isSuccess()) {
            Logger::error("Failed to create problem: HTTP "
                          + std::to_string(response.statusCode));
            return "";
        }

        try {
            auto j = json::parse(response.body);

            // Muchas APIs devuelven { "id": "..." }
            if (j.is_object() && j.contains("id") && j["id"].is_string()) {
                std::string newId = j["id"].get<std::string>();
                Logger::info("Problem created successfully with id=" + newId);
                return newId;
            }

            // o devuelven el problema completo
            if (j.is_object() && j.contains("id")) {
                std::string newId = j.value("id", "");
                Logger::info("Problem created (full object) with id=" + newId);
                return newId;
            }

            Logger::warn("Problem created but couldn't parse id from response");
            return "";

        } catch (const json::parse_error& e) {
            Logger::error(std::string("JSON parse error in ProblemsClient::create: ") + e.what());
            return "";
        }

    } catch (const std::exception& e) {
        Logger::error("Exception in ProblemsClient::create: " + std::string(e.what()));
        return "";
    }
}

// ---------------------------------------------------------
// update(id, problem)
// ---------------------------------------------------------

bool ProblemsClient::update(const std::string& id,
                            const cc::contracts::ProblemDetail& problem) {

    std::string url = baseUrl_ + "/problems/" + id;

    Logger::info("Updating problem: " + id);

    try {
        json bodyJson = to_json_detail(problem);
        std::string body = bodyJson.dump();

        auto response = httpClient_.put(url, body);
        bool success = response.isSuccess();

        if (success) {
            Logger::info("Problem updated: " + id);
        } else {
            Logger::error("Failed to update problem: " + id +
                          " (status: " + std::to_string(response.statusCode) + ")");
        }

        return success;

    } catch (const std::exception& e) {
        Logger::error("Exception in ProblemsClient::update: " + std::string(e.what()));
        return false;
    }
}

// ---------------------------------------------------------
// remove(id)
// ---------------------------------------------------------

bool ProblemsClient::remove(const std::string& id) {
    std::string url = baseUrl_ + "/problems/" + id;

    Logger::info("Deleting problem: " + id);

    try {
        auto response = httpClient_.del(url);
        bool success = response.isSuccess();

        if (success) {
            Logger::info("Problem deleted: " + id);
        } else {
            Logger::error("Failed to delete problem: " + id +
                          " (status: " + std::to_string(response.statusCode) + ")");
        }

        return success;

    } catch (const std::exception& e) {
        Logger::error("Exception in ProblemsClient::remove: " + std::string(e.what()));
        return false;
    }
}

} // namespace cc::sdk
