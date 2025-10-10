//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_EVAL_DTO_H
#define LIB_CODECOACH_EVAL_DTO_H
#include <string>
#include <vector>

#pragma once



namespace cc::contracts {
    struct RunRequest {
        std::string code;
        std::string problemId;
        std::string stdin; // para pruebas en la gu, ojo, no es obligatorio

    };

    struct RunCaseResult {  // resultado por caso de prueba

        std::string input;
        std::string output; //lo que produjo el programa de usuario
        std::string expected;
        bool passed;
        int timeMs; //timepo de ejecucion en ms
        int memoryKB;

    };

    struct RunResult {
        bool passed; // para ver si todos los casos pasaron
        std ::vector<RunCaseResult> cases;
        int timeMs; // para promedio de ejecucion entre casos
        int memoryKB;// para promedio de memoria entre casos
        std::string stdout; //para captura golbal
        std ::string stderr;
        int exitCode;





    };
}











#endif //LIB_CODECOACH_EVAL_DTO_H
// estructura para resultados de respuestas