# Ton Programming Language

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Version](https://img.shields.io/badge/version-1.0.0-orange.svg)

Ton is a modern, statically-typed programming language designed for simplicity, performance, and safety. It combines the best features of systems programming languages with modern language design principles.

## 🚀 Features

### Core Language Features
- **Static Typing**: Strong type system with type inference
- **Memory Safety**: Built-in memory management with automatic cleanup
- **Exception Handling**: Comprehensive try-catch-finally exception system
- **Functions**: First-class functions with closures and lambda expressions
- **Structures**: User-defined data types with methods
- **Pointers**: Low-level memory access with safety checks
- **Macros**: Compile-time code generation and metaprogramming

### Data Types
- **Primitives**: `int`, `float`, `string`, `bool`, `char`
- **Collections**: Arrays, Lists, Maps, Sets
- **Pointers**: Type-safe pointer operations
- **Functions**: Function types and closures
- **Structures**: Custom data structures

### Built-in Libraries (TonLib)
- **Core**: Essential utilities and error handling
- **Collections**: Advanced data structures (List, Map, Set)
- **String**: Comprehensive string manipulation
- **Math**: Mathematical functions and constants
- **I/O**: File and directory operations
- **System**: Operating system integration
- **Network**: HTTP client and networking utilities
- **JSON**: JSON parsing and serialization
- **Crypto**: Cryptographic functions (SHA-256, MD5, Base64)
- **Testing**: Unit testing framework

### Import Specific TonLib Modules
```ton
import "tonlib/core/core.ton";        // Core utilities
import "tonlib/math/math.ton";        // Mathematical functions
import "tonlib/string/string.ton";    // String manipulation
import "tonlib/collections/collections.ton"; // Data structures
import "tonlib/io/io.ton";           // File and I/O operations
import "tonlib/json/json.ton";       // JSON parsing and generation
import "tonlib/network/network.ton"; // HTTP and networking
import "tonlib/testing/testing.ton"; // Unit testing framework
import "tonlib/memory/memory.ton";   // Memory management
import "tonlib/system/system.ton";   // System operations
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