//
// Created by andres on 5/10/25.
//

// http_client.h — Cliente HTTP mínimo para CodeCoach con retries, backoff y logging.
// Si defines CC_USE_CURL, usa libcurl. Si no, entra en modo stub (respuestas simuladas).

// http_client.h — Cliente HTTP mínimo para CodeCoach con retries, backoff y logging.
// Si defines CC_USE_CURL, usa libcurl. Si no, entra en modo stub (respuestas simuladas).

// http_client.h — Cliente HTTP para CodeCoach con retries, backoff y logging.
// Usa http_response.h para el tipo de respuesta.

#ifndef LIB_CODECOACH_HTTP_CLIENT_H
#define LIB_CODECOACH_HTTP_CLIENT_H

#include "http_response.h"  // <<<<<<  centralizamos aquí la definición de HttpResponse

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

namespace cc::http {

    struct HttpRequest {
        std::string method; // "GET", "POST", "PUT", "DELETE"
        std::string url;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        int timeoutMs{5000};
    };

    class HttpClient {
    public:
        HttpClient();

        // Config
        void setTimeout(int ms);
        void setRetries(int n);
        void setDefaultHeader(const std::string& key, const std::string& value);
        void clearDefaultHeader(const std::string& key);

        // Atajos
        HttpResponse get (const std::string& url);
        HttpResponse del (const std::string& url);
        HttpResponse post(const std::string& url, const std::string& body);
        HttpResponse put (const std::string& url, const std::string& body);

        // General
        HttpResponse request(const std::string& method,
                             const std::string& url,
                             const std::string& body = "",
                             const std::unordered_map<std::string, std::string>& headers = {},
                             std::optional<int> timeoutMs = std::nullopt);

    private:
        HttpResponse do_request_once(const HttpRequest& req); // 1 intento (sin retries)

        int timeoutMs_{5000};
        int retries_{1}; // reintentos adicionales (además del intento inicial)
        std::unordered_map<std::string, std::string> defaultHeaders_;
    };

} // namespace cc::http

#endif // LIB_CODECOACH_HTTP_CLIENT_H
