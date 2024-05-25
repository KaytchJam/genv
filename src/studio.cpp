#include <filesystem>
#include <iostream>

#include "headers/studio.h"


namespace genv {
    Studio Studio::from_working_dir() {
        std::filesystem::path cwd = std::filesystem::current_path();
        std::cout << cwd.string() << std::endl;

        return Studio();
    }

    void Studio::process_command(const GenvCommand& command) {
        switch (command.unwrap_command_type()) {

        }
    }
}