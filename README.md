# BFI - Optimized Brainfuck Interpreter

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/itlwas/bfi)
[![License](https://img.shields.io/badge/license-Unlicense-blue.svg)](https://unlicense.org/)
[![Release](https://img.shields.io/badge/release-v1.0-orange.svg)](https://github.com/itlwas/bfi/releases)

A lightweight, efficient, and feature-rich Brainfuck interpreter implemented in C. BFI provides a robust platform for executing Brainfuck code with optimal performance while maintaining memory efficiency. This modular interpreter is designed for both educational purposes and practical use, offering developers a reliable tool to experiment with esoteric programming concepts through the minimalist but Turing-complete Brainfuck language.

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
- [Performance Benchmarking](#performance-benchmarking)
- [Implementation Details](#implementation-details)
  - [Optimizations](#optimizations)
  - [Error Handling](#error-handling)
- [Architecture](#architecture)
  - [Component Overview](#component-overview)
  - [Execution Flow](#execution-flow)
- [Performance Considerations](#performance-considerations)
- [Memory Management](#memory-management)
- [Testing](#testing)
- [Contributing](#contributing)
- [License](#license)

## Features

- **High-performance execution** with runtime optimization and instruction batching for repetitive operations
- **Dynamic memory management** with configurable initial (default: 30,000 cells) and maximum tape capacity (default: 1MB)
- **Comprehensive debugging** capabilities with color-coded memory visualization and execution tracing
- **Precise error detection** with categorized error codes (0x100-0x5FF) and detailed messages
- **Multiple EOF handling modes** (no change, 0, or 255) for flexible input processing
- **Execution limits** with configurable maximum iterations (default: 1 billion) to prevent infinite loops
- **Built-in benchmarking** with detailed performance metrics (execution time, IPS) and memory usage statistics
- **Minimal footprint** with efficient memory usage and optimized binary size
- **Portable C implementation** with no external dependencies besides standard libraries
- **Color-coded output** for enhanced debugging and visualization with terminal color support
- **Safe memory operations** with thorough error checking and protective boundaries
- **Optimized bracket handling** with pre-computed jump positions for efficient loop processing

## Installation

### Prerequisites

To build BFI, you need:
- C compiler (TCC recommended, GCC or Clang also supported)
- Make build system
- Standard C libraries (stdio.h, stdlib.h, string.h, stdbool.h)

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/itlwas/bfi.git
   cd bfi
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

| Option | Description                                                      |
|--------|------------------------------------------------------------------|
| `-h, --help` | Display help message and exit                              |
| `-v, --version` | Display version information and exit                    |
| `-d, --debug` | Enable debug mode with execution snapshots                |
| `-b, --benchmark` | Enable benchmark mode with performance metrics        |
| `-w, --snapshot-width <n>` | Set debug snapshot width (default: 5)        |
| `-i, --iterations <n>` | Set maximum iteration limit (default: 1 billion) |
| `-t, --initial-tape <n>` | Set initial tape capacity (default: 30000)     |
| `-M, --max-tape <n>` | Set maximum tape capacity (default: 1MB)           |
| `-e, --eof-mode <n>` | Set EOF handling mode (0-2)                        |

### EOF Handling Modes

- **Mode 0**: Leave the current cell unchanged when EOF is encountered (default)
- **Mode 1**: Set the current cell to 0 when EOF is encountered
- **Mode 2**: Set the current cell to 255 when EOF is encountered

### Examples

#### Running a Basic Brainfuck Program

Create a file named `hello.bf` with the following content:

```brainfuck
+++++++++[>++++++++>+++++++++++>+++++<<<-]>.>++.+++++++..+++.>-.
------------.<++++++++.--------.+++.------.--------.>+.
```

Run it using BFI:

```bash
bfi hello.bf
```

This will output: `Hello, World!`

#### Using Debug Mode

To understand the execution flow and memory state:

```bash
bfi -d hello.bf
```

This will display color-coded memory snapshots during execution, showing:
- Current instruction and position in the program
- Current iteration count
- Pointer location within the tape
- Cell values (both numeric and ASCII representation)
- Tape capacity and memory usage statistics

#### Configuring Execution Parameters

For a resource-intensive program requiring custom limits:

```bash
bfi -t 10000 -M 500000 -i 5000000 -e 1 complex_program.bf
```

This sets:
- Initial tape size: 10,000 cells
- Maximum tape size: 500,000 cells
- Maximum iterations: 5 million
- EOF handling: Set to 0 when EOF is encountered

### Debugging Tools

BFI provides several built-in debugging commands that can be included in your Brainfuck code:

- `!` - Prints current pointer position and cell value without disrupting execution
- `$` - Outputs detailed state information to stderr (instruction pointer, data pointer, current cell value)

These debug commands can be inserted anywhere in your Brainfuck code and will be interpreted as special instructions without affecting the program state.

## Performance Benchmarking

BFI includes a built-in benchmarking mode that provides detailed performance metrics:

```bash
bfi -b hello.bf
```

Benchmark output includes:
- Total execution time in seconds with millisecond precision
- Number of instructions executed
- Instructions per second (IPS) calculation
- Maximum memory usage during execution
- Initial and final tape sizes
- Memory scaling efficiency information

The benchmark mode is particularly useful for:
- Comparing optimization techniques
- Measuring the efficiency of different algorithmic approaches
- Profiling complex Brainfuck programs
- Determining execution bottlenecks

## Implementation Details

### Optimizations

BFI implements several optimizations to enhance performance:

- **Instruction batching**: Consecutive identical operations are processed in batches:
  - Multiple `+` or `-` are combined into a single addition/subtraction with a multiplier
  - Multiple `>` or `<` are consolidated into a single pointer movement with offset calculation
- **Bracket mapping**: Pre-computes jump positions for all loop structures during initialization, creating a direct jump table for O(1) jumps
- **Dynamic memory expansion**: Uses a doubling strategy for efficient memory allocation when more space is needed
- **Loop optimization**: Fast path for zero checks to optimize loop execution
- **Buffered I/O**: Uses line buffering for improved input/output performance
- **Minimal parsing overhead**: One-pass preprocessing for bracket validation and mapping
- **Memory boundary optimization**: Efficient tape growth strategy with safe bounds checking
- **Inline critical functions**: Performance-critical functions are inlined to reduce call overhead

### Error Handling

BFI includes a comprehensive error handling system with categorized error codes:

- **Syntax errors** (0x100-0x1FF): Bracket mismatches, invalid code structure
- **Runtime errors** (0x200-0x2FF): Tape underflow/overflow, execution limits exceeded
- **Memory errors** (0x300-0x3FF): Allocation failures, capacity limits reached
- **I/O errors** (0x400-0x4FF): File access issues, read/write problems
- **System errors** (0x500-0x5FF): Command-line arguments, environment issues

Each error includes:
- Detailed error message with context
- Color-coded category identification
- Specific error code for troubleshooting
- Contextual information (positions, file paths, etc.)

## Architecture

### Component Overview

The interpreter is organized into several modular components with clear responsibilities:

- `bfi.h`: Core definitions, constants, and configuration values
- `main.c`: Entry point, command-line processing, and execution orchestration
- `memory.c`: Safe memory allocation and deallocation with error handling
- `logger.c`: Comprehensive logging and error reporting with color-coded output
- `fileio.c`: File I/O operations with safety checks and validation
- `syntax.c`: Parser and bracket validation for syntactic correctness
- `utils.c`: Command-line processing and string manipulation utilities
- `executor.c`: Low-level tape operations and state management
- `interpreter.c`: Core execution engine with optimized instruction processing
- `benchmark.c`: Performance measurement and statistics reporting

The architecture incorporates both horizontal and vertical dependencies with a clear component hierarchy:

```
┌─────────────────────────────────────────────────────────────┐
│                           bfi.h                             │
└───────────┬───────────────────────────────┬─────────────────┘
            │                               │
            ▼                               ▼
┌───────────────────────┐       ┌─────────────────────────┐
│       memory.c        │◄──────┤        logger.c         │
└───────────┬───────────┘       └─────────┬───────────────┘
            │                             │
            │                             │
            ▼                             ▼
┌────────────────────────────────────────────────────────────┐
│                  Core System Services                      │
└───┬────────────────┬─────────────────┬────────────────┬────┘
    │                │                 │                │
    ▼                ▼                 ▼                ▼
┌─────────┐    ┌──────────┐     ┌────────────┐    ┌──────────┐
│ utils.c │    │ fileio.c │     │ syntax.c   │    │executor.c│
└────┬────┘    └────┬─────┘     └─────┬──────┘    └────┬─────┘
     │              │                 │                │
     │              │                 │                │
     └──────────────┴─────────┬───────┴────────────────┘
                              │
                              ▼
                     ┌────────────────┐
                     │ interpreter.c  │
                     └────────┬───────┘
                              │
                              ▼
                     ┌────────────────┐
                     │  benchmark.c   │
                     └────────────────┘
```

### Execution Flow

1. **Initialization**:
   - Parse command-line arguments with extensive validation
   - Configure execution environment (stdio buffers, limits, modes)
   - Load and verify source file content with safety checks and size validation

2. **Preprocessing**:
   - Parse Brainfuck source code to identify valid instructions
   - Validate bracket structure and ensure balanced pairs
   - Build jump table for efficient loop handling with direct position mapping

3. **Execution**:
   - Initialize tape with configurable initial capacity
   - Process instructions with optimized batching for repetitive operations
   - Perform bounds checking and dynamic resizing as needed
   - Track instruction count and enforce execution limits

4. **Memory Management**:
   - Dynamically adjust tape size with efficient doubling strategy
   - Enforce maximum memory limits to prevent resource exhaustion
   - Track memory usage statistics for reporting and benchmarking

5. **Output Generation**:
   - Process I/O operations with appropriate buffering
   - Generate debug information in color-coded format when enabled
   - Format and display benchmark results if requested

## Performance Considerations

BFI is optimized for both speed and memory efficiency:

- **Instruction consolidation**: Repeated operations are batched to reduce interpreter overhead
- **Efficient memory model**: Memory is allocated in chunks with exponential growth strategy
- **Jump table optimization**: Pre-computed jump positions eliminate runtime lookup overhead
- **Buffered I/O**: Uses line buffering for efficient input/output operations
- **Minimal parsing overhead**: One-pass preprocessing for structural validation
- **Reduced function call overhead**: Uses inline functions for frequently executed operations
- **Efficient tape access**: Direct memory manipulation for performance-critical operations
- **Optimized conditionals**: Fast paths for common cases to reduce branching

## Memory Management

BFI implements a sophisticated memory management system:

- **Initial allocation**: Starts with a configurable initial tape size (default: 30,000 cells)
- **Growth strategy**: Doubles capacity when more space is needed, ensuring amortized constant time expansion
- **Maximum limit**: Enforces a configurable maximum tape size (default: 1MB) to prevent memory exhaustion
- **Boundary handling**: Detects and prevents tape underflow with clear error reporting
- **Safe allocation**: All memory operations include thorough error checking and validation
- **Automatic cleanup**: Properly releases all allocated resources, even on error conditions
- **Memory tracking**: Reports peak memory usage and capacity statistics in benchmark mode
- **Zero initialization**: All newly allocated memory is initialized to zero to ensure predictable behavior

## Testing

BFI includes a test suite to verify correct behavior. To run the tests:

```bash
make test
```

Test files are located in the `tests/` directory and include:

- **Functionality tests**: Verify correct execution of standard Brainfuck operations
- **Edge case tests**: Validate behavior at boundary conditions
- **Performance tests**: Measure execution speed and memory usage
- **Error handling tests**: Confirm appropriate responses to invalid inputs
- **Compatibility tests**: Ensure consistent behavior across different environments

## Contributing

Contributions are welcome! Here's how you can help:

1. **Bug Reports**: Open an issue describing the bug, how to reproduce it, and expected behavior
2. **Feature Requests**: Suggest new features or improvements via the issue tracker
3. **Code Contributions**: Submit pull requests with bug fixes or new features
4. **Documentation**: Help improve or translate documentation
5. **Performance Optimization**: Identify and address performance bottlenecks

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