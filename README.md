# Lister - A custom implementation of the ls command in Linux

This project is a reimplementation of the familiar ls command on Linux, written in C, with the goal of building a modular, clean, and easily extensible code structure.

## Directory Structure

The project is organized into modules to clearly separate functionalities, making development, debugging, and maintenance much easier:

```
lister/
├── bin/
├── build/
├── src/
│ ├── main.c
│ ├── directory_reader/
│ ├── display/
│ ├── file_info/
│ └── options/
├── Makefile
├── .gitignore
└── README.md
```

## Component Description

- **`bin/`**: Contains the final executable file after a successful build. This is the complete product of the project.
- **`build/`**: Stores intermediate object files (.o) generated during the build process. This helps keep the source tree clean.
- **`src/`**: Contains all the source code of the project, divided into submodules: 
  - **`main.c`**: The entry point and main coordinator of the program.
  - **`options/`**: Module responsible for parsing command-line arguments (options) provided by the user.
  - **`directory_reader/`**: Module responsible for reading the contents of a directory and returning the list of files/subdirectories. 
  - **`file_info/`**: Module responsible for retrieving detailed information about a specific file (e.g., permissions, size, modification date...).
  - **`display/`**: Module responsible for formatting and displaying data to the screen.
- **`Makefile`**: The automated build script. It contains the rules to compile the source code from src/, generate object files in build/, and link them together into an executable in bin/.  
- **`.gitignore`**: Configuration file for Git to ignore unnecessary files and folders (such as bin/ and build/) when committing code.
- **`README.md`**: This file itself, providing an overview of the project. 

## History Feature

To enable command history logging, use the wrapper script:

1. **Install the wrapper:**
   ```bash
   ./install_history.sh
   ```
   This will add the wrapper function to your shell config (`~/.bashrc` or `~/.zshrc`).

2. **Restart your shell** or run:
   ```bash
   source ~/.zshrc  # or ~/.bashrc
   ```

3. **Use `lister` as usual** - commands and results will be automatically logged to `src/history.txt`.

The history file contains:
- Timestamp
- Full command with arguments
- Exit code
- Command output

To manually source the wrapper without installation:
```bash
source lister_history.sh
```

## How to Build the Project

