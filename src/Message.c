#include <stdio.h>

void message(FILE *output, const char *prompt)
{
    fprintf(output, "%s", prompt);
}