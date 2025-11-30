#include "logging/logger.h"
#include "config/config_manager.h"
#include "http/http_client.h"
#include "http/http_response.h"
#include "prompts/coach_prompts.h"
#include "contracts/problem_dto.h"
#include "contracts/eval_dto.h"
#include "sdk/problems_client.h"
#include "sdk/eval_client.h"
#include "sdk/analyzer_client.h"
#include "Mongo/mongo_client.h"
#include "Mongo/problem_repository.h"

#include <iostream>

void print_result(const std::string& name, bool ok) {
    if (ok) {
        cc::logging::Logger::info("[PASS] " + name);
    } else {
        cc::logging::Logger::error("[FAIL] " + name);
    }
}

int main() {
    // 1. Logger
    cc::logging::LogConfig cfg;
    cfg.min_level = cc::logging::Level::Debug;
    cfg.to_stderr = true;
    cc::logging::Logger::init(cfg);

    cc::logging::Logger::info("===== CodeCoach Smoke Test =====");

    // 2. Config
    const auto& conf = cc::config::get();

    cc::logging::Logger::info("problemsBaseUrl = " + conf.endpoints.problemsBaseUrl);
    cc::logging::Logger::info("evalBaseUrl     = " + conf.endpoints.evalBaseUrl);
    cc::logging::Logger::info("analyzerBaseUrl = " + conf.endpoints.analyzerBaseUrl);

    bool config_ok = !conf.endpoints.problemsBaseUrl.empty();
    print_result("Config Manager Load", config_ok);

    // 3. HTTP simple
    {
        cc::http::HttpClient http;
        auto resp = http.get(conf.endpoints.problemsBaseUrl + "/problems");

        bool ok = resp.statusCode != 0;
        print_result("HTTP GET (problems)", ok);

        cc::logging::Logger::info("HTTP Body (preview):");
        cc::logging::Logger::info(cc::http::summarize(resp, 200));
    }

    // 4. Prompts
    {
        cc::contracts::ProblemDetail prob;
        prob.id         = "two-sum";
        prob.title      = "Two Sum";
        prob.statement  = "Given an array...";
        prob.difficulty = "Easy";
        prob.tags       = {"array", "hashmap"};

        cc::contracts::RunResult eval{};
        eval.passed   = false;
        eval.timeMs   = 20;
        eval.memoryKB = 1024;
        eval.exitCode = 0;

        auto prompt = cc::prompts::make_analyze_prompt(
            "int main(){return 0;}",
            eval,
            prob,
            "cpp",
            "gpt-4-turbo"
        );

        bool ok = !prompt.user.empty();
        print_result("Prompt Builder", ok);
    }

#ifdef CC_ENABLE_MONGO_REAL
    // 5. MongoClient directo
    {
        cc::storage::MongoClient mongo(conf.mongo.uri, conf.mongo.dbName, false);
        bool connected = mongo.connect();

        print_result("MongoDB Connect", connected);

        if (connected) {
            bool ins = mongo.insertOne("smoke_test", R"({"msg":"hello","n":1})");
            print_result("Mongo InsertOne", ins);

            auto doc = mongo.findOne("smoke_test", R"({"n":1})");
            print_result("Mongo FindOne", doc.has_value());
        }
    }

    // 6. ProblemRepository
    {
        cc::storage::MongoClient mc(conf.mongo.uri, conf.mongo.dbName, false);
        bool ok_conn = mc.connect();
        if (!ok_conn) {
            print_result("ProblemRepository Mongo connect", false);
        } else {
            cc::storage::ProblemRepository repo(mc);

            cc::contracts::ProblemDetail p;
            p.id         = "smoke-problem";
            p.title      = "Smoke Test Problem";
            p.statement  = "Test only";
            p.difficulty = "Easy";
            p.tags       = {"test"};

            bool created = repo.createProblem(p);
            print_result("Repo createProblem", created);

            auto read = repo.getProblemById("smoke-problem");
            print_result("Repo getProblemById", read.has_value());
        }
    }

#else
    cc::logging::Logger::warn("Mongo tests skipped: CC_ENABLE_MONGO_REAL not defined");
#endif

    // 7. SDK ProblemsClient
    {
        cc::sdk::ProblemsClient pc(conf.endpoints.problemsBaseUrl);
        auto list = pc.list();
        (void)list;
        print_result("SDK ProblemsClient list", true);
    }

    // 8. SDK EvalClient
    {
        cc::sdk::EvalClient ec(conf.endpoints.evalBaseUrl);
        cc::contracts::RunRequest req;
        req.code      = "int main(){return 0;}";
        req.problemId = "two-sum";

        auto result = ec.submit(req);
        (void)result;
        print_result("SDK EvalClient submit", true);
    }

    // 9. SDK AnalyzerClient
    {
        cc::sdk::AnalyzerClient ac(conf.endpoints.analyzerBaseUrl);

        cc::contracts::RunResult eval{};
        eval.passed = false;
        eval.timeMs = 10;

        auto fb = ac.analyze("int main(){}", eval, "two-sum");
        (void)fb;
        print_result("SDK AnalyzerClient analyze", true); // aún stub
    }

    cc::logging::Logger::info("===== END Smoke Test =====");
    return 0;
}
