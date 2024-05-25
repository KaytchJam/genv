#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <queue>
#include <stdlib.h>

#include "headers/utility.h"
#include "headers/chest.h"
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

        cmd_queue.push(cmd_result.open());
        arg_queue.pop();
    }

    /// std::cout << "Commands all make sense it seems" << std::endl;

    // execute our commands
    while (!cmd_queue.empty()) {
        env.process_command(cmd_queue.front());
        cmd_queue.pop();
    }

    return 0;
}