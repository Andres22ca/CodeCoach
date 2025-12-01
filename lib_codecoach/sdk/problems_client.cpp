// Created by andres on 5/10/25.
//

#include "problems_client.h"
#include "logging/logger.h"

#include <nlohmann/json.hpp>
#include <sstream>

namespace cc::sdk {

using cc::logging::Logger;
using nlohmann::json;

// Helpers internos para convertir entre JSON y contracts
namespace {

json to_json(const cc::contracts::ProblemDetail& p) {
    json j;
    j["id"]         = p.id;
    j["title"]      = p.title;
    j["difficulty"] = p.difficulty;
    j["statement"]  = p.statement;

    // tags: vector<string>
    j["tags"] = json::array();
    for (const auto& t : p.tags) {
        j["tags"].push_back(t);
    }

    // samples: vector<Sample> { input, output }
    j["samples"] = json::array();
    for (const auto& s : p.samples) {
        json js;
        js["input"]  = s.input;
        js["output"] = s.output;
        j["samples"].push_back(js);
    }

    return j;
}

cc::contracts::ProblemSummary
from_json_summary(const json& j) {
    cc::contracts::ProblemSummary p;
    p.id         = j.value("id", "");
    p.title      = j.value("title", "");
    p.difficulty = j.value("difficulty", "");

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& t : j["tags"]) {
            if (t.is_string()) {
                p.tags.push_back(t.get<std::string>());
            }
        }
    }
    return p;
}

cc::contracts::ProblemDetail
from_json_detail(const json& j) {
    cc::contracts::ProblemDetail p;

    // Campos heredados de ProblemSummary
    p.id         = j.value("id", "");
    p.title      = j.value("title", "");
    p.difficulty = j.value("difficulty", "");

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& t : j["tags"]) {
            if (t.is_string()) {
                p.tags.push_back(t.get<std::string>());
            }
        }
    }

    // Campos propios de ProblemDetail
    p.statement = j.value("statement", "");

    if (j.contains("samples") && j["samples"].is_array()) {
        for (const auto& js : j["samples"]) {
            cc::contracts::Sample s;
            s.input  = js.value("input", "");
            s.output = js.value("output", "");
            p.samples.push_back(std::move(s));
        }
    }

    return p;
}

} // namespace (helpers anónimos)

// ==========================
// Implementación pública
// ==========================

ProblemsClient::ProblemsClient(const std::string& baseUrl)
    : baseUrl_(baseUrl)
{
    httpClient_.setTimeout(5000);
    httpClient_.setDefaultHeader("Content-Type", "application/json");
}

std::vector<cc::contracts::ProblemSummary>
ProblemsClient::list(const std::string& category,
                     const std::string& difficulty)
{
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

    Logger::debug("Fetching problems from: " + url);

    std::vector<cc::contracts::ProblemSummary> problems;

    try {
        auto response = httpClient_.get(url);

        if (!response.isSuccess()) {
            Logger::error("Failed to fetch problems: HTTP "
                          + std::to_string(response.statusCode));
            return problems;
        }

        json arr = json::parse(response.body);

        if (!arr.is_array()) {
            Logger::error("ProblemsClient::list — response is not an array");
            return problems;
        }

        for (const auto& item : arr) {
            problems.push_back(from_json_summary(item));
        }

        Logger::info("Problems fetched successfully: "
                     + std::to_string(problems.size()));
        return problems;

    } catch (const std::exception& e) {
        Logger::error(std::string("Exception in ProblemsClient::list: ")
                      + e.what());
        return {};
    }
}

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

        json j = json::parse(response.body);
        auto detail = from_json_detail(j);

        Logger::info("Problem detail fetched: " + id);
        return detail;

    } catch (const std::exception& e) {
        Logger::error(std::string("Exception in ProblemsClient::get: ")
                      + e.what());
        return std::nullopt;
    }
}

std::string ProblemsClient::create(const cc::contracts::ProblemDetail& problem) {
    std::string url = baseUrl_ + "/problems";

    Logger::info("Creating new problem: " + problem.title);

    try {
        json bodyJson = to_json(problem);
        std::string body = bodyJson.dump();

        auto response = httpClient_.post(url, body);

        if (!response.isSuccess()) {
            Logger::error("Failed to create problem: HTTP "
                          + std::to_string(response.statusCode));
            return "";
        }

        // Se espera algo como { "id": "nuevo-id" }
        std::string newId;
        try {
            json resp = json::parse(response.body);
            newId = resp.value("id", "");
        } catch (...) {
            // Si el servidor no devuelve JSON, igual damos un id vacío
            Logger::warn("ProblemsClient::create — response body is not JSON");
        }

        Logger::info("Problem created successfully, id = " + newId);
        return newId;

    } catch (const std::exception& e) {
        Logger::error(std::string("Exception in ProblemsClient::create: ")
                      + e.what());
        return "";
    }
}

bool ProblemsClient::update(const std::string& id,
                            const cc::contracts::ProblemDetail& problem)
{
    std::string url = baseUrl_ + "/problems/" + id;

    Logger::info("Updating problem: " + id);

    try {
        json bodyJson = to_json(problem);
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
        Logger::error(std::string("Exception in ProblemsClient::update: ")
                      + e.what());
        return false;
    }
}

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
        Logger::error(std::string("Exception in ProblemsClient::remove: ")
                      + e.what());
        return false;
    }
}

} // namespace cc::sdk
