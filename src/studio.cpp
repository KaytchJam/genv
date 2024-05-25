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
            case CommandType::BUILD:
                break;
            case CommandType::DIR_NAME:
            {
                std::filesystem::path dir_name = command.unwrap_command_arg();
                std::filesystem::create_directory(std::filesystem::current_path() / dir_name);
                break;
            }
            case CommandType::FILE_NAME:
                break;
            case CommandType::END:
                break;
            case CommandType::REFERENCE:
                break;
            case CommandType::URL:
                break;
            default:
                break;
        }
    }
}