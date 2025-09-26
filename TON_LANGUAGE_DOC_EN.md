# Ton Language Documentation

## Table of Contents
1.  Introduction
2.  Data Types and Literals
3.  Variable Declarations
4.  Operators
5.  Control Flow Statements
6.  Functions
7.  Built-in Functions
8.  Code Examples
9.  Compilation and Running Programs

## 1. Introduction
Ton is a simple programming language designed for educational and demonstrative purposes. It features basic programming constructs such as variables, data types, operators, control flow statements, and functions. This document aims to provide a comprehensive specification of the Ton language to facilitate its learning and use.

## 2. Data Types and Literals

The Ton language supports the following basic data types:

*   **int**: Integer numbers, e.g., `10`, `-5`, `0`.
*   **float**: Floating-point numbers, e.g., `3.14`, `-0.5`, `1.0`.
*   **string**: Character strings, enclosed in double quotes, e.g., `"Hello world!"`, `"Ton"`.
*   **bool**: Boolean values, `true` or `false`.
*   **char**: Single characters, enclosed in single quotes, e.g., `'a'`, `'Z'`.
*   **pointer**: Raw pointer type for low-level memory operations.


## 3. Variable Declarations

In the Ton language, variables are declared using the keywords `let` (for mutable variables) or `const` (for constants, immutable variables). A variable declaration requires specifying its name, type, and optionally an initial value.

### Syntax

```ton
let <variable_name>: <type> = <initial_value>;
const <constant_name>: <type> = <initial_value>;
```

*   `<variable_name>` / `<constant_name>`: A unique name identifying the variable or constant.
*   `<type>`: The data type of the variable (e.g., `int`, `float`, `string`, `bool`, `char`).
*   `<initial_value>`: An optional value assigned to the variable during declaration. For `const`, it is mandatory.

### Examples

```ton
let age: int = 30;
let name: string = "Anna";
const PI: float = 3.14159;
let isActive: bool = true;

// Declaration without initialization (only for let)
let result: int;
result = 100;

// Error: attempt to change a constant value
// PI = 3.0;
```


## 4. Operators

Ton supports arithmetic, comparison, logical, assignment (including compound), increment/decrement, member/index access and pointer operators.

### Arithmetic Operators
Used to perform basic mathematical operations.

*   `+`: Addition (e.g., `a + b`)
*   `-`: Subtraction (e.g., `a - b`)
*   `*`: Multiplication (e.g., `a * b`)
*   `/`: Division (e.g., `a / b`)
*   `%`: Modulo (remainder) (e.g., `a % b`)

### Comparison Operators
Used to compare values and return a boolean value (`true` or `false`).

*   `==`: Equal to (e.g., `a == b`)
*   `!=`: Not equal to (e.g., `a != b`)
*   `<`: Less than (e.g., `a < b`)
*   `>`: Greater than (e.g., `a > b`)
*   `<=`: Less than or equal to (e.g., `a <= b`)
*   `>=`: Greater than or equal to (e.g., `a >= b`)

### Logical Operators
Used to combine or negate boolean expressions.

*   `&&`: Logical AND (e.g., `a && b`)
*   `||`: Logical OR (e.g., `a || b`)
*   `!`: Logical NOT (negation) (e.g., `!a`)

### Assignment Operators

*   `=`: Simple assignment (e.g., `variable = value;`)
*   `+=`, `-=`, `*=`, `/=`, `%=`: Compound assignments, e.g., `x += 2` (equivalent to `x = x + 2`)

### Increment and Decrement

*   `++x`, `x++`: Pre/post-increment
*   `--x`, `x--`: Pre/post-decrement

### Access Operators

*   `.`: Member access (e.g., `object.field`)
*   `[]`: Indexing (e.g., `arr[0]`)
*   `->`: Pointer member access (e.g., `ptr->field`)

### Pointer Operators

*   `&`: Address-of
*   `*`: Dereference


## 5. Control Flow Statements

The Ton language offers basic control flow statements that allow for conditional code execution and looping.

### `if-else` Conditional Statement
The `if-else` statement allows a block of code to be executed if a given condition is true, and optionally another block if the condition is false.

#### Syntax

```ton
if (<condition>) {
    // code to execute if the condition is true
} else {
    // code to execute if the condition is false (optional)
}
```

#### Examples

```ton
let x: int = 10;

if (x > 5) {
    print("x is greater than 5");
} else {
    print("x is less than or equal to 5");
}

let y: int = 3;
if (y % 2 == 0) {
    print("y is even");
} else {
    print("y is odd");
}
```

### `while` Loop
The `while` loop executes a block of code as long as a specified condition is true.

#### Syntax

```ton
while (<condition>) {
    // code to execute in the loop
}
```

#### Examples

```ton
let i: int = 0;
while (i < 5) {
    print("Iteration: " + i);
    i = i + 1;
}
```

### Loops: `while`, `for`, `loop`

- `while (cond) { ... }` — classic conditional loop.
- `for (init; cond; step) { ... }` — C-style loop (supported by the parser).
- `loop { ... }` — infinite loop, exit with `break`.

#### Syntax

```ton
while (i < 10) { ... }

for (let i: int = 0; i < 10; i = i + 1) {
    // loop body
}

loop {
    // loop body
    if (<exit_condition>) break;
}
```

#### break and continue
- `break;` — immediately exits the nearest loop.
- `continue;` — skips the current iteration and proceeds to the next one.

```ton
for (let i: int = 0; i < 10; i = i + 1) {
    if (i % 2 == 0) continue; // skip even
    if (i > 7) break;         // exit after 7
    print(i);
}
```


## 6. Functions

Functions in the Ton language allow you to group code for reuse and organization. Functions can accept arguments and return values.

### Function Declaration

#### Syntax

```ton
fn <function_name>(<parameter1>: <type1>, <parameter2>: <type2>, ...) -> <return_type> {
    // function body
    return <value>; // optional
}
```

*   `<function_name>`: The name of the function.
*   `<parameterN>`: The name of the parameter.
*   `<typeN>`: The data type of the parameter.
*   `<return_type>`: The data type of the value returned by the function. If the function returns nothing, you can use `void` or omit `-> <return_type>`.

### Function Call
Functions are called by specifying their name followed by a list of arguments in parentheses.

#### Syntax

```ton
<function_name>(<argument1>, <argument2>, ...);
```

#### Examples

```ton
fn add(a: int, b: int) -> int {
    return a + b;
}

fn greet(name: string) {
    print("Hello, " + name + "!");
}

let sum: int = add(5, 3);
print("Sum: " + sum);

greet("Alice");
```

### `return` Statement
The `return` statement is used to return a value from a function and immediately terminate its execution.

#### Syntax

```ton
return <value>;
```

#### Examples

```ton
fn checkPositive(number: int) -> bool {
    if (number > 0) {
        return true;
    } else {
        return false;
    }
}

let isPositive: bool = checkPositive(10);
print("Is positive: " + isPositive);
```


## 7. Built-in Functions

The Ton language provides several built-in functions that facilitate user interaction and data display.

### `print(<value>)`
The `print` function is used to display values on standard output (console). It can accept arguments of various data types.

#### Syntax

```ton
print(<value>);
```

#### Examples

```ton
print("Hello, Ton!");
let number: int = 123;
print(number);
let truth: bool = true;
print(truth);
```

### `read_line()`
The `read_line` function is used to read a line of text from standard input (console) and returns it as a `string` value.

#### Syntax

```ton
read_line();
```

#### Examples

```ton
print("Enter your name:");
let user_name: string = read_line();
print("Hello, " + user_name + "!");

print("Enter your age:");
let age_str: string = read_line();
// String to int conversion (if available in Ton, otherwise would require an additional function)
// let age: int = to_int(age_str);
// print("Your age is: " + age);
```

### Bitwise operations (built-in functions)
These functions operate on integers and return an int. They validate the number of arguments and require integer arguments.

- bit_and(a, b) -> a & b
- bit_or(a, b)  -> a | b
- bit_xor(a, b) -> a ^ b
- bit_not(a)    -> ~a
- bit_shl(a, n) -> a << n
- bit_shr(a, n) -> a >> n

#### Examples

```ton
let a: int = 6;  // 110
let b: int = 3;  // 011
print(bit_and(a, b)); // 2
print(bit_or(a, b));  // 7
print(bit_xor(a, b)); // 5
print(bit_shl(a, 1)); // 12
print(bit_shr(a, 1)); // 3
print(bit_not(0));    // -1 (two's complement convention)
print(bit_not(a));    // ~6 == -7 (two's complement convention)
```


## 8. Code Examples

Below are some Ton code examples illustrating various language features and constructs.

### Example 1: Simple "Hello world!" program

```ton
print("Hello world!");
```

### Example 2: Variable declaration and arithmetic operations

Note: currently arithmetic operators (+, -, *, /) and unary minus (-a) work on `int` only. Division by zero is a runtime error.

```ton
let a: int = 10;
let b: int = 20;
let sum: int = a + b;
print("Sum of a and b: " + sum);

// Unary minus example
let c: int = -a; // c == -10

// Division (beware division by zero)
let d: int = a / 2; // OK
// let e: int = a / 0; // will cause Runtime Error: Division by zero

// (Float examples below are conceptual; float support may not be available yet)
const PI: float = 3.14;
let radius: float = 5.0;
let circumference: float = 2 * PI * radius;
print("Circle circumference: " + circumference);
```

### Example 3: `if-else` conditional statement

```ton
let age: int = 18;

if (age >= 18) {
    print("You are an adult.");
} else {
    print("You are a minor.");
}
```

### Example 4: `while` loop

```ton
let counter: int = 0;
while (counter < 5) {
    print("Counter: " + counter);
    counter = counter + 1;
}
```

### Example 5: Functions

```ton
fn multiply(x: int, y: int) -> int {
    return x * y;
}

let multiplication_result: int = multiply(4, 6);
print("Multiplication result: " + multiplication_result);

fn greet_person(name: string) {
    print("Hi, " + name + "!");
}

greet_person("Marcin");
```

### Example 6: User interaction (`read_line`)

```ton
print("Enter your favorite animal:");
let animal: string = read_line();
print("Your favorite animal is: " + animal);
```


## 9. Compilation and Running Programs

Programs written in the Ton language are interpreted. This means they are not compiled into machine code before execution, but rather executed line by line by the Ton interpreter.

### Running the Interpreter

To run a Ton program, you need to invoke the Ton interpreter, providing the path to the source file as an argument.

#### Syntax

```bash
ton <filename>.ton
```

*   `<filename>.ton`: The path to the file containing the Ton language source code.

#### Examples

Suppose you have a `hello.ton` file with the following content:

```ton
print("Hello, Ton!");
```

To run this program, execute in the terminal:

```bash
ton hello.ton
```

Output:

```
Hello, Ton!
```

### Entry Point: main and Script Mode

- Ton executes all top-level statements in the file.
- If a function named `main` is defined, the interpreter will execute it after the top-level statements in a fresh local environment, and the integer it returns becomes the process exit code. If `main` returns a non-integer or has no explicit return, the exit code defaults to 0.
- If no `main` is defined, the interpreter runs in script mode: it executes the top-level statements and exits with code 0.

#### Example with `main`

```ton
print("Top-level runs first.");

fn main() -> int {
    print("Hello from main!");
    return 0; // process exit code
}
```

### Error Handling

The Ton interpreter reports errors encountered during parsing or program execution, indicating the line and column number where the error occurred, along with a brief description of the problem.

#### Example of a Parsing Error

If in the `error.ton` file you have:

```ton
let x: int = ;
```

Running `ton error.ton` might return an error similar to:

```
Parser Error at line 1, column 14: Expected an expression
```

#### Example of a Runtime Error

If in the `division.ton` file you have:

```ton
let a: int = 10;
let b: int = 0;
let result: int = a / b;
print(result);
```

Running `ton division.ton` might return an error similar to:

```
Runtime Error at line 3, column 17: Division by zero
```