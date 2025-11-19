// Created by andres on 5/10/25.
//

#include "config_manager.h"
#include "errors/exceptions.h"

#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <string>
#include <mutex>

namespace cc::config {

// =============================
// Estado interno (una sola caché)
// =============================
static const Config* g_test_cfg = nullptr;  // si no es null => modo test
static Config        g_cache;               // almacenamiento de la config "normal"
static Config        g_test_storage;        // almacenamiento para set_for_tests()
static Config*       g_active   = nullptr;  // apunta a la config vigente (g_cache o g_test_storage)
static std::mutex    g_cfg_mtx;

// =============================
// Helpers internos
// =============================
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
        if (v < min || v > max) {
            throw cc::errors::ConfigError(
                std::string(key) + " fuera de rango (" +
                std::to_string(min) + ".." + std::to_string(max) + "): " + raw
            );
        }
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
    if (auto v = opt_getenv("BASE_URL_EVAL"))     cfg.endpoints.eval     = *v;
    if (auto v = opt_getenv("BASE_URL_ANALYZER")) cfg.endpoints.analyzer = *v;

    if (auto v = opt_getenv("HTTP_TIMEOUT_MS"))
        cfg.http.timeoutMs = parse_int_or_throw(*v, 1, 600'000, "HTTP_TIMEOUT_MS");
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

// =============================
// API pública
// =============================

const Config& get() {
    std::lock_guard<std::mutex> _lk(g_cfg_mtx);

    // Si hay config de test activa, g_active debe apuntarla
    if (g_test_cfg != nullptr) {
        g_active = const_cast<Config*>(g_test_cfg);
        return *g_active;
    }

    // Inicialización perezosa de la caché normal
    if (g_active == nullptr || g_active == &g_test_storage) {
        g_cache  = make_from_env_or_defaults();
        g_active = &g_cache;
    }
    return *g_active;
}

void set_for_tests(const Config& c) {
    std::lock_guard<std::mutex> _lk(g_cfg_mtx);
    g_test_storage = c;
    g_test_cfg     = &g_test_storage;
    g_active       = &g_test_storage; // para get
}

void unset_for_tests() {
    std::lock_guard<std::mutex> _lk(g_cfg_mtx);
    g_test_cfg = nullptr;
    g_active   = nullptr;
}

void reload() {
    std::lock_guard<std::mutex> _lk(g_cfg_mtx);

    // No recargar si estamos en modo test
    if (g_test_cfg != nullptr) return;

    g_cache  = make_from_env_or_defaults();
    g_active = &g_cache;
}

} // namespace cc::config
