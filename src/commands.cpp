#include "commands.h"

command::command(std::vector<std::string> &command_list)
{
    for (unsigned int i = 0; i < command_list.size(); i++)
    {
        if (i == 0)
        {
            this->command_name = command_list.at(i);
        }
        else
        {
            this->command_arguments.push_back(command_list.at(i));
        }
    }
}

command::~command() {}

piped_commands::piped_commands() {}

piped_commands::~piped_commands() {}

void piped_commands::set_command_list(command *cmd)
{
    cmd_list.push_back(cmd);
}

command_exec::command_exec()
{
    this->cmd = cmd;
}

command_exec::~command_exec() {}

int command_exec::execute(command *cmd, bool is_background_process)
{
    std::string command_name = cmd->get_command_name();
    std::vector<std::string> command_arguments = cmd->get_command_arguments();

    int ret;

    if (command_name == "exit")
    {
        ret = command_exec::execute_exit(cmd);
    }
    else if (command_name == "cd")
    {
        if (!is_background_process)
        {
            ret = command_exec::execute_cd(cmd);
        }
        else
        {
            ret = command_exec::execute_others(cmd, is_background_process);
        }
    }
    else if (command_name == "pwd")
    {
        if (!is_background_process)
        {
            ret = command_exec::execute_pwd(cmd);
        }
        else
        {
            ret = command_exec::execute_others(cmd, is_background_process);
        }
    }
    else if (command_name == "set")
    {
        if (!is_background_process)
        {
            ret = command_exec::execute_set(cmd);
        }
        else
        {
            ret = command_exec::execute_others(cmd, is_background_process);
        }
    }
    else
    {
        ret = command_exec::execute_others(cmd, is_background_process);
    }
    return ret;
}

int command_exec::execute(piped_commands *pipe_cmd)
{
    signal(SIGCHLD, SIG_DFL);
    int status;
    unsigned int i;
    std::vector<command *> command_list = pipe_cmd->get_command_list();
    int fds[2];
    int input_dup, output_dup;

    input_dup = dup(STDIN_FILENO);
    output_dup = dup(STDOUT_FILENO);

    for (i = 0; i < command_list.size() - 1; i++)
    {
        command *command = command_list.at(i);
        std::string command_name = command->get_command_name();
        std::vector<std::string> command_args = command->get_command_arguments();
        std::string command_path = utility::get_command_path(command_name) + "/" + command_name;

        char **argv = new char *[command_args.size() + 2];
        argv[0] = new char[command_path.size() + 1];
        strcpy(argv[0], command_path.c_str());

        for (unsigned int i = 0; i < command_args.size(); i++)
        {
            argv[i + 1] = new char[command_args.at(i).size() + 1];
            strcpy(argv[i + 1], command_args.at(i).c_str());
        }
        argv[command_args.size() + 1] = NULL;

        pipe(fds);
        int child_pid = fork();

        if (child_pid == 0)
        {
            close(STDOUT_FILENO);
            dup(fds[1]);
            close(fds[1]);
            close(fds[0]);
            if (execv(argv[0], argv) < 0)
            {
                std::cout << "Error:: Command failed" << std::endl;
            }
            exit(1);
        }
        else if (child_pid > 0)
        {
            wait(&status);
            if (status >> 8 == 0xff)
            {
                std::cout << "Error:: Command Failed" << std::endl;
                return 1;
            }
            close(STDIN_FILENO);
            dup(fds[0]);
            close(fds[0]);
            close(fds[1]);
        }
        else
        {
            std::cout << "Error:: Fork Failed" << std::endl;
            return 1;
        }
    }

    command *command = command_list.at(i);
    std::string command_name = command->get_command_name();
    std::vector<std::string> command_args = command->get_command_arguments();
    std::string command_path = utility::get_command_path(command_name) + "/" + command_name;

    char **argv = new char *[command_args.size() + 2];
    argv[0] = new char[command_path.size() + 1];
    strcpy(argv[0], command_path.c_str());

    for (unsigned int i = 0; i < command_args.size(); i++)
    {
        argv[i + 1] = new char[command_args.at(i).size() + 1];
        strcpy(argv[i + 1], command_args.at(i).c_str());
    }
    argv[command_args.size() + 1] = NULL;

    int child_pid = fork();

    if (child_pid == 0)
    {
        if (execvp(argv[0], argv) < 0)
        {
            std::cout << "Error:: Command Failed" << std::endl;
        }
        exit(1);
    }
    else if (child_pid > 0)
    {
        wait(&status);
        if (status >> 8 == 0xff)
        {
            std::cout << "Error:: Command Failed" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Error:: Fork Creation Failed" << std::endl;
        return 1;
    }

    dup2(input_dup, STDIN_FILENO);
    dup2(output_dup, STDOUT_FILENO);
    signal(SIGCHLD, utility::sig_handler);

    return 0;
}

int command_exec::execute_exit(command *cmd)
{
    if (!(cmd->get_command_arguments()).empty())
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int command_exec::execute_cd(command *cmd)
{
    std::string home_directory = utility::get_env_var("HOME");
    std::string target;
    std::vector<std::string> command_arg = cmd->get_command_arguments();
    if (command_arg.size() > 2)
    {
        std::cout << "Error:: Invalid Arguments" << std::endl;
        return 1;
    }
    else if (command_arg.empty())
    {
        target = home_directory;
    }
    else
    {
        target = command_arg.at(0);
        std::string tilt = "~";
        std::size_t tilt_position = target.find(tilt);

        if (tilt_position != std::string::npos)
        {
            target.replace(tilt_position, tilt.size(), home_directory);
        }
        if (target.empty())
        {
            std::cout << "Error:: Invalid Arguments" << std::endl;
            return 1;
        }
    }
    int chdir_output = chdir(target.c_str());
    if (chdir_output < 0)
    {
        std::cout << "Error:: cd failed" << std::endl;
        return 1;
    }
    return 0;
}

int command_exec::execute_pwd(command *cmd)
{
    if (!(cmd->get_command_arguments()).empty())
    {
        std::cout << "Error:: Invalid Arguments" << std::endl;
        return 1;
    }
    std::string cur_dir = utility::get_cur_dir();
    if (!cur_dir.empty())
    {
        std::cout << cur_dir << std::endl;
    }
    else
    {
        std::cout << "Error:: pwd failed";
        return 1;
    }
    return 0;
}

int command_exec::execute_set(command *cmd)
{
    std::string home_directory = utility::get_env_var("HOME");
    std::vector<std::string> command_arg = cmd->get_command_arguments();
    if (command_arg.empty())
    {
        utility::print_environment();
    }
    else if (command_arg.size() > 1)
    {
        std::cout << "Error:: Invalid Arguments" << std::endl;
        return 1;
    }
    else
    {
        std::vector<std::string> splitted_command_argument = utility::parse_input(command_arg.at(0), '=');
        if (splitted_command_argument.size() != 2)
        {
            std::cout << "Error:: Invalid Arguments" << std::endl;
            return 1;
        }
        else
        {
            std::string path = splitted_command_argument.at(1);
            if (!path.empty() && path[0] == '~')
            {
                std::string tilt = "~";
                std::size_t tilt_position = path.find(tilt);

                if (tilt_position != std::string::npos)
                {
                    path.replace(tilt_position, tilt.size(), home_directory);
                }
            }
            setenv(splitted_command_argument.at(0).c_str(), path.c_str(), 1);
        }
    }
    return 0;
}

int command_exec::execute_others(command *cmd, bool is_background_process)
{
    signal(SIGCHLD, SIG_IGN);

    int status;
    std::string command_name = cmd->get_command_name();
    std::vector<std::string> command_args = cmd->get_command_arguments();
    std::string command_path = utility::get_command_path(command_name) + "/" + command_name;

    char **argv = new char *[command_args.size() + 2];
    argv[0] = new char[command_path.size() + 1];
    strcpy(argv[0], command_path.c_str());

    for (unsigned int i = 0; i < command_args.size(); i++)
    {
        argv[i + 1] = new char[command_args.at(i).size() + 1];
        strcpy(argv[i + 1], command_args.at(i).c_str());
    }
    argv[command_args.size() + 1] = NULL;
    int child_pid = fork();
    if (child_pid == 0)
    {
        signal(SIGCHLD, SIG_DFL);
        if (execvp(argv[0], argv) < 0)
        {
            std::cout << "Error:: Command Failed" << std::endl;
        }
        exit(1);
    }
    else if (child_pid > 0)
    {
        if (!is_background_process)
        {
            signal(SIGCHLD, SIG_DFL);
            wait(&status);
            signal(SIGCHLD, utility::sig_handler);
        }
        else
        {
            signal(SIGCHLD, utility::sig_handler);
        }
        if (status >> 8 == 0xff)
        {
            std::cout << "Error:: Command Failed" << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Error:: Fork Creation Failed" << std::endl;
        return 1;
    }
    return 0;
}