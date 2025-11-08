//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_SANDBOX_CONTRACT_H
#define LIB_CODECOACH_SANDBOX_CONTRACT_H
#include <string>
#include <vector>


namespace cc::sandbox {
    struct SandboxLimits {
        int timeLimitMs; //tiempo maximo para la cpu a la hora de cargar la respuesta
        int memoryLimitKB; //pico permitido en KB


    };
    struct SandboxFiles {
        std::string workdir; //directorio de trabajo
        std::string sourceFilename; //el ejecutable
        std::string executablePath; //Donde queda el compilado

    };

    struct SandboxToolchain {
        std::string language;
        std::vector<std::string> compileCmd;
        std::vector<std::string> runCmd;

    };
    struct SandboxRequest {
        std::string code; //fuente o ruta del codigo
        std::string stdin; //entrada global
        SandboxLimits limits;
        SandboxFiles files;
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
        int timeMs; //cpu
        int memoryKB;//pico maximo observado
    };

    struct SandboxResult {
        SandboxStatus status;
        int exitCode; // 0 si ok, si no se pone un cod cualquiera
        std::string stdout;
        std::string stderr;
        SandboxMetrics metrics;

    };
}

#endif //LIB_CODECOACH_SANDBOX_CONTRACT_H

// definir estructura para moror de evaluacion