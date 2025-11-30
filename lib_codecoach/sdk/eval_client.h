#ifndef LIB_CODECOACH_EVAL_CLIENT_H
#define LIB_CODECOACH_EVAL_CLIENT_H

#include "contracts/eval_dto.h"
#include "http/http_client.h"

#include <string>
#include <optional>

namespace cc::sdk {

    class EvalClient {
    private:
        http::HttpClient httpClient_;
        std::string      baseUrl_;

    public:
        explicit EvalClient(const std::string& baseUrl);

        // Enviar código para resultado
        cc::contracts::RunResult submit(const cc::contracts::RunRequest& request);

        // Obtener resultado
        std::optional<cc::contracts::RunResult> getResult(const std::string& submissionId);
    };

} // namespace cc::sdk

#endif // LIB_CODECOACH_EVAL_CLIENT_H
