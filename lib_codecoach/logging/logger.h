//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_LOGGER_H
#define LIB_CODECOACH_LOGGER_H

#include <string>
#include <string_view>
#include <functional>
#include <cstdint>

namespace cc::logging {

enum class Level : uint8_t {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

struct LogConfig {
    Level  min_level    = Level::Info;
    bool   to_stderr    = true;
    bool   use_color    = true;
    bool   use_utc      = false;
    std::string file_path {};       // vacío => no escribir a archivo
    std::size_t max_file_bytes = 2 * 1024 * 1024; // 2 MB
    int    rotate_files = 3;        // número de rotaciones a conservar
};

// Permite “censurar” datos sensibles antes de imprimir (API keys, tokens...)
using Sanitizer = std::function<std::string(std::string_view)>;

class Logger {
public:
    // Inicializa configuración global del logger (idempotente).
    static void init(const LogConfig& cfg);

    // Configuraciones en caliente
    static void set_level(Level lvl);
    static void set_sanitizer(Sanitizer fn);
    static void set_component(std::string component_name); // etiqueta de componente (SDK/HTTP/GUI/…)

    // API simple (compatible con tu código existente)
    static void trace(std::string_view msg);
    static void debug(std::string_view msg);
    static void info (std::string_view msg);
    static void warn (std::string_view msg);
    static void error(std::string_view msg);
    static void critical(std::string_view msg);

    // API con metadata (archivo/línea) — usada por macros
    static void log(Level lvl, std::string_view msg,
                    const char* file, int line);

    // Acceso de utilidad (por si quieres inspeccionar en tests)
    static Level level();

private:
    // Oculta implementación
    static void write_line(Level lvl, std::string_view msg,
                           const char* file, int line);

    // No instanciable
    Logger() = delete;
};

// Macros para capturar file/line en el punto de uso
#define CC_LOG_TRACE(msg)   ::cc::logging::Logger::log(::cc::logging::Level::Trace,   (msg), __FILE__, __LINE__)
#define CC_LOG_DEBUG(msg)   ::cc::logging::Logger::log(::cc::logging::Level::Debug,   (msg), __FILE__, __LINE__)
#define CC_LOG_INFO(msg)    ::cc::logging::Logger::log(::cc::logging::Level::Info,    (msg), __FILE__, __LINE__)
#define CC_LOG_WARN(msg)    ::cc::logging::Logger::log(::cc::logging::Level::Warn,    (msg), __FILE__, __LINE__)
#define CC_LOG_ERROR(msg)   ::cc::logging::Logger::log(::cc::logging::Level::Error,   (msg), __FILE__, __LINE__)
#define CC_LOG_CRITICAL(msg)::cc::logging::Logger::log(::cc::logging::Level::Critical,(msg), __FILE__, __LINE__)

} // namespace cc::logging

#endif // LIB_CODECOACH_LOGGER_H