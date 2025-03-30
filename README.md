# BFI - Optimized Brainfuck Interpreter

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/yourusername/BFI)
[![License](https://img.shields.io/badge/license-Unlicense-blue.svg)](https://unlicense.org/)
[![Release](https://img.shields.io/badge/release-v1.0-orange.svg)](https://github.com/yourusername/BFI/releases)

A lightweight, efficient, and feature-rich Brainfuck interpreter implemented in C. BFI aims to provide a robust platform for executing Brainfuck code with optimal performance while maintaining memory efficiency. This interpreter is designed for both educational purposes and practical use, offering developers a reliable tool to experiment with esoteric programming concepts through the minimalist but Turing-complete Brainfuck language.

## Table of Contents

- [Features](#features)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Building from Source](#building-from-source)
- [Usage](#usage)
  - [Command Line Options](#command-line-options)
  - [EOF Handling Modes](#eof-handling-modes)
  - [Examples](#examples)
  - [Debugging Tools](#debugging-tools)
- [Implementation Details](#implementation-details)
- [Architecture](#architecture)
  - [Component Overview](#component-overview)
  - [Execution Flow](#execution-flow)
- [Performance Considerations](#performance-considerations)
- [Testing](#testing)
- [Contributing](#contributing)
- [License](#license)

## Features

- **High-performance execution** with optimized instruction handling
- **Dynamic memory management** with configurable tape size
- **Comprehensive debugging** capabilities with memory visualization
- **Error detection** for syntax issues and execution boundaries
- **Multiple EOF handling modes** for flexible input processing
- **Execution limits** to prevent infinite loops
- **Minimal footprint** with optimized binary size
- **Portable C implementation** with no external dependencies

## Installation

### Prerequisites

To build BFI, you need:
- C compiler (TCC recommended, GCC or Clang also supported)
- Make build system
- Standard C libraries

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/BFI.git
   cd BFI
   ```

2. Compile using the provided Makefile:
   ```bash
   make
   ```

3. Verify the installation:
   ```bash
   ./bfi --version
   ```

The provided Makefile uses TCC compiler with optimizations for a small binary footprint. If you prefer another compiler, you can modify the `CC` variable in the Makefile:

```bash
make CC=gcc
```

## Usage

The basic syntax for running BFI is:

```
bfi [options] <source_file>
```

### Command Line Options

| Option | Description |
|--------|-------------|
| `-d` | Enable debug mode with execution snapshots |
| `-w <n>` | Set debug snapshot width (default: 5) |
| `-t <n>` | Set initial tape capacity (default: 30000) |
| `-M <n>` | Set maximum tape capacity (default: 1MB) |
| `-i <n>` | Set maximum iteration limit (default: 1 billion) |
| `-e <n>` | Set EOF handling mode (0-2) |

### EOF Handling Modes

- **Mode 0**: Return 0 when EOF is encountered
- **Mode 1**: No change to the current cell when EOF is encountered
- **Mode 2**: Return 255 when EOF is encountered

### Examples

#### Running a Basic Brainfuck Program

Create a file named `hello.bf` with the following content:

```brainfuck
+++++++++[>++++++++>+++++++++++>+++++<<<-]>.>++.+++++++..+++.>-.
------------.<++++++++.--------.+++.------.--------.>+.
```

Then run it using BFI:

```bash
bfi hello.bf
```

This will output: `Hello, World!`

#### Using Debug Mode

To understand the execution flow and memory state:

```bash
bfi -d hello.bf
```

This will display memory snapshots during execution, showing the tape state and instruction pointer.

#### Configuring Execution Parameters

For a resource-intensive program requiring custom limits:

```bash
bfi -t 10000 -M 500000 -i 5000000 -e 1 complex_program.bf
```

This sets:
- Initial tape size: 10,000 cells
- Maximum tape size: 500,000 cells
- Maximum iterations: 5 million
- EOF handling: No change to current cell

### Debugging Tools

BFI provides several built-in debugging commands that can be included in your Brainfuck code:

- `!` - Prints current pointer position and cell value
- `$` - Outputs state information to stderr

## Implementation Details

BFI implements several optimizations to enhance performance:

- **Bracket mapping**: Pre-computes jump positions for loop structures during initialization
- **Command optimization**: Combines consecutive identical instructions (e.g., `+++` is processed as a single add-3 operation)
- **Dynamic memory**: Automatically expands the tape as needed while respecting defined limits
- **Execution limits**: Prevents infinite loops and excessive memory usage
- **Error handling**: Provides detailed error messages for syntax issues and runtime problems

## Architecture

### Component Overview

The interpreter is organized into several modular components:

- `interpreter.c`: Core execution engine that processes Brainfuck commands
- `syntax.c`: Parser and bracket validation for ensuring code correctness
- `memory.c`: Memory allocation and management for tape operations
- `logger.c`: Error reporting and logging facilities
- `fileio.c`: Source file handling for reading Brainfuck programs
- `utils.c`: Utility functions for string processing and argument handling
- `main.c`: Entry point and command-line processing

```
┌─────────────┐
│   main.c    │
└──────┬──────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│  fileio.c   │◄───┤  memory.c   │
└──────┬──────┘    └─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│  syntax.c   │◄───┤  logger.c   │
└──────┬──────┘    └─────────────┘
       │
       ▼
┌─────────────┐    ┌─────────────┐
│interpreter.c│◄───┤   utils.c   │
└─────────────┘    └─────────────┘
```

### Execution Flow

1. **Initialization**: Parse command-line arguments and load the source file
2. **Preprocessing**: Validate syntax and build bracket mapping
3. **Execution**: Process Brainfuck commands with optimization
4. **Memory Management**: Dynamically adjust tape size as needed
5. **Output Generation**: Process output commands and debug information

## Performance Considerations

BFI is optimized for both speed and memory efficiency:

- The interpreter processes consecutive identical commands as a single operation
- Memory is allocated dynamically but within configurable limits
- Preprocessing step builds an efficient jump table for loops
- Minimal runtime overhead with optimized function calls

## Testing

BFI includes a test suite to verify correct behavior. To run the tests:

```bash
make test
```

Test files are located in the `tests/` directory and include:

- Basic functionality tests
- Edge case handling
- Performance benchmarks
- Compatibility tests with standard Brainfuck programs

## Contributing

Contributions are welcome! Here's how you can help:

1. **Bug Reports**: Open an issue describing the bug, how to reproduce it, and expected behavior
2. **Feature Requests**: Suggest new features or improvements via the issue tracker
3. **Code Contributions**: Submit pull requests with bug fixes or new features
4. **Documentation**: Help improve or translate documentation

### Contribution Guidelines

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Commit your changes: `git commit -am 'Add feature'`
4. Push to the branch: `git push origin feature-name`
5. Submit a pull request

Please ensure your code follows the existing style conventions and includes appropriate tests.

## License

This project is open-source and available under the Unlicense. See the [LICENSE](LICENSE) file for details.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org/> 