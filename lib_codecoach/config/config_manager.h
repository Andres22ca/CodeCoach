// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_CONFIG_MANAGER_H
#define LIB_CODECOACH_CONFIG_MANAGER_H

#include <string>

namespace cc::config {

    // URLs base de los microservicios REST
    struct Endpoints {
        std::string problemsBaseUrl;   // p.ej. "http://localhost:8081"
        std::string evalBaseUrl;       // p.ej. "http://localhost:8082"
        std::string analyzerBaseUrl;   // p.ej. "http://localhost:8083"
    };

    // Config de MongoDB
    struct MongoConfig {
        std::string uri;               // p.ej. "mongodb://localhost:27017"
        std::string dbName;            // p.ej. "codecoach"
    };

    // Política HTTP (timeouts y reintentos)
    struct HttpPolicy {
        int timeoutMs{10000};
        int retries{2};
    };

    // Configuración global de CodeCoach
    struct Config {
        Endpoints  endpoints;
        MongoConfig mongo;
        HttpPolicy http;
    };

    // API principal
    const Config& get();                 // Devuelve referencia a la configuración cargada (cacheada)
    void reload();                       // Fuerza releer el entorno (no hace nada si set_for_tests() activo)
    void set_for_tests(const Config&);   // Inyecta una configuración (para pruebas)
    void unset_for_tests();              // Desactiva el modo test y deja que get()/reload() usen el entorno

} // namespace cc::config

#endif // LIB_CODECOACH_CONFIG_MANAGER_H
