# Ton-Lang Documentation

## Introduction

Ton-Lang is a lightweight, embeddable scripting language designed for simplicity and ease of integration with C applications. It features a familiar C-like syntax, dynamic typing, and automatic memory management.

## Features

*   **C-like Syntax:** Easy to learn for developers familiar with C, Java, or JavaScript.
*   **Dynamic Typing:** Variables do not have a fixed type.
*   **Automatic Memory Management:** A custom tracking allocator is used to manage memory, helping to prevent leaks.
*   **Embeddable:** Designed to be easily integrated into C projects.
*   **Extensible:** Supports user-defined functions and data structures.

## Getting Started

### Compilation

To compile the Ton-Lang interpreter, you will need a C compiler (like GCC). The following command will compile the interpreter and all its modules:

```bash
gcc -o ton.exe main.c parser.c environment.c builtin.c memory.c collections.c builtin_tonlib.c sha256.c md5.c value.c ast.c token.c lexer.c interpreter.c bitops.c struct.c array.c -Iinclude
```

### Running a Script

To run a Ton-Lang script, pass the filename as an argument to the interpreter:

```bash
./ton.exe your_script.ton
```