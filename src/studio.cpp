#include <filesystem>
#include <iostream>
#include <fstream>
#include <curl/curl.h>

#include "headers/studio.h"

#define GENV_FILE_NAME ".genv"

namespace genv {
    Studio Studio::from_working_dir() {
        std::filesystem::path cwd = std::filesystem::current_path();
        std::cout << "Current working directory: " << cwd.string() << std::endl;
        Studio s;

        for (auto const& entry : std::filesystem::directory_iterator(cwd)) {
            if (std::filesystem::path(entry).filename().string() == GENV_FILE_NAME) {
                //std::cout << "this is a genv directory" << std::endl;
                s.is_genv_directory = true;
                return s;
            }
        }

        std::cout << "this is not a .genv directory" << std::endl;
        return s;
    }

    void Studio::process_command(const GenvCommand& command) {
        switch (command.unwrap_command_type()) {
            case CommandType::BUILD:
            {
                if (this->is_genv_directory) {
                    std::cout << "This is already a .genv directory! Exiting!";
                    exit(EXIT_FAILURE);
                }
            }
                break;
            case CommandType::DIR_NAME:
            {
                std::filesystem::path dir_full_path = (std::filesystem::current_path() / command.unwrap_command_arg());

                bool success = std::filesystem::create_directory(dir_full_path);
                if (!success) {
                    std::cout << "Studio.process_command(): Was unable to create directory: " + dir_full_path.string();
                    exit(EXIT_FAILURE);
                }

                std::ofstream outfile(dir_full_path / GENV_FILE_NAME);
                if (!outfile.is_open()) {
                    std::cout << "Studio.process_command(): Was unable to create .genv configuration file at directory: " + dir_full_path.string();
                    exit(EXIT_FAILURE);
                }

                outfile << "This is a genv file." << std::endl;
                outfile.close();
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