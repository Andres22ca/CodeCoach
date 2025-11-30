cmake_minimum_required(VERSION 3.20)
project(codecoach CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -----------------------------
#  DEPENDENCIAS EXTERNAS
# -----------------------------
find_package(CURL REQUIRED)                 # libcurl
find_package(nlohmann_json 3.2.0 REQUIRED)  # <-- JSON

# -----------------------------
#  LA LIBRERÍA
# -----------------------------
add_library(lib_codecoach STATIC
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

        # Headers (opcionales en la lista)
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

target_include_directories(lib_codecoach
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (MSVC)
    target_compile_options(lib_codecoach PRIVATE /W4 /permissive-)
else()
    target_compile_options(lib_codecoach PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Linkear libcurl, nlohmann_json y definir CC_USE_CURL
target_link_libraries(lib_codecoach
        PRIVATE
        CURL::libcurl
        nlohmann_json::nlohmann_json
)

target_compile_definitions(lib_codecoach
        PRIVATE
        CC_USE_CURL
)

# -----------------------------
#  EL EJECUTABLE DE PRUEBA
# -----------------------------
add_executable(codecoach_smoke
        tests/smoke_main.cpp
)

target_link_libraries(codecoach_smoke
        PRIVATE lib_codecoach
)
