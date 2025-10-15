//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_CONFIG_MANAGER_H
#define LIB_CODECOACH_CONFIG_MANAGER_H

#include <string>


namespace cc::config {

    // Tipos
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
    void reload();                       // Fuerza releer el entorno
    void set_for_tests(const Config&);   // Inyecta una configuración (para pruebas)

    // No declares cc::errors aquí; ya lo estás incluyendo
    // Si ocurre un error, las funciones lanzarán cc::errors::ConfigError

} // namespace cc::config

#endif // LIB_CODECOACH_CONFIG_MANAGER_H
