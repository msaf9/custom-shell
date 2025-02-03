/**
 * @file shell.c
 * @brief A simple custom shell implementation in C.
 *
 * This program implements a basic shell that can execute commands with support for:
 * - Background execution using '&'
 * - Input redirection using '<'
 * - Output redirection using '>'
 * - Piped commands using '|'
 * - Built-in commands like 'cd' and 'exit'
 *
 * The shell reads user input, parses it to identify special symbols, and executes the commands accordingly.
 *
 * Functions:
 * - parse_input: Parses the input string into arguments and detects special symbols for background execution, redirection, and piping.
 * - execute_command: Executes a command with optional input/output redirection.
 * - execute_piped_command: Executes two commands connected by a pipe.
 * - handle_builtin: Handles built-in shell commands like 'cd' and 'exit'.
 * - main: The main loop of the shell that reads user input and executes commands.
 *
 * Usage:
 * - Compile the program using a C compiler (e.g., gcc).
 * - Run the compiled executable to start the custom shell.
 * - Enter commands at the prompt (mysh>).
 * - Use '&' for background execution, '<' for input redirection, '>' for output redirection, and '|' for piping commands.
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

// Function to parse input into arguments and detect special symbols
void parse_input(char *input, char **args, int *is_background, char **input_file, char **output_file, int *is_piped, char **pipe_cmd)
{
    int i = 0;
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        if (strcmp(token, "&") == 0)
        { // Background execution
            *is_background = 1;
        }
        else if (strcmp(token, "<") == 0)
        { // Input redirection
            token = strtok(NULL, " ");
            *input_file = token;
        }
        else if (strcmp(token, ">") == 0)
        { // Output redirection
            token = strtok(NULL, " ");
            *output_file = token;
        }
        else if (strcmp(token, "|") == 0)
        { // Piping
            *is_piped = 1;
            token = strtok(NULL, ""); // Remaining part is the second command
            *pipe_cmd = token;
            break; // Stop parsing after pipe
        }
        else
        {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the argument list
}

// Function to execute commands with redirection
void execute_command(char **args, int is_background, char *input_file, char *output_file)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    { // Child process
        if (input_file)
        { // Handle input redirection
            int fd = open(input_file, O_RDONLY);
            if (fd == -1)
            {
                perror("Input file opening failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (output_file)
        { // Handle output redirection
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                perror("Output file opening failed");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        if (execvp(args[0], args) == -1)
        {
            perror("Command execution failed");
        }
        exit(EXIT_FAILURE);
    }
    else
    {
        if (!is_background)
        {
            waitpid(pid, NULL, 0); // Parent waits unless it's a background process
        }
    }
}

// Function to execute piped commands
void execute_piped_command(char **args, char *pipe_cmd)
{
    int pipe_fd[2];
    char *pipe_args[MAX_ARG_COUNT];

    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe failed");
        return;
    }

    pid_t pid1 = fork();
    if (pid1 == -1)
    {
        perror("fork failed");
        return;
    }
    else if (pid1 == 0)
    {                                    // First command (writes to pipe)
        close(pipe_fd[0]);               // Close unused read end
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect stdout to pipe
        close(pipe_fd[1]);

        if (execvp(args[0], args) == -1)
        {
            perror("First command execution failed");
            exit(EXIT_FAILURE);
        }
    }

    pid_t pid2 = fork();
    if (pid2 == -1)
    {
        perror("fork failed");
        return;
    }
    else if (pid2 == 0)
    {                                   // Second command (reads from pipe)
        close(pipe_fd[1]);              // Close unused write end
        dup2(pipe_fd[0], STDIN_FILENO); // Redirect stdin to pipe
        close(pipe_fd[0]);

        parse_input(pipe_cmd, pipe_args, NULL, NULL, NULL, NULL, NULL);
        if (execvp(pipe_args[0], pipe_args) == -1)
        {
            perror("Second command execution failed");
            exit(EXIT_FAILURE);
        }
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

// Function to handle built-in commands
int handle_builtin(char **args)
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
    return 0;
}

int main()
{
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARG_COUNT];
    int is_background;
    char *input_file, *output_file, *pipe_cmd;
    int is_piped;

    while (1)
    {
        printf("mysh> ");
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        input[strcspn(input, "\n")] = '\0'; // Remove newline character
        if (strlen(input) == 0)
            continue;

        // Reset parsing variables
        is_background = 0;
        input_file = output_file = pipe_cmd = NULL;
        is_piped = 0;

        // Parse user input
        parse_input(input, args, &is_background, &input_file, &output_file, &is_piped, &pipe_cmd);

        // Handle built-in commands
        if (handle_builtin(args))
        {
            continue;
        }

        if (is_piped)
        {
            execute_piped_command(args, pipe_cmd);
        }
        else
        {
            execute_command(args, is_background, input_file, output_file);
        }
    }

    return 0;
}
