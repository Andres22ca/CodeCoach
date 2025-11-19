//
// Created by andres on 5/10/25.
//

// http_response.h — Tipo de respuesta HTTP + helpers (reason phrase, headers CI, resumen).
#ifndef LIB_CODECOACH_HTTP_RESPONSE_H
#define LIB_CODECOACH_HTTP_RESPONSE_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <cstddef> // size_t

namespace cc::http {

    struct HttpResponse {
        int statusCode{0};
        std::string body;
        std::unordered_map<std::string, std::string> headers;

        bool isSuccess() const noexcept { return statusCode >= 200 && statusCode < 300; }
    };

    // Reason phrase típico (200 -> "OK", 404 -> "Not Found", 0 -> "Network Error", etc.)
    const char* reason_phrase(int status) noexcept;

    // Helpers por código de estado
    inline bool is_success(int s)       noexcept { return s >= 200 && s < 300; }
    inline bool is_redirect(int s)      noexcept { return s >= 300 && s < 400; }
    inline bool is_client_error(int s)  noexcept { return s >= 400 && s < 500; }
    inline bool is_server_error(int s)  noexcept { return s >= 500 && s < 600; }
    inline bool is_network_error(int s) noexcept { return s == 0; } // timeout / fallo de red

    // Obtiene un header (case-insensitive). Devuelve nullopt si no existe.
    std::optional<std::string> get_header_ci(
        const std::unordered_map<std::string, std::string>& headers,
        std::string_view key);

    // Resumen legible (status + reason + algunos headers + body truncado a maxBody bytes).
    std::string summarize(
        int status,
        std::string_view body,
        const std::unordered_map<std::string, std::string>& headers,
        std::size_t maxBody = 512);

    // Overload conveniente para HttpResponse
    std::string summarize(const HttpResponse& resp, std::size_t maxBody = 512);

} // namespace cc::http

#endif // LIB_CODECOACH_HTTP_RESPONSE_H
