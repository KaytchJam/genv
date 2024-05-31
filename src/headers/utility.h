#pragma once

#include <queue>
#include <string>
#include <iostream>
#include <filesystem>

namespace genv {
    // takes in the args 
    static std::queue<std::string> build_arg_queue(int argc, char** argv) {
        std::queue<std::string> arg_queue;

        for (int i = 1; i < argc; i++) {
            //std::cout << argv[i] << std::endl;
            arg_queue.push(argv[i]);
        }

        return arg_queue;
    }

    // Returns the path to the commands.txt file as a string
    static std::string get_command_path(const std::string& command_file_name) {
        char* ptr;
        _get_pgmptr(&ptr); // fuck linux users lmaoooooooooooooooooo
        std::filesystem::path exe_path = ptr;

        const int UPPER = 1000;
        int loop_times = 0;
        while (exe_path.filename() != "genv" && loop_times < UPPER) {
            exe_path = exe_path.parent_path();
            loop_times++;
        }

        if (loop_times >= UPPER) {
            // Would've return a chest here but we really can't do anything without the commands.txt file, so i might as well just exit
            std::cout << "Was unable to find the 'genv' directory from the executable's path." << std::endl;
            exit(EXIT_FAILURE);
        }

        return (exe_path / command_file_name).string();
    }

    // represents all the encounterable states
    enum CommandType{
        BUILD,
        REFERENCE,
        URL,
        DIR_NAME,
        FILE_NAME,
        END,
        LAST_ENUM
    };

    // Wrapper for the CommandType enum
    class GenvCommand {
    private:
        CommandType cmd_type;
        const std::string value;
    public:
        GenvCommand() : cmd_type(CommandType::END), value("") {}
        GenvCommand(const CommandType ct) : cmd_type(ct), value("") {}
        GenvCommand(const CommandType ct, const std::string& arg) : cmd_type(ct), value(arg) {}
        GenvCommand(const GenvCommand& rhs) : cmd_type(rhs.cmd_type), value(rhs.value) {}
        ~GenvCommand() {}

        // return a string representation of the current command_type
        std::string to_string() const {
            switch (this->cmd_type) {
                case CommandType::BUILD: return "build";
                case CommandType::REFERENCE: return "ref";
                case CommandType::DIR_NAME: return "<DIR_NAME>";
                case CommandType::FILE_NAME: return "<FILE_NAME>";
                case CommandType::URL: return "<URL>";
                case CommandType::END: return "*";
                case CommandType::LAST_ENUM: return "";
                default: return "not implemented lol";
            }
        }

        typedef bool (*arg_validation_function)(const std::string&);
        arg_validation_function get_validation_function() const {
            switch (this->cmd_type) {

                case CommandType::BUILD:
                    return [](const std::string& arg) {
                        return arg == "build";
                    };
                case CommandType::DIR_NAME:
                    return [](const std::string& arg) {
                        return arg.find(' ') == std::string::npos && !std::filesystem::path(arg).has_extension();
                    };
                default:
                    return [](const std::string& arg) {
                        return false;
                    };
            }
        }

        // Return the CommandType being wrapped by the GenvCommand object
        CommandType unwrap_command_type() const {
            return this->cmd_type;
        }

        // Returns the string value being wrapped by the GenvCommand object
        std::string unwrap_command_arg() const {
            return this->value;
        }
    };
}