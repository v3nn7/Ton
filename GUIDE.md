# Ton Language - Quick Start Guide

This guide will help you get started with the Ton programming language. It covers installation, basic syntax, and how to run your first Ton program.

## 1. Installation

To use Ton, you need to build it from source. Follow these steps:

### Prerequisites
- **GCC or Clang**: A C compiler is required.
- **Make**: A build automation tool.

### Steps
1. **Clone the repository** (replace `yourusername` with the actual repository owner):
   ```bash
   git clone https://github.com/v3nn7/Ton.git
   cd ton
   ```
2. **Build the interpreter**:
   ```bash
   make
   ```
   This will compile the Ton interpreter and create an executable named `ton.exe` (or `ton` on Linux/macOS) in the project root directory.

## 2. Your First Ton Program: "Hello, World!"

Let's write a simple program to print "Hello, World!" to the console.

1. **Create a new file** named `hello.ton` in your project directory.

2. **Add the following code** to `hello.ton`:
   ```ton
   fn main() -> int {
       print("Hello, World!");
       return 0;
   }
   ```
   - `fn main() -> int`: Defines the main function, the entry point of the program, which returns an integer.
   - `print("Hello, World!")`: Prints the string "Hello, World!" to standard output.
   - `return 0`: Indicates successful program execution.

## 3. Running Your Ton Program

To execute your `hello.ton` program:

1. **Open your terminal or command prompt**.

2. **Navigate to the directory** where you saved `hello.ton`.

3. **Run the program** using the Ton interpreter:
   ```bash
   ./ton hello.ton
   ```
   On Windows, you might use:
   ```bash
   .\ton.exe hello.ton
   ```

### Expected Output

```
Hello, World!
```

## 4. Basic Syntax Overview

### Variables
Declare variables using `let` and specify their type:

```ton
let age: int = 30;
let name: string = "Alice";
let is_active: bool = true;
```

### Functions
Define functions with `fn`, specifying parameter types and return type:

```ton
fn add(a: int, b: int) -> int {
    return a + b;
}

let sum: int = add(5, 10);
print(sum); // Output: 15
```

### Control Flow

#### If-Else Statements
```ton
if (age >= 18) {
    print("Adult");
} else {
    print("Minor");
}
```

#### For Loops
```ton
for (let i: int = 0; i < 5; i++) {
    print(i);
} // Output: 0, 1, 2, 3, 4
```

### Exception Handling
Use `try-catch-finally` to manage errors:

```ton
fn divide(a: int, b: int) -> int {
    if (b == 0) {
        throw "Division by zero error";
    }
    return a / b;
}

try {
    let result: int = divide(10, 0);
    print("Result: " + string(result));
} catch (error) {
    print("Error: " + error);
} finally {
    print("Operation completed.");
}
// Expected Output:
// Error: Division by zero error
// Operation completed.
```

## Next Steps

- Explore the [Language Reference (English)](TON_LANGUAGE_DOC_EN.md) for a complete overview of Ton's features.
- Check out the [TonLib API Reference](tonlib/docs/API_REFERENCE.md) for details on the standard library.
- Review the [Examples](tonlib/examples/) for more code demonstrations.

Happy coding with Ton! 🎉