//
// Created by andres on 5/10/25.
//
// coach_prompts.cpp — Implementación de helpers para construir prompts del LLM.
//

#include "prompts/coach_prompts.h"

#include "contracts/eval_dto.h"
#include "contracts/problem_dto.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace cc::prompts {

// -------------------------------------------------
// Utilidades internas
// -------------------------------------------------

std::string sanitize_for_llm(std::string_view s) {
    std::string out;
    out.reserve(s.size());

    for (char c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        // Dejamos \n, \r, \t; el resto de controles los reemplazamos por espacio.
        if (uc < 0x20 && c != '\n' && c != '\r' && c != '\t') {
            out.push_back(' ');
        } else {
            out.push_back(c);
        }
    }

    // Normalizar espacios consecutivos a uno solo (simple)
    std::string compact;
    compact.reserve(out.size());
    bool prev_space = false;
    for (char c : out) {
        bool is_space = std::isspace(static_cast<unsigned char>(c)) != 0;
        if (is_space) {
            if (!prev_space) {
                compact.push_back(' ');
                prev_space = true;
            }
        } else {
            compact.push_back(c);
            prev_space = false;
        }
    }

    return compact;
}

std::string truncate_middle(std::string_view s, std::size_t max) {
    if (s.size() <= max || max < 5) {
        return std::string{s};
    }

    const std::size_t keep  = max - 1;
    const std::size_t front = keep / 2;
    const std::size_t back  = keep - front;

    std::string out;
    out.reserve(max);
    out.append(s.substr(0, front));
    out.push_back('...');
    out.append(s.substr(s.size() - back));
    return out;
}

std::string cases_to_compact_json(const cc::contracts::RunResult& eval,
                                  std::size_t max_chars)
{
    // Representación tipo JSON simplificada para el LLM, recortando si excede max_chars.
    std::ostringstream oss;
    oss << "[";
    bool first = true;

    for (const auto& c : eval.cases) {
        if (!first) {
            oss << ",";
        }
        first = false;

        oss << "{"
            << "\"in\":\""  << sanitize_for_llm(c.input)    << "\","
            << "\"out\":\"" << sanitize_for_llm(c.output)   << "\","
            << "\"exp\":\"" << sanitize_for_llm(c.expected) << "\","
            << "\"ok\":"    << (c.passed ? "true" : "false")
            << "}";

        if (oss.tellp() >= static_cast<std::streamoff>(max_chars)) {
            oss << ",{\"truncated\":true}";
            break;
        }
    }

    oss << "]";
    std::string json = oss.str();
    if (json.size() > max_chars) {
        return truncate_middle(json, max_chars);
    }
    return json;
}

std::string language_from_problem_tags(const std::vector<std::string>& tags,
                                       std::string_view fallback)
{
    for (const auto& t : tags) {
        std::string lower = t;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c){ return std::tolower(c); });

        if (lower == "cpp" || lower == "c++") {
            return "cpp";
        }
        if (lower == "python" || lower == "py") {
            return "python";
        }
        if (lower == "java") {
            return "java";
        }
        if (lower == "c" || lower == "c-lang") {
            return "c";
        }
    }
    return std::string{fallback};
}

// -------------------------------------------------
// Helpers internos para armar secciones del prompt
// -------------------------------------------------

static std::string build_problem_section(const cc::contracts::ProblemDetail& problem,
                                         const RenderLimits& limits)
{
    std::ostringstream oss;
    oss << "Problema: " << truncate_middle(problem.title, limits.maxTitleChars) << "\n";
    oss << "Dificultad: " << problem.difficulty << "\n";
    if (!problem.tags.empty()) {
        oss << "Tags: ";
        bool first = true;
        for (const auto& tag : problem.tags) {
            if (!first) oss << ", ";
            first = false;
            oss << tag;
        }
        oss << "\n";
    }

    std::string statement = sanitize_for_llm(problem.statement);
    statement = truncate_middle(statement, limits.maxStatementChars);
    oss << "\nEnunciado (resumido):\n" << statement << "\n";

    if (!problem.samples.empty()) {
        oss << "\nEjemplos:\n";
        for (const auto& s : problem.samples) {
            oss << "Input:\n"  << s.input  << "\n";
            oss << "Output:\n" << s.output << "\n";
        }
    }

    return oss.str();
}

static std::string build_eval_section(const cc::contracts::RunResult& eval,
                                      const RenderLimits& limits)
{
    std::ostringstream oss;
    oss << "Resultado de ejecución:\n";
    oss << "  Pasó todos los casos: " << (eval.passed ? "sí" : "no") << "\n";
    oss << "  Tiempo total (ms): "   << eval.timeMs   << "\n";
    oss << "  Memoria total (KB): "  << eval.memoryKB << "\n";
    oss << "  Exit code: "           << eval.exitCode << "\n\n";

    std::string stdout_s = truncate_middle(sanitize_for_llm(eval.stdout),
                                           limits.maxStdoutChars);
    std::string stderr_s = truncate_middle(sanitize_for_llm(eval.stderr),
                                           limits.maxStderrChars);
    oss << "STDOUT (recortado):\n" << stdout_s << "\n\n";
    oss << "STDERR (recortado):\n" << stderr_s << "\n\n";

    std::string cases_json = cases_to_compact_json(eval, limits.maxCasesJsonChars);
    oss << "Casos de prueba (JSON compacto):\n" << cases_json << "\n";

    return oss.str();
}

static std::string build_code_section(const std::string& code,
                                      const RenderLimits& limits)
{
    std::string sanitized = sanitize_for_llm(code);
    sanitized = truncate_middle(sanitized, limits.maxCodeChars);

    std::ostringstream oss;
    oss << "Código del usuario (recortado si es muy largo):\n";
    oss << "```cpp\n" << sanitized << "\n```\n";
    return oss.str();
}

// -------------------------------------------------
// Constructores de Prompt (API pública)
// -------------------------------------------------

Prompt make_analyze_prompt(const std::string& code,
                           const cc::contracts::RunResult& eval,
                           const cc::contracts::ProblemDetail& problem,
                           std::string_view language,
                           std::string_view model,
                           const RenderLimits& limits)
{
    Prompt p;
    p.version     = kVersionAnalyze;
    p.locale      = kDefaultLocale;
    p.maxTokens   = 800;
    p.temperature = 0.2;

    std::string lang = language_from_problem_tags(problem.tags, language);
    (void)model; // por ahora no se usa dentro del prompt

    // Mensaje system
    p.system =
        "Eres un asistente experto en algoritmos y estructuras de datos. "
        "Analizas código enviado por estudiantes, junto con los resultados de las pruebas, "
        "y generas un diagnóstico técnico y pedagógico. "
        "Responde siempre en español neutro, claro y conciso.";

    // Mensaje user
    std::ostringstream u;
    u << "Lenguaje objetivo: " << lang << "\n\n";
    u << build_problem_section(problem, limits) << "\n\n";
    u << build_eval_section(eval, limits)       << "\n\n";
    u << build_code_section(code, limits)       << "\n\n";

    u << "Tarea:\n"
      << "1. Explica brevemente qué intenta resolver el problema.\n"
      << "2. Analiza el enfoque del estudiante (complejidad temporal y espacial aproximada).\n"
      << "3. Señala los errores lógicos o de implementación que explican los fallos.\n"
      << "4. Propón una estrategia mejor (sin dar el código completo) y su complejidad.\n";

    p.user = u.str();
    return p;
}

Prompt make_hints_prompt(const std::string& code,
                         const cc::contracts::RunResult& eval,
                         const cc::contracts::ProblemDetail& problem,
                         std::string_view language,
                         std::string_view model,
                         const RenderLimits& limits)
{
    Prompt p;
    p.version     = kVersionHints;
    p.locale      = kDefaultLocale;
    p.maxTokens   = 600;
    p.temperature = 0.3;

    std::string lang = language_from_problem_tags(problem.tags, language);
    (void)model;

    p.system =
        "Eres un tutor de programación. Tu objetivo es dar pistas graduales "
        "para que el estudiante corrija su solución sin revelar directamente la respuesta. "
        "Responde siempre en español, usando un tono amigable y motivador.";

    std::ostringstream u;
    u << "Lenguaje objetivo: " << lang << "\n\n";
    u << build_problem_section(problem, limits) << "\n\n";
    u << build_eval_section(eval, limits)       << "\n\n";
    u << build_code_section(code, limits)       << "\n\n";

    u << "Tarea:\n"
      << "Genera como máximo 3 pistas (de menor a mayor detalle) para ayudar al estudiante a mejorar su solución.\n"
      << "No des el código completo; enfócate en ideas, casos borde y errores típicos.\n";

    p.user = u.str();
    return p;
}

Prompt make_explain_failure_prompt(const std::string& code,
                                   const cc::contracts::RunResult& eval,
                                   std::string_view language,
                                   std::string_view model,
                                   const RenderLimits& limits)
{
    Prompt p;
    p.version     = kVersionExplain;
    p.locale      = kDefaultLocale;
    p.maxTokens   = 500;
    p.temperature = 0.2;

    std::string lang = std::string{language};
    (void)model;

    p.system =
        "Eres un asistente que explica por qué una solución de programación falla en ciertas pruebas. "
        "Debes ser muy claro y concreto, usando ejemplos basados en los casos de prueba fallidos.";

    std::ostringstream u;
    u << "Lenguaje objetivo: " << lang << "\n\n";
    u << build_eval_section(eval, limits) << "\n\n";
    u << build_code_section(code, limits) << "\n\n";

    u << "Tarea:\n"
      << "1. Explica qué patrón o error principal provoca que algunos casos fallen.\n"
      << "2. Menciona un caso concreto de entrada/salida donde falle y por qué.\n"
      << "3. Da una sugerencia breve para corregir el problema.\n";

    p.user = u.str();
    return p;
}

} // namespace cc::prompts
