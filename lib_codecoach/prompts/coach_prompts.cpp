//
// Created by andres on 5/10/25.
//

// prompts.h — Plantillas y utilidades para construir prompts del LLM (Analyzer/Coach).
#ifndef LIB_CODECOACH_PROMPTS_H
#define LIB_CODECOACH_PROMPTS_H

#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <cstddef>

namespace cc {
namespace contracts {
    struct RunCaseResult;
    struct RunResult;
    struct Sample;
    struct ProblemSummary;
    struct ProblemDetail;
}

namespace prompts {

struct Prompt {
    std::string system;      // Mensaje "system"
    std::string user;        // Mensaje "user"
    int         maxTokens{800};
    double      temperature{0.2};
    std::string version;     // p.ej. "analyze/v1"
    std::string locale;      // p.ej. "es-CR"
};

struct RenderLimits {
    std::size_t maxCodeChars        = 8000;
    std::size_t maxStdoutChars      = 1500;
    std::size_t maxStderrChars      = 1500;
    std::size_t maxCasesJsonChars   = 6000;
    std::size_t maxStatementChars   = 2000;
    std::size_t maxTitleChars       = 120;
};

constexpr const char* kVersionAnalyze = "analyze/v1";
constexpr const char* kVersionHints   = "hints/v1";
constexpr const char* kVersionExplain = "explain/v1";
constexpr const char* kDefaultLocale  = "es-CR";

// --- Utilidades ---
std::string sanitize_for_llm(std::string_view s);                 // limpia control chars, normaliza espacios
std::string truncate_middle(std::string_view s, std::size_t max); // "inicio…fin"
std::string cases_to_compact_json(const cc::contracts::RunResult& eval, std::size_t max_chars);
std::string language_from_problem_tags(const std::vector<std::string>& tags,
                                       std::string_view fallback = "cpp");

// --- Constructores de prompts ---
Prompt make_analyze_prompt(const std::string& code,
                           const cc::contracts::RunResult& eval,
                           const cc::contracts::ProblemDetail& problem,
                           std::string_view language = "cpp",
                           std::string_view model = "gpt-4-turbo",
                           const RenderLimits& limits = {});

Prompt make_hints_prompt(const std::string& code,
                         const cc::contracts::RunResult& eval,
                         const cc::contracts::ProblemDetail& problem,
                         std::string_view language = "cpp",
                         std::string_view model = "gpt-4-turbo",
                         const RenderLimits& limits = {});

Prompt make_explain_failure_prompt(const std::string& code,
                                   const cc::contracts::RunResult& eval,
                                   std::string_view language = "cpp",
                                   std::string_view model = "gpt-4-turbo",
                                   const RenderLimits& limits = {});

} // namespace prompts
} // namespace cc

#endif // LIB_CODECOACH_PROMPTS_H




#include "coach_prompts.h"
