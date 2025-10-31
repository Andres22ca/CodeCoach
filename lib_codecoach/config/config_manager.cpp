//
// Created by andres on 5/10/25.
//

#include "config_manager.h"
#include "errors/exceptions.h"

#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>

namespace cc::config {

// Config de prueba (override)
static const Config* g_test_cfg = nullptr;

// Helpers internos
static std::optional<std::string> opt_getenv(const char* key) {
    const char* val = std::getenv(key);
    if (!val) return std::nullopt;
    return std::string(val);
}

static bool looks_like_url(const std::string& s) {
    return s.rfind("http://", 0) == 0 || s.rfind("https://", 0) == 0;
}

static int parse_int_or_throw(const std::string& raw, int min, int max, const char* key) {
    try {
        int v = std::stoi(raw);
        if (v < min || v > max)
            throw cc::errors::ConfigError(std::string(key) + " fuera de rango (" +
                                          std::to_string(min) + ".." + std::to_string(max) + "): " + raw);
        return v;
    } catch (...) {
        throw cc::errors::ConfigError(std::string("Valor inválido para ") + key + ": " + raw);
    }
}

// Construye Config desde entorno + defaults
static Config make_from_env_or_defaults() {
    Config cfg;

    // Defaults
    cfg.endpoints.problems = "http://127.0.0.1:8081";
    cfg.endpoints.eval     = "http://127.0.0.1:8082";
    cfg.endpoints.analyzer = "http://127.0.0.1:8083";
    cfg.http.timeoutMs     = 5000;
    cfg.http.retries       = 1;

    // Overrides por entorno
    if (auto v = opt_getenv("BASE_URL_PROBLEMS")) cfg.endpoints.problems = *v;
    if (auto v = opt_getenv("BASE_URL_EVAL"))     cfg.endpoints.eval = *v;
    if (auto v = opt_getenv("BASE_URL_ANALYZER")) cfg.endpoints.analyzer = *v;

    if (auto v = opt_getenv("HTTP_TIMEOUT_MS"))
        cfg.http.timeoutMs = parse_int_or_throw(*v, 1, 600000, "HTTP_TIMEOUT_MS");
    if (auto v = opt_getenv("HTTP_RETRIES"))
        cfg.http.retries = parse_int_or_throw(*v, 0, 5, "HTTP_RETRIES");

    // Validaciones
    if (!looks_like_url(cfg.endpoints.problems))
        throw cc::errors::ConfigError("BASE_URL_PROBLEMS inválida: " + cfg.endpoints.problems);
    if (!looks_like_url(cfg.endpoints.eval))
        throw cc::errors::ConfigError("BASE_URL_EVAL inválida: " + cfg.endpoints.eval);
    if (!looks_like_url(cfg.endpoints.analyzer))
        throw cc::errors::ConfigError("BASE_URL_ANALYZER inválida: " + cfg.endpoints.analyzer);

    return cfg;
}

// --- API pública ---

const Config& get() {
    if (g_test_cfg) return *g_test_cfg;
    static Config cfg = make_from_env_or_defaults();
    return cfg;
}

void set_for_tests(const Config& c) {
    static Config testCfg;
    testCfg = c;
    g_test_cfg = &testCfg;
}

void reload() {
    if (g_test_cfg) return; // no recargar en modo test
    // Rehacer la cache
    static Config cfg = make_from_env_or_defaults();
    cfg = make_from_env_or_defaults(); // reconstruye con nuevos valores del entorno
}

} // namespace cc::config
