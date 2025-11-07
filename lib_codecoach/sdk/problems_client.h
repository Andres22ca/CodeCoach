//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_PROBLEMS_CLIENT_H
#define LIB_CODECOACH_PROBLEMS_CLIENT_H

#include "contracts/problem_dto.h"
#include "http/http_client.h"
#include <vector>
#include <string>
#include <optional>

namespace cc::sdk {

    class ProblemsClient {
    private:
        http::HttpClient httpClient_;
        std::string baseUrl_;

    public:
        explicit ProblemsClient(const std::string& baseUrl);

        // Listar problemas (con filtro opcional)
        std::vector<contracts::ProblemSummary> list(
            const std::string& category = "",
            const std::string& difficulty = ""
        );

        // Obtener detalle de un problema
        std::optional<contracts::ProblemDetail> get(const std::string& id);

        // Crear nuevo problema (admin)
        std::string create(const contracts::ProblemDetail& problem);

        // Actualizar problema (admin)
        bool update(const std::string& id, const contracts::ProblemDetail& problem);

        // Eliminar problema (admin)
        bool remove(const std::string& id);
    };

} // namespace cc::sdk

#endif //LIB_CODECOACH_PROBLEMS_CLIENT_H