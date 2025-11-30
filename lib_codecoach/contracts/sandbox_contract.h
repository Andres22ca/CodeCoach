// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_SANDBOX_CONTRACT_H
#define LIB_CODECOACH_SANDBOX_CONTRACT_H

#include <string>
#include <vector>

namespace cc::sandbox {

    struct SandboxLimits {
        int timeLimitMs{0};   // tiempo máximo para la CPU
        int memoryLimitKB{0}; // pico permitido en KB
    };

    struct SandboxFiles {
        std::string workdir;        // directorio de trabajo
        std::string sourceFilename; // nombre del archivo fuente
        std::string executablePath; // ruta del ejecutable compilado
    };

    struct SandboxToolchain {
        std::string              language;    // ej. "cpp20", "python3"
        std::vector<std::string> compileCmd;  // comando para compilar
        std::vector<std::string> runCmd;      // comando para ejecutar
    };

    struct SandboxRequest {
        std::string   code;       // código fuente o ruta
        std::string   stdin;      // entrada global
        SandboxLimits limits;
        SandboxFiles  files;
        SandboxToolchain toolchain;
    };

    enum class SandboxStatus {
        OK,
        COMPILE_ERROR,
        RUNTIME_ERROR,
        TIMEOUT,
        MEMORY_EXCEEDED,
        OUTPUT_EXCEEDED,
        INTERNAL_ERROR
    };

    struct SandboxMetrics {
        int timeMs{0};    // tiempo de CPU usado
        int memoryKB{0};  // pico máximo observado
    };

    struct SandboxResult {
        SandboxStatus status{SandboxStatus::INTERNAL_ERROR};
        int           exitCode{0};  // 0 si OK, otro valor si fallo
        std::string   stdout;
        std::string   stderr;
        SandboxMetrics metrics;
    };

} // namespace cc::sandbox

#endif // LIB_CODECOACH_SANDBOX_CONTRACT_H
