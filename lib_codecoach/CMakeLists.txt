cmake_minimum_required(VERSION 4.0)
project(lib_codecoach)

set(CMAKE_CXX_STANDARD 20)

add_library(lib_codecoach STATIC library.cpp
        contracts/problem_dto.h
        contracts/eval_dto.h
        contracts/analyzer_dto.h
        contracts/sandbox_contract.h
        http/http_client.cpp
        http/http_client.h
        http/http_response.cpp
        http/http_response.h
        sdk/problems_client.cpp
        sdk/problems_client.h
        sdk/eval_client.cpp
        sdk/eval_client.h
        sdk/analyzer_client.cpp
        sdk/analyzer_client.h
        sdk/llm_client.h
        sdk/llm_client_openai.cpp
        sdk/llm_client_openai.h
        config/config_manager.cpp
        config/config_manager.h
        errors/exceptions.h
        logging/logger.cpp
        logging/logger.h
        metrics/timer.cpp
        metrics/timer.h
        prompts/coach_prompts.cpp
        prompts/coach_prompts.h)
