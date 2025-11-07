//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_LLM_CLIENT_H
#define LIB_CODECOACH_LLM_CLIENT_H

#include <string>

namespace cc::sdk {

    // Interfaz abstracta para clientes LLM
    class ILLMClient {
    public:
        virtual ~ILLMClient() = default;

        // Completar un prompt con el LLM
        virtual std::string complete(const std::string& prompt,
                                     const std::string& systemPrompt = "") = 0;

        // Verificar si el cliente está disponible (tiene API key, etc)
        virtual bool isAvailable() = 0;
    };

} // namespace cc::sdk

#endif //LIB_CODECOACH_LLM_CLIENT_H