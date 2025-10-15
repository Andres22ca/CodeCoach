cmake_minimum_required(VERSION 3.20)
project(lib_codecoach CXX)

# Estándar y PIC
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Librería
add_library(lib_codecoach STATIC
        # Fuentes (.cpp)
        http/http_client.cpp
        http/http_response.cpp
        sdk/problems_client.cpp
        sdk/eval_client.cpp
        sdk/analyzer_client.cpp
        sdk/llm_client_openai.cpp
        config/config_manager.cpp
        logging/logger.cpp
        metrics/timer.cpp
        prompts/coach_prompts.cpp

        # Headers
        contracts/problem_dto.h
        contracts/eval_dto.h
        contracts/analyzer_dto.h
        contracts/sandbox_contract.h
        http/http_client.h
        http/http_response.h
        sdk/problems_client.h
        sdk/eval_client.h
        sdk/analyzer_client.h
        sdk/llm_client.h
        sdk/llm_client_openai.h
        config/config_manager.h
        errors/exceptions.h
        logging/logger.h
        metrics/timer.h
        prompts/coach_prompts.h
)

# Rutas de include públicas (tus headers están directamente bajo shared/)
target_include_directories(lib_codecoach
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# (Opcional) Warnings útiles
if (MSVC)
    target_compile_options(lib_codecoach PRIVATE /W4 /permissive-)
else()
    target_compile_options(lib_codecoach PRIVATE -Wall -Wextra -Wpedantic)
endif()

# (Opcional) Aquí enlazarás deps cuando las agregues:
# find_package(nlohmann_json CONFIG REQUIRED)
# target_link_libraries(lib_codecoach PUBLIC nlohmann_json::nlohmann_json)
