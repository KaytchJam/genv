#include <fstream>

#include "headers/command_tree.h"

#define COMMAND_TXT_DELIM ' '

namespace genv {
    void CommandTree::CTNode::print_from(const CommandTree::CTNode& node) {
        std::cout << node.command.to_string();
        for (int i = 0; i < node.branches.size(); i++) {
            std::cout << ' ';
            CommandTree::CTNode::print_from(node.branches[i]);
        }
    }

    void CommandTree::print_tree() {
        for (int i = 0; i < this->root.branches.size(); i++) {
            CommandTree::CTNode::print_from(this->root.branches[i]);
            std::cout << std::endl;
        }
    }

    CommandTree::CTNode CommandTree::CTNode::node_from_token(const std::string& token) {
        for (int i = CommandType::BUILD; i < CommandType::LAST_ENUM; i++) {
            GenvCommand cur_command = GenvCommand((CommandType) i, token);
            if (token == cur_command.to_string()) {
                return { cur_command, std::vector<CTNode>() };
            }
        }
        
        std::cout << "Error occured" << std::endl;
        return {GenvCommand(CommandType::LAST_ENUM), std::vector<CTNode>() };
    }

    void CommandTree::path_from_line(CommandTree::CTNode& node, const std::string& line) {
        const int line_size = (int) line.size();
        CommandTree::CTNode* cur_node = &node;
        int start_index = 0;

        for (int i = 0; i < line_size; i++) {
            const char cur = line[i];

            if (cur == COMMAND_TXT_DELIM) {
                cur_node->branches.push_back(CommandTree::CTNode::node_from_token(line.substr(start_index, i - start_index)));
                cur_node = &cur_node->branches[cur_node->branches.size() - 1];
                start_index = i + 1;
            }
        }
    }

    Chest<CommandTree> CommandTree::build_from(const std::string& command_path) {
        std::ifstream input(command_path);

        if (!input.is_open()) {
            return Chest<CommandTree>::sign("Error: Unable to open file at: " + command_path);
        }

        CommandTree new_tree;
        std::string line;
        while (std::getline(input, line)) {
            CommandTree::path_from_line(new_tree.root, line);
        }

        return Chest<CommandTree>::stuff(new_tree);
    } 
}