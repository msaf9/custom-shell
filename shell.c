/**
 * @file shell.c
 * @brief A simple custom shell implementation in C.
 *
 * This program implements a basic shell that can execute commands with support for:
 * - Command history
 * - Background execution using '&'
 * - Input redirection using '<'
 * - Output redirection using '>'
 * - Piped commands using '|'
 * - Built-in commands like 'cd', 'exit', and 'history'
 * - Re-execution of commands from history using '!<number>'
 *
 * The shell reads user input, parses it to identify special symbols, and executes the commands accordingly.
 *
 * Functions:
 * - add_to_history: Saves commands in history.
 * - print_history: Displays the command history.
 * - parse_input: Parses the input string into commands, supporting multiple pipes.
 * - parse_command: Parses a single command into arguments, detecting background execution, input, and output redirection.
 * - execute_command: Executes a command with optional input/output redirection.
 * - execute_piped_commands: Executes a pipeline of commands.
 * - handle_builtin: Handles built-in shell commands like 'cd', 'exit', and 'history'.
 * - main: The main loop of the shell that reads user input and executes commands.
 *
 * Usage:
 * - Compile the program using a C compiler (e.g., gcc).
 * - Run the compiled executable to start the custom shell.
 * - Enter commands at the prompt (mysh>).
 * - Use '&' for background execution, '<' for input redirection, '>' for output redirection, and '|' for piping commands.
 * - Use 'history' to view command history and '!<number>' to re-execute a command from history.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_COUNT 64
#define MAX_HISTORY 100

char history[MAX_HISTORY][MAX_INPUT_SIZE]; // Command history
int history_count = 0;

// Function to save commands in history
void add_to_history(const char *input)
{
    if (history_count < MAX_HISTORY)
    {
        strcpy(history[history_count], input);
        history_count++;
    }
    else
    {
        // Shift commands up when history is full
        for (int i = 1; i < MAX_HISTORY; i++)
        {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[MAX_HISTORY - 1], input);
    }
}

// Function to display command history
void print_history()
{
    for (int i = 0; i < history_count; i++)
    {
        printf("[%d] %s\n", i + 1, history[i]);
    }
}

// Function to parse input into commands, supporting multiple pipes
void parse_input(char *input, char *commands[], int *command_count)
{
    *command_count = 0;
    char *token = strtok(input, "|");

    while (token != NULL)
    {
        commands[(*command_count)++] = token;
        token = strtok(NULL, "|");
    }
    commands[*command_count] = NULL;
}

// Function to parse a single command into arguments
void parse_command(char *command, char **args, int *is_background, char **input_file, char **output_file)
{
    int i = 0;
    char *token = strtok(command, " ");
    *is_background = 0;
    *input_file = *output_file = NULL;

    while (token != NULL)
    {
        if (strcmp(token, "&") == 0)
        {
            *is_background = 1;
        }
        else if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " ");
            if (!token)
            {
                fprintf(stderr, "Syntax error: expected input file after '<'\n");
                return;
            }
            *input_file = token;
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " ");
            if (!token)
            {
                fprintf(stderr, "Syntax error: expected output file after '>'\n");
                return;
            }
            *output_file = token;
        }
        else
        {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

// Function to execute a command with redirection
void execute_command(char **args, int is_background, char *input_file, char *output_file)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Fork failed");
        return;
    }
    else if (pid == 0)
    { // Child process
        if (input_file)
        {
            int fd = open(input_file, O_RDONLY);
            if (fd == -1)
            {
                perror("Error opening input file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (output_file)
        {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("Error opening output file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (execvp(args[0], args) == -1)
        {
            fprintf(stderr, "Command execution failed: %s\n", args[0]);
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        if (!is_background)
        {
            waitpid(pid, NULL, 0);
        }
    }
}

// Function to execute a pipeline of commands
void execute_piped_commands(char *commands[], int command_count)
{
    int pipe_fd[2], prev_fd = 0;
    pid_t pid;

    for (int i = 0; i < command_count; i++)
    {
        char *args[MAX_ARG_COUNT], *input_file = NULL, *output_file = NULL;
        int is_background = 0;

        parse_command(commands[i], args, &is_background, &input_file, &output_file);

        if (pipe(pipe_fd) == -1)
        {
            perror("Pipe failed");
            return;
        }

        pid = fork();
        if (pid == -1)
        {
            perror("Fork failed");
            return;
        }
        else if (pid == 0)
        { // Child process
            if (i > 0)
            { // Not the first command
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }
            if (i < command_count - 1)
            { // Not the last command
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }

            close(pipe_fd[0]);

            if (execvp(args[0], args) == -1)
            {
                perror("Command execution failed");
                exit(EXIT_FAILURE);
            }
        }

        close(pipe_fd[1]); // Close write end in parent
        if (i > 0)
        {
            close(prev_fd); // Close previous read end
        }
        prev_fd = pipe_fd[0]; // Save read end for next iteration
    }

    close(prev_fd); // Close last read end

    for (int i = 0; i < command_count; i++)
    {
        wait(NULL);
    }
}

// Function to execute built-in commands
int handle_builtin(char *input, char **args)
{
    if (strcmp(args[0], "exit") == 0)
    {
        printf("Exiting shell...\n");
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            fprintf(stderr, "cd: missing argument\n");
        }
        else if (chdir(args[1]) != 0)
        {
            perror("chdir failed");
        }
        return 1;
    }
    else if (strcmp(args[0], "history") == 0)
    {
        print_history();
        return 1;
    }
    else if (args[0][0] == '!')
    { // Re-execute a command from history
        int index = atoi(args[0] + 1);
        if (index < 1 || index > history_count)
        {
            fprintf(stderr, "No such command in history\n");
        }
        else
        {
            printf("Executing: %s\n", history[index - 1]);
            char temp_input[MAX_INPUT_SIZE];
            strcpy(temp_input, history[index - 1]);
            temp_input[strcspn(temp_input, "\n")] = '\0'; // Remove newline
            strcpy(input, temp_input);                    // Replace input with history command
            char *new_args[MAX_ARG_COUNT];
            int new_is_background;
            char *new_input_file, *new_output_file;
            parse_command(input, new_args, &new_is_background, &new_input_file, &new_output_file);
            if (handle_builtin(input, new_args))
                return 1;
            execute_command(new_args, new_is_background, new_input_file, new_output_file);
            return 1;
            return 0;
        }
    }
    return 0;
}

int main()
{
    char input[MAX_INPUT_SIZE], *commands[MAX_ARG_COUNT];
    int command_count;

    while (1)
    {
        printf("mysh> ");
        if (!fgets(input, MAX_INPUT_SIZE, stdin))
        {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        input[strcspn(input, "\n")] = '\0'; // Remove newline
        if (strlen(input) == 0)
            continue;

        add_to_history(input);

        parse_input(input, commands, &command_count);

        if (command_count > 1)
        {
            execute_piped_commands(commands, command_count);
        }
        else
        {
            char *args[MAX_ARG_COUNT];
            int is_background;
            char *input_file, *output_file;

            parse_command(commands[0], args, &is_background, &input_file, &output_file);
            if (handle_builtin(input, args))
                continue;

            execute_command(args, is_background, input_file, output_file);
        }
    }
}
