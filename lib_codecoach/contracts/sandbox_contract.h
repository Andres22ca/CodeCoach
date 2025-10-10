//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_SANDBOX_CONTRACT_H
#define LIB_CODECOACH_SANDBOX_CONTRACT_H
#include <string>

struct SandboxLimits {
    int timeLimitsMs; //tiempo maximo para la cpu a la hora de cargar la respuesta
    int memoryLimitKb;


};
struct SandboxFiles {
    std::string workdir; //directorio de trabajo
    std::string sourceFilename; //el ejecutable
    std::string executablePath; //Donde queda el compilado





};








#endif //LIB_CODECOACH_SANDBOX_CONTRACT_H

// definir estructura para moror de evaluacion