#include "utility.h"

int utility::fd_in = -1;
int utility::fd_out = -1;
int utility::dup_fd_in = -1;
int utility::dup_fd_out = -1;
int utility::input_file_redirection = 0;
int utility::ouput_file_redirection = 0;
int utility::input_output_file_redirection = 0;

std::vector<std::string> utility::parse_input(const std::string &input_string, char delimeter)
{
    std::vector<std::string> command_vector;

    std::stringstream command_stream(input_string);
    std::string splitted_string;

    while (std::getline(command_stream, splitted_string, delimeter))
    {
        size_t left_index = splitted_string.find_first_not_of(' ');
        if (left_index == std::string::npos)
        {
            std::cout << "End of string" << std::endl;
        }
        size_t right_index = splitted_string.find_last_not_of(' ');
        splitted_string = splitted_string.substr(left_index, (right_index - left_index) + 1);
        if (!splitted_string.empty())
        {
            command_vector.push_back(splitted_string);
        }
    }
    return command_vector;
}

bool utility::is_input_redirection(std::string input_string)
{
    std::size_t index = input_string.find_first_of("<");
    return (index != std::string::npos);
}

bool utility::is_output_redirection(std::string input_string)
{
    std::size_t index = input_string.find_first_of(">");
    return (index != std::string::npos);
}

bool utility::is_piped_command(std::string input_string)
{
    std::size_t index = input_string.find_first_of("|");
    return (index != std::string::npos);
}

bool utility::is_background_process(std::string input_string)
{
    std::size_t index = input_string.find_first_of("&");
    return (index != std::string::npos);
}

std::string utility::get_env_var(const std::string &var)
{
    char *value = getenv(var.c_str());
    if (value)
    {
        return std::string(value);
    }
    return "";
}

std::string utility::get_cur_dir()
{
    long max_path_size = pathconf(".", _PC_PATH_MAX);
    char *path_buffer;
    char *cwd_output;

    path_buffer = (char *)malloc((size_t)max_path_size);
    if (path_buffer != NULL)
    {
        cwd_output = getcwd(path_buffer, (size_t)max_path_size);
        if (cwd_output != NULL)
        {
            return std::string(path_buffer);
        }
        else
        {
            return "";
        }
    }
    else
    {
        return "";
    }
}

void utility::print_environment()
{
    extern char **environ;
    int i = 0;
    while (environ[i])
    {
        std::cout << environ[i++] << std::endl;
    }
}

std::string utility::get_cur_usr()
{
    struct passwd *passwrd;
    unsigned int usr_id = geteuid();

    passwrd = getpwuid(usr_id);

    if (passwrd != NULL)
    {
        return std::string(passwrd->pw_name);
    }
    else
    {
        char uid[80];
        sprintf(uid, "%d", usr_id);
        return std::string(uid);
    }
}

void utility::print_start_symbol()
{
    std::string start_symbol = "$ ";
    std::string username = utility::get_cur_usr();
    if (username == "root")
    {
        start_symbol = "# ";
    }
    char host_name_buffer[HOST_NAME_MAX];
    gethostname(host_name_buffer, HOST_NAME_MAX);
    std::string hostname(host_name_buffer);
    std::string cur_dir = utility::get_cur_dir();
    std::string home_dir = utility::get_env_var("HOME");
    int home_dir_position = cur_dir.find(home_dir);
    if (home_dir_position >= 0)
        cur_dir.replace(home_dir_position, home_dir.size(), "~");

    std::cout << username << "@" << hostname << ":" << cur_dir << start_symbol << std::flush;
}

std::string utility::get_command_path(std::string command_name)
{
    std::string env_var = "PATH";
    if (command_name == "myls")
    {
        env_var = "MYPATH";
    }
    std::string env_var_val = utility::get_env_var(env_var);
    if (env_var_val.empty())
    {
        std::cout << "Environment Variable " << env_var << " not set." << std::endl;
    }
    std::vector<std::string> splitted_env_var_val = utility::parse_input(env_var_val, ':');

    for (unsigned int i = 0; i < splitted_env_var_val.size(); i++)
    {
        if (utility::is_command_in_path(command_name, splitted_env_var_val.at(i)))
        {
            env_var_val = splitted_env_var_val.at(i);
            break;
        }
    }
    return env_var_val;
}

bool utility::is_command_in_path(std::string command, std::string path)
{
    struct dirent *dir_entry;
    DIR *dir = opendir(path.c_str());

    if (dir == NULL)
    {
        return false;
    }

    while ((dir_entry = readdir(dir)) != NULL)
    {
        std::string filename(dir_entry->d_name);

        if (filename.empty() && filename.at(0) == '.')
        {
            continue;
        }

        if (filename == command)
        {
            return true;
        }
    }
    return false;
}

std::vector<std::string> utility::handle_input_output_redirection(std::string input)
{
    std::vector<std::string> parsed_input;
    std::string command_name;
    if (utility::is_input_redirection(input) && utility::is_output_redirection(input))
    {
        utility::input_output_file_redirection = 1;
        parsed_input = utility::parse_input(input, '<');
        if (parsed_input.size() != 2)
        {
            std::cout << "Error" << std::endl;
        }

        std::vector<std::string> file_names = utility::parse_input(parsed_input.at(1), '>');
        if (file_names.size() != 2)
        {
            std::cout << "Error" << std::endl;
        }

        command_name = parsed_input.at(0);
        std::string input_file_name = file_names.at(0);
        std::string output_file_name = file_names.at(1);

        dup_fd_in = dup(STDIN_FILENO);
        dup_fd_out = dup(STDOUT_FILENO);
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        utility::fd_in = open(input_file_name.c_str(), O_RDWR, 0777);
        if (utility::fd_in < 0)
        {
            std::cout << "Error" << std::endl;
        }

        utility::fd_out = open(output_file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (utility::fd_out < 0)
        {
            std::cout << "Error" << std::endl;
        }
    }
    else if (utility::is_input_redirection(input))
    {
        utility::input_file_redirection = 1;
        parsed_input = utility::parse_input(input, '<');
        if (parsed_input.size() != 2)
        {
            std::cout << "Error" << std::endl;
        }

        command_name = parsed_input.at(0);
        std::string input_file_name = parsed_input.at(1);

        dup_fd_in = dup(STDIN_FILENO);
        close(STDIN_FILENO);
        utility::fd_in = open(input_file_name.c_str(), O_RDWR, 0777);
        if (utility::fd_in < 0)
        {
            std::cout << "Error" << std::endl;
        }
    }
    else if (utility::is_output_redirection(input))
    {
        utility::ouput_file_redirection = 1;
        parsed_input = utility::parse_input(input, '>');
        if (parsed_input.size() != 2)
        {
            std::cout << "Error" << std::endl;
        }
        command_name = parsed_input.at(0);
        std::string output_file_name = parsed_input.at(1);

        dup_fd_out = dup(STDOUT_FILENO);
        close(STDOUT_FILENO);
        utility::fd_out = open(output_file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (utility::fd_out < 0)
        {
            std::cout << "Error" << std::endl;
        }
    }
    std::vector<std::string> command_vector = utility::parse_input(command_name, ' ');
    return command_vector;
}

void utility::close_input_output_redirection()
{
    if (utility::input_output_file_redirection)
    {
        close(fd_in);
        close(fd_out);
        dup2(dup_fd_in, STDIN_FILENO);
        dup2(dup_fd_out, STDOUT_FILENO);
        utility::fd_in = -1;
        utility::fd_out = -1;
        utility::dup_fd_in = -1;
        utility::dup_fd_out = -1;
        utility::input_output_file_redirection = 0;
    }
    else if (utility::input_file_redirection)
    {
        close(fd_in);
        dup2(dup_fd_in, STDIN_FILENO);
        utility::fd_in = -1;
        utility::dup_fd_in = -1;
        utility::input_file_redirection = 0;
    }
    else if (utility::ouput_file_redirection)
    {
        close(fd_out);
        dup2(dup_fd_out, STDOUT_FILENO);
        utility::fd_out = -1;
        utility::dup_fd_out = -1;
        utility::ouput_file_redirection = 0;
    }
}

void utility::sig_handler(int signal)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        ;
    std::cout << std::endl
              << "Background Process Finished" << std::endl
              << std::flush;
    utility::print_start_symbol();
}