//
// Created by andres on 5/10/25.
//

// logger.cpp
#include "logger.h"

#include <atomic>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

namespace cc::logging {

namespace {

// Estado global
std::mutex               g_mtx;
LogConfig                g_cfg{};
std::atomic<bool>        g_inited{false};
std::atomic<Level>       g_level{Level::Info};
Sanitizer                g_sanitize{};
std::string              g_component{};
std::unique_ptr<std::ofstream> g_file;

// Helpers
const char* level_str(Level l) {
    switch (l) {
        case Level::Trace:   return "TRACE";
        case Level::Debug:   return "DEBUG";
        case Level::Info:    return "INFO";
        case Level::Warn:    return "WARN";
        case Level::Error:   return "ERROR";
        case Level::Critical:return "CRIT";
    }
    return "?";
}

std::string timestamp(bool utc) {
    using namespace std::chrono;
    auto now   = system_clock::now();
    std::time_t tt = system_clock::to_time_t(now);
    std::tm tm{};
    if (utc) {
        gmtime_r(&tt, &tm);
    } else {
        localtime_r(&tt, &tm);
    }
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return oss.str();
}

std::string color_on(Level l) {
    if (!g_cfg.use_color) return {};
    switch (l) {
        case Level::Trace:   return "\033[90m"; // gray
        case Level::Debug:   return "\033[36m"; // cyan
        case Level::Info:    return "\033[32m"; // green
        case Level::Warn:    return "\033[33m"; // yellow
        case Level::Error:   return "\033[31m"; // red
        case Level::Critical:return "\033[41m\033[97m"; // white on red
    }
    return {};
}

std::string color_off() {
    return g_cfg.use_color ? "\033[0m" : "";
}

void rotate_if_needed() {
    if (g_cfg.file_path.empty() || !g_file) return;
    g_file->flush();
    try {
        g_file->seekp(0, std::ios::end);
        auto sz = g_file->tellp();
        if (sz < 0) return;
        if (static_cast<std::size_t>(sz) < g_cfg.max_file_bytes) return;

        // Cerrar actual
        g_file->close();

        // Rotar archivos: file.log.2 <- file.log.1 <- file.log
        for (int i = g_cfg.rotate_files - 1; i >= 1; --i) {
            std::ostringstream src, dst;
            src << g_cfg.file_path << "." << i;
            dst << g_cfg.file_path << "." << (i + 1);
            std::rename(src.str().c_str(), dst.str().c_str());
        }
        // file.log.1 <- file.log
        std::ostringstream first;
        first << g_cfg.file_path << ".1";
        std::rename(g_cfg.file_path.c_str(), first.str().c_str());

        // Reabrir nuevo
        g_file = std::make_unique<std::ofstream>(g_cfg.file_path, std::ios::out | std::ios::trunc);
    } catch (...) {
        // evitar tirar en logger
    }
}

std::string sanitize(std::string_view msg) {
    if (g_sanitize) return g_sanitize(msg);

    // Sanitizador por defecto: oculta Authorization: Bearer y api_key=XXXX
    std::string s(msg);
    auto hide = [&](std::string key, std::string marker, int keep = 4) {
        std::size_t pos = 0;
        while ((pos = s.find(key, pos)) != std::string::npos) {
            auto start = pos + key.size();
            auto end = s.find('\n', start);
            if (end == std::string::npos) end = s.size();

            if (start < end) {
                int n = static_cast<int>(end - start);
                if (n > keep) {
                    s.replace(start + keep, n - keep, std::string(n - keep, '*'));
                } else {
                    s.replace(start, n, std::string(n, '*'));
                }
            }
            pos = end;
        }
    };

    hide("Authorization: Bearer ", "Bearer ");
    hide("api_key=", "=", 0);
    hide("apikey=", "=", 0);
    hide("token=", "=", 0);

    return s;
}

} // anon namespace

// ============ Logger API ============
void Logger::init(const LogConfig& cfg) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_cfg   = cfg;
    g_level.store(cfg.min_level, std::memory_order_relaxed);

    if (!cfg.file_path.empty()) {
        g_file = std::make_unique<std::ofstream>(cfg.file_path, std::ios::out | std::ios::app);
    }
    g_inited.store(true, std::memory_order_release);
}

void Logger::set_level(Level lvl) {
    g_level.store(lvl, std::memory_order_relaxed);
}

void Logger::set_sanitizer(Sanitizer fn) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_sanitize = std::move(fn);
}

void Logger::set_component(std::string component_name) {
    std::lock_guard<std::mutex> lk(g_mtx);
    g_component = std::move(component_name);
}

Level Logger::level() {
    return g_level.load(std::memory_order_relaxed);
}

void Logger::trace(std::string_view msg)   { write_line(Level::Trace,   msg, nullptr, 0); }
void Logger::debug(std::string_view msg)   { write_line(Level::Debug,   msg, nullptr, 0); }
void Logger::info (std::string_view msg)   { write_line(Level::Info,    msg, nullptr, 0); }
void Logger::warn (std::string_view msg)   { write_line(Level::Warn,    msg, nullptr, 0); }
void Logger::error(std::string_view msg)   { write_line(Level::Error,   msg, nullptr, 0); }
void Logger::critical(std::string_view msg){ write_line(Level::Critical,msg, nullptr, 0); }

void Logger::log(Level lvl, std::string_view msg, const char* file, int line) {
    write_line(lvl, msg, file, line);
}

void Logger::write_line(Level lvl, std::string_view msg, const char* file, int line) {
    if (lvl < g_level.load(std::memory_order_relaxed)) return;

    std::lock_guard<std::mutex> lk(g_mtx);

    // Formato: 2025-11-11 14:33:12.123 [INFO] (Component) file.cpp:123 — mensaje
    std::ostringstream oss;
    oss << timestamp(g_cfg.use_utc) << " "
        << "[" << level_str(lvl) << "] ";

    if (!g_component.empty()) {
        oss << "(" << g_component << ") ";
    }
    if (file && line > 0) {
        oss << file << ":" << line << " — ";
    }

    oss << sanitize(msg);

    const auto line_str = oss.str();

    // Consola
    if (g_cfg.to_stderr) {
        auto on  = color_on(lvl);
        auto off = color_off();
        std::cerr << on << line_str << off << "\n";
    }

    // Archivo
    if (g_file) {
        (*g_file) << line_str << "\n";
        rotate_if_needed();
    }
}

} // namespace cc::logging
