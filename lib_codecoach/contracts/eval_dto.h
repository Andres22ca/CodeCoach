// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_EVAL_DTO_H
#define LIB_CODECOACH_EVAL_DTO_H

#include <string>
#include <vector>

namespace cc::contracts {

    struct RunRequest {
        std::string code;
        std::string problemId;
        std::string stdin; // para pruebas en la GUI, no es obligatorio
    };

    // Resultado por caso de prueba
    struct RunCaseResult {
        std::string input;
        std::string output;   // lo que produjo el programa del usuario
        std::string expected;
        bool passed{false};
        int  timeMs{0};       // tiempo de ejecución en ms
        int  memoryKB{0};     // memoria usada en KB
    };

    struct RunResult {
        bool passed{false};                  // true si TODOS los casos pasaron
        std::vector<RunCaseResult> cases;    // resultados por caso

        int timeMs{0};       // total
        int memoryKB{0};     // pico de memoria
        std::string stdout;  // salida global capturada
        std::string stderr;  // errores globales capturados
        int exitCode{0};     // código de salida del proceso
    };

} // namespace cc::contracts

#endif // LIB_CODECOACH_EVAL_DTO_H
