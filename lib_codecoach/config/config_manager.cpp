// config_manager.cpp
// Variables de entorno soportadas:
//   CODECOACH_PROBLEMS_URL   (default: http://localhost:8081)
//   CODECOACH_EVAL_URL       (default: http://localhost:8082)
//   CODECOACH_ANALYZER_URL   (default: http://localhost:8083)

//   CODECOACH_MONGO_URI      (default: mongodb://localhost:27017)
//   CODECOACH_MONGO_DB       (default: codecoach)

//   CODECOACH_HTTP_TIMEOUT_MS (default: 10000, rango [100, 120000])
//   CODECOACH_HTTP_RETRIES    (default: 2,     rango [0, 10])


#include "config_manager.h"
#include "errors/exceptions.h"

#include <cstdlib>   // std::getenv
#include <cctype>    // std::isdigit
#include <string>
#include <stdexcept>

namespace cc::config {

    using cc::errors::ConfigError;

    // estado interno

    // Override para tests
    static const Config* g_test_cfg = nullptr;

    // Config cacheada
    static Config g_cfg{};
    static bool   g_loaded = false;

    // Helpers internos

    // Devuelve el valor de una variable
    static std::string getenv_or(const char* key, const char* defVal) {
        if (const char* v = std::getenv(key)) {
            if (*v != '\0') return std::string(v);
        }
        return std::string(defVal);
    }

    // Devuelve true si el string parece una URL http/https
    static bool looks_like_url(const std::string& s) {
        return s.rfind("http://", 0) == 0 || s.rfind("https://", 0) == 0;
    }

    // Parsea un entero y valida rango. Lanza ConfigError si hay problema.
    static int parse_int_or_throw(const std::string& raw,
                                  int minVal,
                                  int maxVal,
                                  const char* key)
    {
        if (raw.empty()) {
            throw ConfigError(std::string("Empty integer value for env var: ") + key);
        }

        // Validar formato [opcional '-'] + dígitos
        std::size_t start = 0;
        if (raw[0] == '-') {
            start = 1;
        }
        for (std::size_t i = start; i < raw.size(); ++i) {
            if (!std::isdigit(static_cast<unsigned char>(raw[i]))) {
                throw ConfigError(std::string("Invalid integer value for env var ")
                                  + key + ": '" + raw + "'");
            }
        }

        int value = 0;
        try {
            value = std::stoi(raw);
        } catch (const std::exception&) {
            throw ConfigError(std::string("Failed to parse integer for env var ")
                              + key + ": '" + raw + "'");
        }

        if (value < minVal || value > maxVal) {
            throw ConfigError("Env var " + std::string(key) +
                              " out of range: " + std::to_string(value) +
                              " (expected between " + std::to_string(minVal) +
                              " and " + std::to_string(maxVal) + ")");
        }
        return value;
    }

    // Carga la config desde el entorno o usa defaults.
    static Config make_from_env_or_default() {
        Config cfg{};

        // --- Endpoints REST ---
        cfg.endpoints.problemsBaseUrl =
            getenv_or("CODECOACH_PROBLEMS_URL", "http://localhost:8081");
        cfg.endpoints.evalBaseUrl =
            getenv_or("CODECOACH_EVAL_URL", "http://localhost:8082");
        cfg.endpoints.analyzerBaseUrl =
            getenv_or("CODECOACH_ANALYZER_URL", "http://localhost:8083");

        // Validación básica de URLs
        if (!looks_like_url(cfg.endpoints.problemsBaseUrl)) {
            throw ConfigError("CODECOACH_PROBLEMS_URL no parece una URL válida: " +
                              cfg.endpoints.problemsBaseUrl);
        }
        if (!looks_like_url(cfg.endpoints.evalBaseUrl)) {
            throw ConfigError("CODECOACH_EVAL_URL no parece una URL válida: " +
                              cfg.endpoints.evalBaseUrl);
        }
        if (!looks_like_url(cfg.endpoints.analyzerBaseUrl)) {
            throw ConfigError("CODECOACH_ANALYZER_URL no parece una URL válida: " +
                              cfg.endpoints.analyzerBaseUrl);
        }

        // --- MongoDB ---
        cfg.mongo.uri =
            getenv_or("CODECOACH_MONGO_URI", "mongodb://localhost:27017");
        cfg.mongo.dbName =
            getenv_or("CODECOACH_MONGO_DB", "codecoach");



        // HTTP
        {
            std::string tRaw =
                getenv_or("CODECOACH_HTTP_TIMEOUT_MS", "10000");
            std::string rRaw =
                getenv_or("CODECOACH_HTTP_RETRIES", "2");

            cfg.http.timeoutMs = parse_int_or_throw(
                    tRaw, 100, 120000, "CODECOACH_HTTP_TIMEOUT_MS");
            cfg.http.retries = parse_int_or_throw(
                    rRaw, 0, 10, "CODECOACH_HTTP_RETRIES");
        }

        return cfg;
    }

    // ------------------------
    // API pública
    // ------------------------

    const Config& get() {
        // Si hay config de pruebas, usarla siempre
        if (g_test_cfg) {
            return *g_test_cfg;
        }

        // Cargar una sola vez desde el entorno
        if (!g_loaded) {
            g_cfg = make_from_env_or_default();
            g_loaded = true;
        }
        return g_cfg;
    }

    void reload() {
        // Si estamos en modo test, no recargar nada
        if (g_test_cfg) {
            return;
        }
        g_cfg = make_from_env_or_default();
        g_loaded = true;
    }

    void set_for_tests(const Config& cfg) {
        g_test_cfg = &cfg;
    }

    void unset_for_tests() {
        g_test_cfg = nullptr;
    }

} // namespace cc::config
