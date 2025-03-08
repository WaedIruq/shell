# shell

This is a simple shell-like command processor written in C. It supports basic shell features such as command execution, input/output redirection, background execution, and piping.

## Features

- **Command Execution**: Executes commands using `execvp`.
- **Input/Output Redirection**: Supports `<` for input redirection and `>` for output redirection.
- **Background Execution**: Supports `&` for running commands in the background.
- **Piping**: Supports `|` for piping the output of one command as input to another.
- **Signal Handling**: Ignores `SIGINT` (Ctrl+C) and `SIGCHLD` signals in the parent process to prevent interruption of background processes.

## Code Overview

The program consists of the following functions:

1. **`prepare()`**:
   - Sets up signal handling to ignore `SIGINT` and `SIGCHLD` in the parent process.

2. **`finalize()`**:
   - Placeholder for any cleanup operations (currently does nothing).

3. **`process_arglist(int count, char** arglist)`**:
   - Processes the command-line arguments and executes the appropriate actions based on the presence of special symbols (`<`, `>`, `|`, `&`).
   - Handles forking, piping, redirection, and background execution.
