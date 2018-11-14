#ifndef COMMANDS_H
#define COMMANDS_H

#include <iostream>
#include <string>
#include <set>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utility.h"

class command
{
public:
  command(std::vector<std::string> &command_list);
  ~command();

  inline std::string get_command_name() { return command_name; }
  inline std::vector<std::string> get_command_arguments() { return command_arguments; }

private:
  std::string command_name;
  std::vector<std::string> command_arguments;
};

class piped_commands
{
public:
  piped_commands();
  ~piped_commands();

  inline std::vector<command *> get_command_list() { return cmd_list; }
  void set_command_list(command *cmd);

private:
  std::vector<command *> cmd_list;
};

class command_exec
{
public:
  command_exec();
  ~command_exec();

  int execute(command *cmd, bool is_background_process);
  int execute(piped_commands *pipe_cmd);

private:
  command *cmd;
  int execute_exit(command *cmd);
  int execute_cd(command *cmd);
  int execute_pwd(command *cmd);
  int execute_set(command *cmd);
  int execute_others(command *cmd, bool is_background_process);
};

#endif