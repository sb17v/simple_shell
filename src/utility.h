#ifndef UTILITY_H
#define UTILITY_H
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <dirent.h>
#include <wait.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

class utility
{
public:
  static std::vector<std::string> parse_input(const std::string &command_string, char delimeter);

  static bool is_input_redirection(std::string input_string);
  static bool is_output_redirection(std::string input_string);
  static bool is_piped_command(std::string input_string);
  static bool is_background_process(std::string input_string);
  static bool is_command_in_path(std::string command, std::string path);

  static std::string get_env_var(const std::string &var);
  static std::string get_cur_dir();
  static std::string get_cur_usr();
  static void print_environment();
  static void print_start_symbol();
  static std::string get_command_path(std::string command_name);

  static int fd_in;
  static int fd_out;
  static int dup_fd_in;
  static int dup_fd_out;
  static int input_file_redirection;
  static int ouput_file_redirection;
  static int input_output_file_redirection;

  static std::vector<std::string> handle_input_output_redirection(std::string input);
  static void close_input_output_redirection();

  static void sig_handler(int signal);
};
#endif