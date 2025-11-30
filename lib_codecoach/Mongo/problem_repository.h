// ProblemRepository — capa de acceso a datos para problemas de CodeCoach.
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_PROBLEM_REPOSITORY_H
#define LIB_CODECOACH_PROBLEM_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>

#include "contracts/problem_dto.h"
#include "Mongo/mongo_client.h"

namespace cc::storage {

    class ProblemRepository {
    public:
        ProblemRepository(MongoClient& client,
                          std::string collectionName = "problems");

        bool createProblem(const cc::contracts::ProblemDetail& problem);
        bool updateProblem(const cc::contracts::ProblemDetail& problem);
        bool deleteProblem(const std::string& id);

        std::optional<cc::contracts::ProblemDetail>
        getProblemById(const std::string& id);

        std::vector<cc::contracts::ProblemSummary>
        listProblems(const std::string& difficulty = "",
                     const std::vector<std::string>& tags = {});

    private:
        MongoClient& client_;
        std::string  collectionName_;
    };

} // namespace cc::storage

#endif // LIB_CODECOACH_PROBLEM_REPOSITORY_H
