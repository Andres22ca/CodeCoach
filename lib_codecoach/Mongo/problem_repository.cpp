// Created by andres on 5/10/25.
//

#include "Mongo/problem_repository.h"
#include "logging/logger.h"

#include <nlohmann/json.hpp>
#include <exception>

namespace cc::storage {

using nlohmann::json;
using cc::contracts::ProblemDetail;
using cc::contracts::ProblemSummary;
using cc::contracts::Sample;
using cc::logging::Logger;

ProblemRepository::ProblemRepository(MongoClient& client,
                                     std::string collectionName)
    : client_(client),
      collectionName_(std::move(collectionName))
{
}

static json detail_to_json(const ProblemDetail& p) {
    json j;
    j["id"]         = p.id;
    j["title"]      = p.title;
    j["difficulty"] = p.difficulty;
    j["statement"]  = p.statement;

    j["tags"] = json::array();
    for (const auto& t : p.tags) j["tags"].push_back(t);

    j["samples"] = json::array();
    for (const auto& s : p.samples) {
        json sj;
        sj["input"]  = s.input;
        sj["output"] = s.output;
        j["samples"].push_back(std::move(sj));
    }

    return j;
}

static ProblemDetail json_to_detail(const json& j) {
    ProblemDetail d;
    d.id         = j.value("id",         std::string{});
    d.title      = j.value("title",      std::string{});
    d.difficulty = j.value("difficulty", std::string{});
    d.statement  = j.value("statement",  std::string{});

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& t : j["tags"]) {
            d.tags.push_back(t.get<std::string>());
        }
    }

    if (j.contains("samples") && j["samples"].is_array()) {
        for (const auto& sj : j["samples"]) {
            Sample s;
            s.input  = sj.value("input",  std::string{});
            s.output = sj.value("output", std::string{});
            d.samples.push_back(std::move(s));
        }
    }

    return d;
}

static ProblemSummary json_to_summary(const json& j) {
    ProblemSummary s;
    s.id         = j.value("id",         std::string{});
    s.title      = j.value("title",      std::string{});
    s.difficulty = j.value("difficulty", std::string{});

    if (j.contains("tags") && j["tags"].is_array()) {
        for (const auto& t : j["tags"]) {
            s.tags.push_back(t.get<std::string>());
        }
    }

    return s;
}

bool ProblemRepository::createProblem(const ProblemDetail& problem) {
    if (!client_.isConnected() && !client_.connect()) {
        Logger::error("[ProblemRepository] createProblem: Mongo not connected");
        return false;
    }

    try {
        json doc = detail_to_json(problem);
        return client_.insertOne(collectionName_, doc.dump());
    } catch (const std::exception& e) {
        Logger::error(std::string("[ProblemRepository] createProblem exception: ") + e.what());
        return false;
    }
}

bool ProblemRepository::updateProblem(const ProblemDetail& problem) {
    if (!client_.isConnected() && !client_.connect()) {
        Logger::error("[ProblemRepository] updateProblem: Mongo not connected");
        return false;
    }

    try {
        json filter;
        filter["id"] = problem.id;

        json doc = detail_to_json(problem);

        return client_.replaceOne(collectionName_, filter.dump(), doc.dump());
    } catch (const std::exception& e) {
        Logger::error(std::string("[ProblemRepository] updateProblem exception: ") + e.what());
        return false;
    }
}

bool ProblemRepository::deleteProblem(const std::string& id) {
    if (!client_.isConnected() && !client_.connect()) {
        Logger::error("[ProblemRepository] deleteProblem: Mongo not connected");
        return false;
    }

    try {
        json filter;
        filter["id"] = id;
        return client_.deleteOne(collectionName_, filter.dump());
    } catch (const std::exception& e) {
        Logger::error(std::string("[ProblemRepository] deleteProblem exception: ") + e.what());
        return false;
    }
}

std::optional<ProblemDetail>
ProblemRepository::getProblemById(const std::string& id) {
    if (!client_.isConnected() && !client_.connect()) {
        Logger::error("[ProblemRepository] getProblemById: Mongo not connected");
        return std::nullopt;
    }

    try {
        json filter;
        filter["id"] = id;

        auto docStr = client_.findOne(collectionName_, filter.dump());
        if (!docStr) return std::nullopt;

        json j = json::parse(*docStr);
        return json_to_detail(j);
    } catch (const std::exception& e) {
        Logger::error(std::string("[ProblemRepository] getProblemById exception: ") + e.what());
        return std::nullopt;
    }
}

std::vector<ProblemSummary>
ProblemRepository::listProblems(const std::string& difficulty,
                                const std::vector<std::string>& tags) {
    std::vector<ProblemSummary> out;

    if (!client_.isConnected() && !client_.connect()) {
        Logger::error("[ProblemRepository] listProblems: Mongo not connected");
        return out;
    }

    try {
        json filter = json::object();

        if (!difficulty.empty()) {
            filter["difficulty"] = difficulty;
        }

        if (!tags.empty()) {
            // Buscar problemas que contengan TODOS los tags indicados
            filter["tags"]["$all"] = tags;
        }

        auto docs = client_.findMany(collectionName_, filter.dump());
        for (const auto& s : docs) {
            try {
                json j = json::parse(s);
                out.push_back(json_to_summary(j));
            } catch (const std::exception& e) {
                Logger::warn(std::string("[ProblemRepository] listProblems: skipping doc: ") + e.what());
            }
        }

        return out;

    } catch (const std::exception& e) {
        Logger::error(std::string("[ProblemRepository] listProblems exception: ") + e.what());
        return out;
    }
}

} // namespace cc::storage
