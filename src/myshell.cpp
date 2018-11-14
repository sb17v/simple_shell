#include "myshell.h"

int main(int argc, char **argv)
{
    // utility::print_start_symbol();
    while (true)
    {
        std::string input = "";
        int ret = 0;

        utility::print_start_symbol();
        std::getline(std::cin, input);

        if (std::cin.eof())
        {
            break;
        }
        if (input.empty())
        {
            continue;
        }
        if (input.length() > MAX_INPUT_BUFFER_SIZE)
        {
            std::cout << "Input line exceed 80 characters" << std::endl;
            utility::print_start_symbol();
            continue;
        }

        if (utility::is_piped_command(input))
        {
            std::vector<std::string> command_list;
            command_list = utility::parse_input(input, '|');
            piped_commands *piped_commands_obj = new piped_commands();
            for (unsigned int i = 0; i < command_list.size(); i++)
            {
                std::vector<std::string> command_vector = utility::parse_input(command_list.at(i), ' ');
                command *command_obj = new command(command_vector);
                piped_commands_obj->set_command_list(command_obj);
            }
            command_exec *command_exec_obj = new command_exec();
            ret = command_exec_obj->execute(piped_commands_obj);
        }
        else
        {
            bool background_process_flag = utility::is_background_process(input);
            std::string command_string;
            std::vector<std::string> command_vector;

            if (background_process_flag)
            {
                command_string = utility::parse_input(input, '&').at(0);
            }
            else
            {
                command_string = input;
            }

            if (utility::is_input_redirection(input) || utility::is_output_redirection(input))
            {
                command_vector = utility::handle_input_output_redirection(command_string);
            }
            else
            {
                command_vector = utility::parse_input(command_string, ' ');
            }
            command *command_obj = new command(command_vector);
            command_exec *command_exec_obj = new command_exec();
            ret = command_exec_obj->execute(command_obj, background_process_flag);
            if (utility::is_input_redirection(input) || utility::is_output_redirection(input))
            {
                utility::close_input_output_redirection();
            }
        }

        if (ret < 0)
        {
            std::cout << "Exiting myshell..." << ret << std::endl;
            break;
        }
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}