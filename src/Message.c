/**
 * @file Message.c
 * @brief This file contains the implementation of a simple message printing function.
 */

/**
 * @brief Prints a message to the specified output stream.
 *
 * This function takes a file pointer and a prompt string as arguments,
 * and prints the prompt string to the specified output stream.
 *
 * @param output A pointer to a FILE object that identifies the output stream.
 * @param prompt A constant character pointer to the message string to be printed.
 */

#include <stdio.h>

void message(FILE *output, const char *prompt)
{
    fprintf(output, "%s", prompt);
}
