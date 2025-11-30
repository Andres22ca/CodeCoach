//
// Created by andres on 5/10/25.
//

#ifndef LIB_CODECOACH_LLM_CLIENT_OPENAI_H
#define LIB_CODECOACH_LLM_CLIENT_OPENAI_H

#include "llm_client.h"
#include "http/http_client.h"
#include <string>

namespace cc::sdk {

    class OpenAIClient : public ILLMClient {
    private:
        std::string apiKey_;
        std::string model_;
        http::HttpClient httpClient_;

    public:
        // model: "gpt-4-turbo-preview", "gpt-3.5-turbo", etc
        explicit OpenAIClient(const std::string& apiKey,
                             const std::string& model = "gpt-3.5-turbo");

        std::string complete(const std::string& prompt,
                            const std::string& systemPrompt = "") override;

        bool isAvailable() const override;

    };

} // namespace cc::sdk

#endif //LIB_CODECOACH_LLM_CLIENT_OPENAI_H