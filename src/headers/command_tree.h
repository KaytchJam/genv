#pragma once

#include <string>

#include "utility.h"
#include "chest.h"

namespace genv {
    // Data structure for handling the validity of user passed in arguments
    class CommandTree {

        // Node in the CommandTree: holds a command and a vector of other nodes
        struct CTNode {
            GenvCommand command;
            std::vector<CTNode> branches;

            // generate a node from a string 'token'
            static CTNode node_from_token(const std::string& token);

            // print the path from a given CTNode node
            static void print_from(const CTNode& node);

            CTNode(const CTNode& rhs) : command(rhs.command), branches(rhs.branches) {}
            CTNode(GenvCommand command, std::vector<CTNode> v) : command(command), branches(v) {}
        };

        // generate a path of CTNodes based on the delimited tokens in string 'line'
        static void path_from_line(CTNode& node, const std::string& line);

        CTNode root;
    public:
        // default constructor for CommandTree
        CommandTree() : root({ GenvCommand(CommandType::LAST_ENUM), std::vector<CTNode>() }) {}

        // takes in a commands.txt file and build a command tree from it
        static Chest<CommandTree> build_from(const std::string& command_path);

        // print every path in the CommandTree
        void print_tree();

        /// A kind of iterator for the CommmandTree
        /// Movement through the tree occurs when 'walk' is invoked along with
        /// a passed in argument. The validity of the argument for the current step
        /// in the command tree is returned
        class CommandTreeWalker {
            CTNode* cur; // we don't own the object being pointed to
        public:
            CommandTreeWalker(CTNode& node) : cur(&node) {}

            // walk through the associated command tree given an argument,
            // returns a Chest<Command> for the given argument depending on whether the
            // argument was valid or not
            Chest<GenvCommand> walk(const std::string& argument) {
                for (int i = 0; i < this->cur->branches.size(); i++) {
                    CTNode* current_node = &this->cur->branches[i];
                    if (current_node->command.get_validation_function()(argument)) {
                        this->cur = current_node;
                        return Chest<GenvCommand>::stuff({current_node->command.unwrap_command_type(), argument});
                    }
                }
                
                return Chest<GenvCommand>::sign("No walk could be made using argument {" + argument + "}");
            }
        };

        // Returns a CommandTreeWalker for this CommandTree
        CommandTreeWalker get_walker() {
            return CommandTreeWalker(this->root);
        }
    };
}