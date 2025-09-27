# Ton Programming Language

![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-2.0.0-orange.svg)

Ton is a modern, feature-rich programming language with advanced capabilities for system programming, web development, and general-purpose applications. It combines the best features of modern programming languages while maintaining simplicity and readability.

## 🚀 Features

### Core Language Features
- **Static Typing**: Strong type system with type inference and compile-time checks
- **Generic Programming**: Full support for generic types and functions with constraints
- **Object-Oriented Programming**: Classes, inheritance, polymorphism, and virtual methods
- **Functional Programming**: Lambda expressions, higher-order functions, and function composition
- **Memory Safety**: Built-in memory management with RAII and automatic cleanup
- **Concurrency**: Built-in threading, async/await, and synchronization primitives
- **Pattern Matching**: Powerful pattern matching capabilities
- **Module System**: Advanced module and namespace management
- **Exception Handling**: Comprehensive try-catch-finally exception system

### Advanced Type System
- **Primitives**: `int`, `float`, `string`, `bool`, `char`, `void`
- **Generic Types**: `Optional<T>`, `Result<T, E>`, `Tuple2<T1, T2>`, `Tuple3<T1, T2, T3>`
- **Collections**: `ArrayList<T>`, `HashMap<K, V>`, `HashSet<T>`, `Queue<T>`, `Stack<T>`
- **Pointers**: Type-safe pointer operations with null safety
- **Functions**: Function types, closures, and lambda expressions
- **Classes**: Object-oriented programming with inheritance and polymorphism

### Comprehensive Standard Library (TonLib)

#### Core Modules
- **Core**: Essential utilities and error handling
- **Types**: Generic type system and advanced type utilities
- **Collections**: Advanced data structures with generic support
- **Functional**: Lambda expressions, function composition, and higher-order functions
- **Concurrency**: Threading, async/await, and synchronization primitives

#### I/O and System
- **I/O**: Advanced file operations, streams, and file watching
- **System**: Operating system integration and namespace management
- **Network**: HTTP client/server, WebSocket, and networking utilities
- **Regex**: Regular expressions with pattern matching and validation

#### Utilities
- **DateTime**: Comprehensive date and time handling with timezone support
- **String**: Advanced string manipulation and processing
- **Math**: Mathematical functions, constants, and algorithms
- **JSON**: JSON parsing and serialization
- **Crypto**: Cryptographic functions (SHA-256, MD5, Base64)
- **Testing**: Unit testing framework with assertions

### Import TonLib Modules
```ton
// Core language features
import tonlib.types.generics;           // Generic types and collections
import tonlib.functional.lambda;       // Functional programming features
import tonlib.concurrency.threading;   // Threading and async/await

// Collections and data structures
import tonlib.collections.list;        // ArrayList implementation
import tonlib.collections.map;         // HashMap implementation
import tonlib.collections.dictionary;  // Advanced dictionary
import tonlib.collections.queue;       // Queue and PriorityQueue
import tonlib.collections.stack;       // Stack implementations

// I/O and system operations
import tonlib.io.advanced_io;          // Advanced file operations
import tonlib.system.namespace;        // Namespace management
import tonlib.system.module_manager;   // Module system

// Utilities
import tonlib.regex.regex;             // Regular expressions
import tonlib.datetime.datetime;       // Date and time handling
import tonlib.string.string;           // String manipulation
import tonlib.math.math;               // Mathematical functions
import tonlib.json.json;               // JSON processing
import tonlib.network.network;         // Networking
import tonlib.testing.testing;         // Unit testing
```

### Import TonCrypto Library
```ton
import "stdlib/toncrypto.ton"; // Cryptographic functions
```

## 📦 Installation

### Prerequisites
- GCC or Clang compiler
- Make build system
- Windows, Linux, or macOS

### Building from Source

```bash
# Clone the repository
git clone https://github.com/v3nn7/Ton.git
cd Ton

# Build the interpreter
make

# Run tests (optional)
make test
```

## 🏃‍♂️ Quick Start

### Hello World

Create a file `hello.ton`:

```ton
fn main() -> int {
    print("Hello, World!");
    return 0;
}
```

Run it:

```bash
./ton hello.ton
```

### Basic Examples

#### Variables and Types

```ton
fn main() -> int {
    let name: string = "Ton";
    let version: float = 1.0;
    let is_awesome: bool = true;
    
    print("Language: " + name);
    print("Version: " + string(version));
    
    return 0;
}
```

#### Functions and Control Flow

```ton
fn factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() -> int {
    for (let i: int = 1; i <= 5; i++) {
        print("Factorial of " + string(i) + " is " + string(factorial(i)));
    }
    
    return 0;
}
```

#### Exception Handling

```ton
fn divide(a: int, b: int) -> int {
    if (b == 0) {
        throw "Division by zero error";
    }
    return a / b;
}

fn main() -> int {
    try {
        let result: int = divide(10, 0);
        print("Result: " + string(result));
    } catch (error) {
        print("Error: " + error);
    }
    
    return 0;
}
```

#### Working with Collections

```ton
import "tonlib/collections/collections.ton";

fn main() -> int {
    // Create a list
    let numbers: TonList* = tonlist_create();
    tonlist_push(numbers, create_value_int(1));
    tonlist_push(numbers, create_value_int(2));
    tonlist_push(numbers, create_value_int(3));
    
    // Create a map
    let person: TonMap* = tonmap_create();
    tonmap_set(person, "name", create_value_string("John"));
    tonmap_set(person, "age", create_value_int(30));
    
    print("List size: " + string(tonlist_size(numbers)));
    print("Person name: " + value_to_string(tonmap_get(person, "name")));
    
    // Cleanup
    tonlist_destroy(numbers);
    tonmap_destroy(person);
    
    return 0;
}
```

## 📚 Documentation

- **[Language Reference (English)](TON_LANGUAGE_DOC_EN.md)** - Complete language specification
- **[Quick Start Guide](GUIDE.md)** - Get started with Ton
- **[Language Reference (Polish)](TON_LANGUAGE_DOC_PL.md)** - Pełna specyfikacja języka
- **[TonLib API Reference](tonlib/docs/API_REFERENCE.md)** - Standard library documentation
- **[Examples](tonlib/examples/)** - Code examples and tutorials

## 🏗️ Project Structure

```
ton-lang/
├── src/                    # Core interpreter source code
│   ├── lexer.c/h          # Lexical analysis
│   ├── parser.c/h         # Syntax analysis
│   ├── ast.c/h            # Abstract Syntax Tree
│   ├── interpreter_*.c/h  # Interpreter components
│   ├── memory.c/h         # Memory management
│   ├── error.c/h          # Error handling
│   └── main.c             # Entry point
├── tonlib/                # Standard library
│   ├── core/              # Core utilities
│   ├── collections/       # Data structures
│   ├── string/            # String operations
│   ├── math/              # Mathematical functions
│   ├── io/                # Input/Output
│   └── ...               # Other modules
├── stdlib/                # Built-in functions
├── tests/                 # Test files
└── docs/                  # Documentation
```

## 🧪 Testing

Run the test suite:

```bash
# Run all tests
make test

# Run specific test
./ton test_file.ton

# Run with memory debugging
make debug
./ton test_file.ton
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup

1. Fork the repository
2. Create a feature branch: `git checkout -b feature-name`
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass: `make test`
6. Submit a pull request

### Code Style

- Follow the existing C code style
- Use meaningful variable and function names
- Add comments for complex logic
- Update documentation for new features

## License

Copyright 2024 v3nn7 (Ton)

This project is licensed under the Apache License 2.0 - see the [LICENSE.md](LICENSE.md) file for details.

## 🙏 Acknowledgments

- Inspired by modern programming languages like Rust, Go, and TypeScript
- Built with performance and safety in mind
- Community-driven development

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/ton-lang/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/ton-lang/discussions)
- **Email**: support@ton-lang.org

---

**Happy coding with Ton! 🎉**