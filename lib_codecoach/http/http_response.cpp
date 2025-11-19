//
// Created by andres on 5/10/25.
//

// http_response.cpp — Implementación de helpers para HttpResponse.

#include "http_response.h"

#include <algorithm>
#include <sstream>
#include <cctype>

namespace cc::http {

const char* reason_phrase(int status) noexcept {
    switch (status) {
        case 0:   return "Network Error";
        case 100: return "Continue";
        case 101: return "Switching Protocols";
        case 102: return "Processing";
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 204: return "No Content";
        case 206: return "Partial Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 307: return "Temporary Redirect";
        case 308: return "Permanent Redirect";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 409: return "Conflict";
        case 413: return "Payload Too Large";
        case 415: return "Unsupported Media Type";
        case 418: return "I'm a teapot";
        case 422: return "Unprocessable Entity";
        case 429: return "Too Many Requests";
        case 499: return "Client Closed Request";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        default:  return "";
    }
}

static inline char to_lower(unsigned char c) { return static_cast<char>(std::tolower(c)); }

std::optional<std::string> get_header_ci(
    const std::unordered_map<std::string, std::string>& headers,
    std::string_view key)
{
    // Intento rápido: exacto
    if (auto it = headers.find(std::string(key)); it != headers.end())
        return it->second;

    // Búsqueda CI mediante escaneo
    std::string key_lc;
    key_lc.resize(key.size());
    std::transform(key.begin(), key.end(), key_lc.begin(), to_lower);

    for (const auto& kv : headers) {
        std::string k = kv.first;
        std::transform(k.begin(), k.end(), k.begin(), to_lower);
        if (k == key_lc) return kv.second;
    }
    return std::nullopt;
}

std::string summarize(
    int status,
    std::string_view body,
    const std::unordered_map<std::string, std::string>& headers,
    std::size_t maxBody)
{
    std::ostringstream oss;
    oss << status;
    if (const char* r = reason_phrase(status); r && *r) {
        oss << ' ' << r;
    }

    if (auto ct = get_header_ci(headers, "Content-Type"))   oss << " | Content-Type: "   << *ct;
    if (auto cl = get_header_ci(headers, "Content-Length")) oss << " | Content-Length: " << *cl;

    oss << " | Body(" << body.size() << "B): ";
    if (body.size() <= maxBody) {
        oss << std::string(body);
    } else {
        oss << std::string(body.substr(0, maxBody)) << "...";
    }
    return oss.str();
}

std::string summarize(const HttpResponse& resp, std::size_t maxBody) {
    return summarize(resp.statusCode, resp.body, resp.headers, maxBody);
}

} // namespace cc::http
