#pragma once

#include <exception>

namespace genv {
    class argument_validation_error : public std::exception {
    public:
        argument_validation_error() {}
        const char* what() {
            return "Failed to properly validate the passed in arguments.";
        }
    };
}