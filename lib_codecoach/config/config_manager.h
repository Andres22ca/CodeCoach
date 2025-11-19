// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_CONFIG_MANAGER_H
#define LIB_CODECOACH_CONFIG_MANAGER_H

#include <string>

namespace cc::config {

    struct Endpoints {
        std::string problems;
        std::string eval;
        std::string analyzer;
    };

    struct HttpPolicy {
        int timeoutMs;
        int retries;
    };

    struct Config {
        Endpoints endpoints;
        HttpPolicy http;
    };

    // API principal
    const Config& get();                 // Devuelve referencia a la configuración cargada (cacheada)
    void reload();                       // Fuerza releer el entorno (no hace nada si set_for_tests() activo)
    void set_for_tests(const Config&);   // Inyecta una configuración (para pruebas)
    void unset_for_tests();              // Desactiva el modo test y deja que get()/reload() usen el entorno

} // namespace cc::config

#endif // LIB_CODECOACH_CONFIG_MANAGER_H
