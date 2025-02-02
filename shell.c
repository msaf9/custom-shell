/**
 * @file shell.c
 * @brief A simple custom shell implementation in C.
 *
 * This program implements a basic shell that continuously prompts the user for input,
 * reads the input, and processes it. The shell supports built-in commands like "exit"
 * and "cd", and can execute external commands.
 *
 * Features:
 * - Displays a prompt "mysh>" for user input.
 * - Reads user input up to a maximum size of 1024 characters.
 * - Removes the newline character from the input.
 * - Exits the shell when the user types "exit".
 * - Changes the current directory with the "cd" command.
 * - Executes external commands using fork and execvp.
 *
 * Usage:
 * Compile the program and run the executable. Type commands at the prompt and press
 * Enter. Type "exit" to terminate the shell.
 *
 * Functions:
 * - parse_input: Tokenizes the input string into arguments.
 * - execute_command: Forks a child process to execute external commands.
 * - handle_builtin: Handles built-in commands like "exit" and "cd".
 * - main: The main loop that displays the prompt, reads input, and processes commands.
 *
 * Compilation:
 * gcc -o shell shell.c
 *
 * Example:
 * ./shell
 * mysh> ls
 * mysh> cd ..
 * mysh> exit
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_COUNT 64

// Function to parse input into arguments
void parse_input(char *input, char **args)
{
    int i = 0;
    char *token = strtok(input, " "); // Tokenize input by spaces
    while (token != NULL)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the argument list
}

// Function to execute external commands
void execute_command(char **args)
{
    pid_t pid = fork();
    if (pid == -1)
    { // Fork failed
        perror("fork failed");
        return;
    }
    else if (pid == 0)
    { // Child process
        if (execvp(args[0], args) == -1)
        {
            perror("Command execution failed");
        }
        exit(EXIT_FAILURE);
    }
    else
    {               // Parent process
        wait(NULL); // Wait for the child to complete
    }
}

// Function to handle built-in commands
int handle_builtin(char **args)
{
    if (strcmp(args[0], "exit") == 0)
    { // Built-in exit command
        printf("Exiting shell...\n");
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0)
    { // Built-in cd command
        if (args[1] == NULL)
        {
            fprintf(stderr, "cd: missing argument\n");
        }
        else if (chdir(args[1]) != 0)
        {
            perror("chdir failed");
        }
        return 1; // Return 1 to indicate built-in command was handled
    }
    return 0; // Return 0 if it's not a built-in command
}

int main()
{
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARG_COUNT];

    while (1)
    {
        printf("mysh> "); // Display shell prompt

        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        // Ignore empty input
        if (strlen(input) == 0)
        {
            continue;
        }

        // Parse input into arguments
        parse_input(input, args);

        // Handle built-in commands
        if (handle_builtin(args))
        {
            continue; // Skip execution if it was a built-in command
        }

        // Execute external commands
        execute_command(args);
    }

    return 0;
}
