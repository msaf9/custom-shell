# Custom Shell

## Overview
This project is a simple custom shell implemented in C. The shell supports various features such as:

- Command execution
- Background execution using `&`
- Input redirection using `<`
- Output redirection using `>`
- Piped commands using `|`
- Built-in commands: `cd`, `exit`, and `history`
- Command history tracking
- Re-execution of commands from history using `!<number>`

## Features
### 1. Command Execution
The shell allows users to execute standard shell commands by typing them into the prompt.

### 2. Background Execution
Appending `&` to a command runs it in the background.

### 3. Input and Output Redirection
- `< filename` redirects input from a file.
- `> filename` redirects output to a file.

### 4. Piped Commands
The shell supports multiple piped commands (e.g., `ls | grep txt`).

### 5. Built-in Commands
- `cd <directory>`: Changes the current working directory.
- `exit`: Exits the shell.
- `history`: Displays a list of previously executed commands.
- `!<number>`: Re-executes a command from history.

## Compilation and Execution
### Compiling the Shell
```sh
gcc shell.c -o mysh
```

### Running the Shell
```sh
./mysh
```

## Usage
1. Run the shell using `./mysh`.
2. Enter commands at the `mysh>` prompt.
3. Use built-in commands as needed.
4. Use `CTRL+C` to terminate a running foreground process.

## Code Structure
### Files
- `shell.c`: Main source file for the shell implementation.
- `shell.h`: Header file containing function declarations and constants.
- `Makefile`: Automates the build process.

### Functions
- `add_to_history`: Stores commands in history.
- `print_history`: Displays command history.
- `parse_input`: Splits input into commands, supporting pipes.
- `parse_command`: Parses a command, handling arguments and redirections.
- `execute_command`: Runs a command with optional redirection.
- `execute_piped_commands`: Executes a sequence of piped commands.
- `handle_builtin`: Processes built-in commands like `cd`, `exit`, and `history`.
- `main`: Main loop handling user input and executing commands.

## Example Commands
```sh
mysh> ls -l
mysh> echo "Hello World" > output.txt
mysh> cat < output.txt
mysh> history
mysh> !2  # Re-executes the second command from history
mysh> ls | grep .c
mysh> ./long_running_process &  # Runs in the background
```

## Limitations
- No advanced error handling for deeply nested pipes.
- Limited support for environment variable expansion.
- No tab-completion or advanced interactive features.
- Background processes do not support job control.

## Future Enhancements
- Implement environment variable support.
- Add tab-completion.
- Improve error handling and reporting.
- Support job control for background processes.

## License


