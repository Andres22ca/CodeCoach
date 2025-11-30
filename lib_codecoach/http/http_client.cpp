//
// Created by andres on 5/10/25.
//

#include "http_client.h"

#include "logging/logger.h"
#include "metrics/timer.h"
#include "config/config_manager.h"

#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>
#include <cctype>
#include <string_view>

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

#ifdef CC_USE_CURL
// --- Callbacks para libcurl --- //
static size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* s = static_cast<std::string*>(userdata);
    s->append(ptr, size * nmemb);
    return size * nmemb;
}

static size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
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
}
#endif // CC_USE_CURL

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

// ---------------------
// request() con retries
// ---------------------
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
#ifndef CC_USE_CURL
    HttpResponse out;
    out.statusCode = 0;
    out.body = "ERROR: CC_USE_CURL no está definido y no hay stub. "
               "Debe compilar con libcurl o definir CC_USE_CURL.";
    return out;
#else
    HttpResponse out;

    CURL* curl = curl_easy_init();
    if (!curl) {
        out.statusCode = 0;
        out.body = "curl_easy_init failed";
        return out;
    }

    // URL
    curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());

    // Método HTTP
    if (req.method == "GET") {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    } else if (req.method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                         static_cast<long>(req.body.size()));
    } else if (req.method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,
                         static_cast<long>(req.body.size()));
    } else if (req.method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req.method.c_str());
    }

    // Timeout
#if defined(CURLOPT_TIMEOUT_MS)
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, req.timeoutMs);
#else
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (req.timeoutMs + 999) / 1000);
#endif

    // Headers de request
    struct curl_slist* hdrs = nullptr;
    for (const auto& kv : req.headers) {
        std::string line = kv.first + ": " + kv.second;
        hdrs = curl_slist_append(hdrs, line.c_str());
    }
    if (hdrs) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
    }

    // Callbacks de respuesta
    std::string response_body;
    std::unordered_map<std::string, std::string> resp_headers;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &resp_headers);

    // Ejecutar
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
#endif // CC_USE_CURL
}

} // namespace cc::http