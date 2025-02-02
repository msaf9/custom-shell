/**
 * @file shell.c
 * @brief A simple custom shell implementation in C.
 *
 * This program implements a basic shell that continuously prompts the user for input,
 * reads the input, and displays it back to the user. The shell exits when the user
 * types "exit".
 *
 * Features:
 * - Displays a prompt "mysh>" for user input.
 * - Reads user input up to a maximum size of 1024 characters.
 * - Removes the newline character from the input.
 * - Exits the shell when the user types "exit".
 * - Displays the entered input as a placeholder for future execution logic.
 *
 * Usage:
 * Compile the program and run the executable. Type commands at the prompt and press
 * Enter. Type "exit" to terminate the shell.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024

int main()
{
    char input[MAX_INPUT_SIZE];

    while (1)
    {
        printf("mysh> "); // Display prompt
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
        {
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        // Remove newline character
        input[strcspn(input, "\n")] = '\0';

        // Exit condition
        if (strcmp(input, "exit") == 0)
        {
            printf("Exiting shell...\n");
            break;
        }

        printf("You entered: %s\n", input); // Placeholder for execution logic
    }

    return 0;
}
