//
// Created by andres on 5/10/25.
//

#pragma once
#include <stdexcept>
#include <string>

namespace cc::errors {
    class ConfigError : public std::runtime_error {
    public:
        explicit ConfigError(const std::string& msg)
            : std::runtime_error(msg) {}
    };
}
