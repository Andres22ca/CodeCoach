//
// Created by andres on 17/11/25.
//

#include "logging/logger.h"
#include "config/config_manager.h"
#include "http/http_client.h"
#include "http/http_response.h"
#include "prompts/coach_prompts.h"
#include "contracts/problem_dto.h"
#include "contracts/eval_dto.h"

int main() {

    // --------------------------
    // 1. Inicializar Logger
    // --------------------------
    cc::logging::LogConfig cfg;
    cfg.min_level = cc::logging::Level::Debug;
    cfg.to_stderr = true;
    cc::logging::Logger::init(cfg);

    cc::logging::Logger::info("==== Smoke Test CodeCoach ====");

    // --------------------------
    // 2. Leer configuración
    // --------------------------
    const auto& conf = cc::config::get();
    cc::logging::Logger::info("BASE_URL_PROBLEMS = " + conf.endpoints.problems);

    // --------------------------
    // 3. HTTP Client (modo STUB si no hay CURL)
    // --------------------------
    cc::http::HttpClient client;
    auto resp = client.get(conf.endpoints.problems + "/problems");

    cc::logging::Logger::info("HTTP Status = " + std::to_string(resp.statusCode));
    cc::logging::Logger::info("Body summary = " + cc::http::summarize(resp, 100));

    // --------------------------
    // 4. Probar Prompts (analyze)
    // --------------------------
    cc::contracts::ProblemDetail prob;
    prob.id = "two-sum";
    prob.title = "Two Sum";
    prob.statement = "Given an array of integers...";
    prob.difficulty = "Easy";
    prob.tags = {"array", "hashmap"};

    cc::contracts::RunResult eval{};
    eval.passed = true;
    eval.timeMs = 25;
    eval.memoryKB = 2048;
    eval.exitCode = 0;

    auto prompt = cc::prompts::make_analyze_prompt(
        "int main(){ return 0; }",
        eval,
        prob,
        "cpp",
        "gpt-4-turbo"
    );

    cc::logging::Logger::info("SYSTEM PROMPT:\n" + prompt.system);
    cc::logging::Logger::info("USER PROMPT:\n" + prompt.user);

    cc::logging::Logger::info("==== Smoke Test OK ====");

    return 0;
}
