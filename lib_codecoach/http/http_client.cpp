//
// Created by andres on 5/10/25.
//

// http_client.cpp — Implementación con soporte de retries/backoff y logging.
// Define CC_USE_CURL para habilitar libcurl; si no, se usa un stub útil para defensa.

// http_client.cpp — Implementación con soporte de retries/backoff y logging.
// Define CC_USE_CURL para habilitar libcurl; si no, se usa un stub útil para defensa.

// http_client.cpp — Implementación con soporte de retries/backoff y logging.
// Define CC_USE_CURL para habilitar libcurl; si no, se usa un stub útil para defensa.

#include "http_client.h"

#include "logging/logger.h"
#include "metrics/timer.h"
#include "config/config_manager.h"

#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <cctype>

#ifdef CC_USE_CURL
  #include <curl/curl.h>
#endif

namespace cc::http {

using cc::logging::Logger;
using cc::logging::Level;
using cc::time::Millis;
using cc::time::Backoff;
using cc::time::BackoffPolicy;

// ---------------------
// Utilidades internas
// ---------------------
static std::string method_upper(std::string m) {
    std::transform(m.begin(), m.end(), m.begin(),
                   [](unsigned char c){ return static_cast<char>(std::toupper(c)); });
    return m;
}

static bool is_retryable_status(int code) {
    // 5xx + 429 + 0 (error de red/timeout)
    return (code >= 500 && code < 600) || code == 429 || code == 0;
}

static std::string short_url(std::string_view url) {
    if (url.size() <= 256) return std::string(url);
    return std::string(url.substr(0, 256)) + "...";
}

// ---------------------
// HttpClient — público
// ---------------------
HttpClient::HttpClient() {
    const auto& cfg = cc::config::get();
    timeoutMs_ = cfg.http.timeoutMs;
    retries_   = std::max(0, cfg.http.retries);
}

void HttpClient::setTimeout(int ms) {
    timeoutMs_ = std::max(1, ms);
}

void HttpClient::setRetries(int n) {
    retries_ = std::max(0, n);
}

void HttpClient::setDefaultHeader(const std::string& key, const std::string& value) {
    defaultHeaders_[key] = value;
}

void HttpClient::clearDefaultHeader(const std::string& key) {
    defaultHeaders_.erase(key);
}

HttpResponse HttpClient::get(const std::string& url)  { return request("GET", url); }
HttpResponse HttpClient::del(const std::string& url)  { return request("DELETE", url); }
HttpResponse HttpClient::post(const std::string& url, const std::string& body) { return request("POST", url, body); }
HttpResponse HttpClient::put(const std::string& url, const std::string& body)  { return request("PUT", url, body); }

// Con retries + backoff
HttpResponse HttpClient::request(const std::string& method,
                                 const std::string& url,
                                 const std::string& body,
                                 const std::unordered_map<std::string, std::string>& headers,
                                 std::optional<int> timeoutMs)
{
    const std::string m = method_upper(method);
    const int tmo = timeoutMs.has_value() ? std::max(1, *timeoutMs) : timeoutMs_;

    BackoffPolicy pol;
    pol.base         = Millis{200};
    pol.factor       = 2.0;
    pol.max_delay    = Millis{3000};
    pol.jitter_pct   = 0.20;
    pol.max_attempts = std::max(1, retries_ + 1); // intentos totales

    Backoff backoff(pol);

    HttpResponse last;
    for (int attempt = 1; attempt <= pol.max_attempts; ++attempt) {
        HttpRequest req;
        req.method    = m;
        req.url       = url;
        req.body      = body;
        req.timeoutMs = tmo;
        req.headers   = defaultHeaders_;
        for (const auto& kv : headers) req.headers[kv.first] = kv.second;

        CC_LOG_DEBUG(std::string("[HTTP] ") + m + " " + short_url(url));
        if (!body.empty() && (m == "POST" || m == "PUT")) {
            CC_LOG_TRACE(std::string("[HTTP] body bytes = ") + std::to_string(body.size()));
        }

        last = do_request_once(req);

        if (last.isSuccess()) {
            CC_LOG_DEBUG(std::string("[HTTP] response ") + std::to_string(last.statusCode));
            return last;
        }

        const bool retryable = is_retryable_status(last.statusCode);
        if (!retryable || attempt == pol.max_attempts) {
            if (!retryable) {
                CC_LOG_WARN(std::string("[HTTP] non-retryable status ") + std::to_string(last.statusCode));
            }
            return last;
        }

        const auto delay = backoff.next_delay();
        CC_LOG_WARN(std::string("[HTTP] failed (status=") + std::to_string(last.statusCode) +
                    ") attempt " + std::to_string(attempt) + "/" + std::to_string(pol.max_attempts) +
                    " — retry in " + std::to_string(delay.count()) + " ms");
        cc::time::sleep_for(delay);
    }
    return last;
}

// ---------------------
// do_request_once()
// ---------------------
HttpResponse HttpClient::do_request_once(const HttpRequest& req) {
#ifdef CC_USE_CURL
    // ---- Implementación real con libcurl ----
    HttpResponse out;

    CURL* curl = curl_easy_init();
    if (!curl) {
        out.statusCode = 0;
        out.body = "curl_easy_init failed";
        return out;
    }

    curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());

    if (req.method == "GET") {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    } else if (req.method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(req.body.size()));
    } else if (req.method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(req.body.size()));
    } else if (req.method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req.method.c_str());
    }

#if defined(CURLOPT_TIMEOUT_MS)
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, req.timeoutMs);
#else
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (req.timeoutMs + 999) / 1000);
#endif

    struct curl_slist* hdrs = nullptr;
    for (const auto& kv : req.headers) {
        std::string line = kv.first + ": " + kv.second;
        hdrs = curl_slist_append(hdrs, line.c_str());
    }
    if (hdrs) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);

    std::string response_body;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
        +[](char* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
            auto* s = static_cast<std::string*>(userdata);
            s->append(ptr, size * nmemb);
            return size * nmemb;
        });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

    std::unordered_map<std::string, std::string> resp_headers;
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION,
        +[](char* buffer, size_t size, size_t nitems, void* userdata) -> size_t {
            size_t total = size * nitems;
            auto* map = static_cast<std::unordered_map<std::string, std::string>*>(userdata);
            std::string line(buffer, total);
            auto pos = line.find(':');
            if (pos != std::string::npos) {
                std::string k = line.substr(0, pos);
                std::string v = line.substr(pos + 1);
                while (!v.empty() && (v.front()==' ' || v.front()=='\t')) v.erase(v.begin());
                while (!v.empty() && (v.back()=='\r' || v.back()=='\n')) v.pop_back();
                while (!k.empty() && (k.back()=='\r' || k.back()=='\n')) k.pop_back();
                if (!k.empty()) (*map)[k] = v;
            }
            return total;
        });
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp_headers);

    CURLcode rc = curl_easy_perform(curl);
    long http_code = 0;
    if (rc == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        out.statusCode = static_cast<int>(http_code);
        out.body       = std::move(response_body);
        out.headers    = std::move(resp_headers);
    } else {
        out.statusCode = 0;
        out.body = std::string("curl error: ") + curl_easy_strerror(rc);
    }

    if (hdrs) curl_slist_free_all(hdrs);
    curl_easy_cleanup(curl);
    return out;

#else
    // ---- Modo STUB (sin libcurl / sin red) ----
    HttpResponse out;
    const std::string& U = req.url;

    // Problems API
    if (U.find("/problems") != std::string::npos) {
        if (req.method == "GET") {
            if (U.find("/problems/") != std::string::npos) {
                out.statusCode = 200;
                out.body =
                    "{"
                      "\"id\":\"two-sum\","
                      "\"title\":\"Two Sum\","
                      "\"difficulty\":\"Easy\","
                      "\"tags\":[\"array\",\"hashmap\"],"
                      "\"statement\":\"Given an array of integers...\","
                      "\"samples\":[{\"input\":\"4\\n2 7 11 15\\n9\",\"output\":\"0 1\"}]"
                    "}";
                return out;
            } else {
                out.statusCode = 200;
                out.body =
                    "{"
                      "\"problems\":["
                        "{\"id\":\"two-sum\",\"title\":\"Two Sum\",\"difficulty\":\"Easy\",\"tags\":[\"array\",\"hashmap\"]},"
                        "{\"id\":\"longest-substring\",\"title\":\"Longest Substring\",\"difficulty\":\"Medium\",\"tags\":[\"string\",\"sliding-window\"]}"
                      "]"
                    "}";
                return out;
            }
        }
        if (req.method == "POST") {
            out.statusCode = 201;
            out.body = "{\"id\":\"new-problem-id\"}";
            return out;
        }
        if (req.method == "PUT") {
            out.statusCode = 200;
            out.body = "{\"ok\":true}";
            return out;
        }
        if (req.method == "DELETE") {
            out.statusCode = 204;
            out.body.clear();
            return out;
        }
    }

    // Eval API
    if (U.find("/evaluate") != std::string::npos && req.method == "POST") {
        out.statusCode = 200;
        out.body =
            "{"
              "\"passed\":false,"
              "\"timeMs\":42,"
              "\"memoryKB\":2560,"
              "\"stdout\":\"OK\","
              "\"stderr\":\"\","
              "\"exitCode\":0,"
              "\"cases\":["
                "{\"input\":\"2 7 11 15\\n9\",\"output\":\"0 1\",\"expected\":\"0 1\",\"passed\":true,\"timeMs\":12,\"memoryKB\":2048},"
                "{\"input\":\"1 2 3\\n7\",\"output\":\"-1\",\"expected\":\"0 2\",\"passed\":false,\"timeMs\":30,\"memoryKB\":1024}"
              "]"
            "}";
        return out;
    }

    // Analyzer API
    if (U.find("/analyze") != std::string::npos && req.method == "POST") {
        out.statusCode = 200;
        out.body =
            "{"
              "\"hints\":["
                "{\"title\":\"Usa un mapa\",\"body\":\"Puedes reducir a O(n)\",\"level\":1},"
                "{\"title\":\"indices correctos\",\"body\":\"Cuida el retorno\",\"level\":1}"
              "],"
              "\"nextStep\":\"Implementa hash map y reintenta\","
              "\"commonMistake\":\"Confundir valor con índice\","
              "\"complexity\":{\"time\":\"O(n)\",\"space\":\"O(n)\"},"
              "\"algorithm\":{\"name\":\"Hash map\",\"confidence\":85}"
            "}";
        return out;
    }

    // Default 404
    out.statusCode = 404;
    out.body = "{\"error\":\"stub: route not found\"}";
    return out;
#endif
}

} // namespace cc::http
