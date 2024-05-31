#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <queue>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <curl/curl.h>

#include "headers/toolkit/chest.h"
#include "headers/toolkit/str_slice.h"

#include "headers/utility.h"
#include "headers/general_errors.h"
#include "headers/studio.h"
#include "headers/command_tree.h"

#define COMMANDS_FILE_NAME "commands.txt"
#define VALIDATION_ERROR_TEXT "Error during command validation:\n"
#define GENV_PANIC_CLOSING_TEXT "\ngenv is now closing...\n"

///
/// BASIC GENV ARGUMENTS
/// genv <command> <arguments>
/// GENV [build] {directory_name}
/// 

int main(int argc, char **argv) {
    genv::Studio env = genv::Studio::from_working_dir();
    genv::CommandTree cmd_tree = genv::CommandTree::build_from(genv::get_command_path(COMMANDS_FILE_NAME)).open();
    //cmd_tree.print_tree();

    genv::CommandTree::CommandTreeWalker cmd_walker = cmd_tree.get_walker();
    std::queue<std::string> arg_queue = genv::build_arg_queue(argc, argv);
    std::queue<genv::GenvCommand> cmd_queue;

    // are the passed in commands valid?
    while (!arg_queue.empty()) {
        std::string& cur_arg = arg_queue.front();
        Chest<genv::GenvCommand> cmd_result = cmd_walker.walk(cur_arg);

        if (cmd_result.is_signed()) {
            std::cout << + VALIDATION_ERROR_TEXT << cmd_result.open_sign() << GENV_PANIC_CLOSING_TEXT << std::endl;
            exit(EXIT_FAILURE);
        }

        // execute our command
        std::cout << "Processing argument: " << cmd_result.open().unwrap_command_arg() << std::endl;
        env.process_command(cmd_result.open());
        arg_queue.pop();
    }

    std::cout << "end" << std::endl;
    return EXIT_SUCCESS;
}