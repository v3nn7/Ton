# TonLib API Reference

**Author: v3nn7**

This document provides a comprehensive reference for all TonLib modules, functions, and data structures.

## Table of Contents

1. [Core Module](#core-module)
2. [Math Module](#math-module)
3. [String Module](#string-module)
4. [Collections Module](#collections-module)
5. [IO Module](#io-module)
6. [JSON Module](#json-module)
7. [Network Module](#network-module)
8. [Testing Module](#testing-module)
9. [Memory Module](#memory-module)
10. [System Module](#system-module)

---

## Core Module

The core module provides fundamental utilities and type conversion functions.

### Data Types

#### Error
```ton
struct Error {
    message: string;
    code: int;
}
```

### Functions

#### Type Conversion Functions

##### `string_to_int(value: string): int`
Converts a string to an integer.
- **Parameters**: `value` - String representation of a number
- **Returns**: Integer value
- **Example**: `let num: int = string_to_int("42");`

##### `string_to_float(value: string): float`
Converts a string to a floating-point number.
- **Parameters**: `value` - String representation of a number
- **Returns**: Float value
- **Example**: `let pi: float = string_to_float("3.14159");`

##### `int_to_string(value: int): string`
Converts an integer to a string.
- **Parameters**: `value` - Integer value
- **Returns**: String representation
- **Example**: `let text: string = int_to_string(42);`

##### `float_to_string(value: float): string`
Converts a float to a string.
- **Parameters**: `value` - Float value
- **Returns**: String representation
- **Example**: `let text: string = float_to_string(3.14159);`

##### `bool_to_string(value: bool): string`
Converts a boolean to a string.
- **Parameters**: `value` - Boolean value
- **Returns**: "true" or "false"
- **Example**: `let text: string = bool_to_string(true);`

#### Utility Functions

##### `type_of(value: any): string`
Returns the type name of a value.
- **Parameters**: `value` - Any value
- **Returns**: Type name as string
- **Example**: `let type: string = type_of(42); // "int"`

##### `is_null(value: any): bool`
Checks if a value is null.
- **Parameters**: `value` - Any value
- **Returns**: True if null, false otherwise

##### `is_defined(value: any): bool`
Checks if a value is defined (not null or undefined).
- **Parameters**: `value` - Any value
- **Returns**: True if defined, false otherwise

#### Error Handling

##### `new_error(message: string, code: int): Error`
Creates a new error object.
- **Parameters**: 
  - `message` - Error description
  - `code` - Error code
- **Returns**: Error object
- **Example**: `let err: Error = new_error("File not found", 404);`

---

## Math Module

The math module provides mathematical constants, basic operations, and advanced mathematical functions.

### Constants

```ton
const PI: float = 3.14159265358979323846;
const E: float = 2.71828182845904523536;
const SQRT_2: float = 1.41421356237309504880;
const SQRT_3: float = 1.73205080756887729352;
const LN_2: float = 0.69314718055994530942;
const LN_10: float = 2.30258509299404568402;
```

### Functions

#### Basic Operations

##### `abs(value: float): float`
Returns the absolute value of a number.
- **Parameters**: `value` - Input number
- **Returns**: Absolute value
- **Example**: `let result: float = abs(-3.14); // 3.14`

##### `abs_int(value: int): int`
Returns the absolute value of an integer.
- **Parameters**: `value` - Input integer
- **Returns**: Absolute value
- **Example**: `let result: int = abs_int(-42); // 42`

##### `min(a: float, b: float): float`
Returns the smaller of two numbers.
- **Parameters**: `a`, `b` - Numbers to compare
- **Returns**: Minimum value
- **Example**: `let result: float = min(3.14, 2.71); // 2.71`

##### `max(a: float, b: float): float`
Returns the larger of two numbers.
- **Parameters**: `a`, `b` - Numbers to compare
- **Returns**: Maximum value
- **Example**: `let result: float = max(3.14, 2.71); // 3.14`

##### `pow(base: float, exponent: float): float`
Raises a number to a power.
- **Parameters**: 
  - `base` - Base number
  - `exponent` - Power to raise to
- **Returns**: Result of base^exponent
- **Example**: `let result: float = pow(2.0, 3.0); // 8.0`

##### `sqrt(value: float): float`
Returns the square root of a number.
- **Parameters**: `value` - Input number (must be non-negative)
- **Returns**: Square root
- **Example**: `let result: float = sqrt(16.0); // 4.0`

#### Trigonometric Functions

##### `sin(angle: float): float`
Returns the sine of an angle (in radians).
- **Parameters**: `angle` - Angle in radians
- **Returns**: Sine value
- **Example**: `let result: float = sin(PI / 2); // 1.0`

##### `cos(angle: float): float`
Returns the cosine of an angle (in radians).
- **Parameters**: `angle` - Angle in radians
- **Returns**: Cosine value
- **Example**: `let result: float = cos(0.0); // 1.0`

##### `tan(angle: float): float`
Returns the tangent of an angle (in radians).
- **Parameters**: `angle` - Angle in radians
- **Returns**: Tangent value
- **Example**: `let result: float = tan(PI / 4); // 1.0`

##### `asin(value: float): float`
Returns the arcsine of a value.
- **Parameters**: `value` - Input value (-1 to 1)
- **Returns**: Angle in radians
- **Example**: `let angle: float = asin(1.0); // PI/2`

##### `acos(value: float): float`
Returns the arccosine of a value.
- **Parameters**: `value` - Input value (-1 to 1)
- **Returns**: Angle in radians
- **Example**: `let angle: float = acos(0.0); // PI/2`

##### `atan(value: float): float`
Returns the arctangent of a value.
- **Parameters**: `value` - Input value
- **Returns**: Angle in radians
- **Example**: `let angle: float = atan(1.0); // PI/4`

#### Logarithmic Functions

##### `ln(value: float): float`
Returns the natural logarithm of a number.
- **Parameters**: `value` - Input number (must be positive)
- **Returns**: Natural logarithm
- **Example**: `let result: float = ln(E); // 1.0`

##### `log10(value: float): float`
Returns the base-10 logarithm of a number.
- **Parameters**: `value` - Input number (must be positive)
- **Returns**: Base-10 logarithm
- **Example**: `let result: float = log10(100.0); // 2.0`

##### `log2(value: float): float`
Returns the base-2 logarithm of a number.
- **Parameters**: `value` - Input number (must be positive)
- **Returns**: Base-2 logarithm
- **Example**: `let result: float = log2(8.0); // 3.0`

#### Rounding Functions

##### `ceil(value: float): float`
Returns the smallest integer greater than or equal to the value.
- **Parameters**: `value` - Input number
- **Returns**: Ceiling value
- **Example**: `let result: float = ceil(3.14); // 4.0`

##### `floor(value: float): float`
Returns the largest integer less than or equal to the value.
- **Parameters**: `value` - Input number
- **Returns**: Floor value
- **Example**: `let result: float = floor(3.14); // 3.0`

##### `round(value: float): float`
Returns the nearest integer to the value.
- **Parameters**: `value` - Input number
- **Returns**: Rounded value
- **Example**: `let result: float = round(3.14); // 3.0`

#### Statistical Functions

##### `average(values: float[]): float`
Calculates the arithmetic mean of an array of numbers.
- **Parameters**: `values` - Array of numbers
- **Returns**: Average value
- **Example**: `let avg: float = average([1.0, 2.0, 3.0]); // 2.0`

##### `sum(values: float[]): float`
Calculates the sum of an array of numbers.
- **Parameters**: `values` - Array of numbers
- **Returns**: Sum of all values
- **Example**: `let total: float = sum([1.0, 2.0, 3.0]); // 6.0`

##### `min_array(values: float[]): float`
Finds the minimum value in an array.
- **Parameters**: `values` - Array of numbers
- **Returns**: Minimum value
- **Example**: `let min_val: float = min_array([3.0, 1.0, 2.0]); // 1.0`

##### `max_array(values: float[]): float`
Finds the maximum value in an array.
- **Parameters**: `values` - Array of numbers
- **Returns**: Maximum value
- **Example**: `let max_val: float = max_array([3.0, 1.0, 2.0]); // 3.0`

#### Random Number Generation

##### `random(): float`
Generates a random floating-point number between 0.0 and 1.0.
- **Returns**: Random float [0.0, 1.0)
- **Example**: `let rand: float = random();`

##### `random_int(min: int, max: int): int`
Generates a random integer within a specified range.
- **Parameters**: 
  - `min` - Minimum value (inclusive)
  - `max` - Maximum value (inclusive)
- **Returns**: Random integer
- **Example**: `let dice: int = random_int(1, 6);`

---

## String Module

The string module provides comprehensive string manipulation and processing functions.

### Functions

#### Basic Operations

##### `length(str: string): int`
Returns the length of a string.
- **Parameters**: `str` - Input string
- **Returns**: String length
- **Example**: `let len: int = length("hello"); // 5`

##### `concat(str1: string, str2: string): string`
Concatenates two strings.
- **Parameters**: `str1`, `str2` - Strings to concatenate
- **Returns**: Combined string
- **Example**: `let result: string = concat("hello", " world");`

##### `repeat(str: string, count: int): string`
Repeats a string a specified number of times.
- **Parameters**: 
  - `str` - String to repeat
  - `count` - Number of repetitions
- **Returns**: Repeated string
- **Example**: `let result: string = repeat("ha", 3); // "hahaha"`

##### `is_empty(str: string): bool`
Checks if a string is empty.
- **Parameters**: `str` - String to check
- **Returns**: True if empty, false otherwise
- **Example**: `let empty: bool = is_empty(""); // true`

#### Searching and Indexing

##### `index_of(str: string, substring: string): int`
Finds the first occurrence of a substring.
- **Parameters**: 
  - `str` - String to search in
  - `substring` - String to find
- **Returns**: Index of first occurrence, -1 if not found
- **Example**: `let pos: int = index_of("hello world", "world"); // 6`

##### `substring(str: string, start: int, end?: int): string`
Extracts a portion of a string between specified indices.
- **Parameters**: 
  - `str` - Input string
  - `start` - Starting index (inclusive)
  - `end` - Ending index (exclusive, optional - defaults to string length)
- **Returns**: Extracted substring
- **Example**: 
  - `let sub: string = substring("hello world", 6); // "world"`
  - `let sub2: string = substring("hello world", 0, 5); // "hello"`

##### `contains(str: string, substring: string): bool`
Checks if a string contains a substring.
- **Parameters**: 
  - `str` - String to search in
  - `substring` - String to find
- **Returns**: True if found, false otherwise
- **Example**: `let found: bool = contains("hello", "ell"); // true`

##### `starts_with(str: string, prefix: string): bool`
Checks if a string starts with a prefix.
- **Parameters**: 
  - `str` - String to check
  - `prefix` - Prefix to look for
- **Returns**: True if starts with prefix
- **Example**: `let starts: bool = starts_with("hello", "he"); // true`

##### `ends_with(str: string, suffix: string): bool`
Checks if a string ends with a suffix.
- **Parameters**: 
  - `str` - String to check
  - `suffix` - Suffix to look for
- **Returns**: True if ends with suffix
- **Example**: `let ends: bool = ends_with("hello", "lo"); // true`

##### `char_at(str: string, index: int): string`
Returns the character at a specific index.
- **Parameters**: 
  - `str` - Input string
  - `index` - Character index
- **Returns**: Character at index
- **Example**: `let char: string = char_at("hello", 1); // "e"`

#### String Manipulation

##### `to_upper(str: string): string`
Converts a string to uppercase.
- **Parameters**: `str` - Input string
- **Returns**: Uppercase string
- **Example**: `let upper: string = to_upper("hello"); // "HELLO"`

##### `to_lower(str: string): string`
Converts a string to lowercase.
- **Parameters**: `str` - Input string
- **Returns**: Lowercase string
- **Example**: `let lower: string = to_lower("HELLO"); // "hello"`

##### `trim(str: string): string`
Removes whitespace from both ends of a string.
- **Parameters**: `str` - Input string
- **Returns**: Trimmed string
- **Example**: `let trimmed: string = trim("  hello  "); // "hello"`

##### `trim_start(str: string): string`
Removes whitespace from the beginning of a string.
- **Parameters**: `str` - Input string
- **Returns**: Left-trimmed string
- **Example**: `let trimmed: string = trim_start("  hello"); // "hello"`

##### `trim_end(str: string): string`
Removes whitespace from the end of a string.
- **Parameters**: `str` - Input string
- **Returns**: Right-trimmed string
- **Example**: `let trimmed: string = trim_end("hello  "); // "hello"`

#### Splitting and Joining

##### `split(str: string, delimiter: string): string[]`
Splits a string into an array using a delimiter.
- **Parameters**: 
  - `str` - String to split
  - `delimiter` - Delimiter string
- **Returns**: Array of string parts
- **Example**: `let parts: string[] = split("a,b,c", ","); // ["a", "b", "c"]`

##### `join(strings: string[], delimiter: string): string`
Joins an array of strings with a delimiter.
- **Parameters**: 
  - `strings` - Array of strings
  - `delimiter` - Delimiter string
- **Returns**: Joined string
- **Example**: `let result: string = join(["a", "b", "c"], ","); // "a,b,c"`

#### String Replacement

##### `replace_first(str: string, search: string, replacement: string): string`
Replaces the first occurrence of a substring.
- **Parameters**: 
  - `str` - Input string
  - `search` - String to find
  - `replacement` - Replacement string
- **Returns**: String with first occurrence replaced
- **Example**: `let result: string = replace_first("hello hello", "hello", "hi"); // "hi hello"`

##### `replace(str: string, search: string, replacement: string): string`
Replaces the first occurrence of a substring (alias for replace_first).
- **Parameters**: 
  - `str` - Input string
  - `search` - String to find
  - `replacement` - Replacement string
- **Returns**: String with first occurrence replaced
- **Example**: `let result: string = replace("hello world", "world", "universe"); // "hello universe"`

##### `replace_all(str: string, search: string, replacement: string): string`
Replaces all occurrences of a substring.
- **Parameters**: 
  - `str` - Input string
  - `search` - String to find
  - `replacement` - Replacement string
- **Returns**: String with all occurrences replaced
- **Example**: `let result: string = replace_all("hello hello", "hello", "hi"); // "hi hi"`

#### String Formatting

##### `format(template: string, args: string[]): string`
Formats a string template with arguments.
- **Parameters**: 
  - `template` - Template string with placeholders
  - `args` - Array of replacement values
- **Returns**: Formatted string
- **Example**: `let result: string = format("Hello {0}, you are {1}", ["John", "awesome"]);`

##### `format_number(value: float, decimals: int): string`
Formats a number with specified decimal places.
- **Parameters**: 
  - `value` - Number to format
  - `decimals` - Number of decimal places
- **Returns**: Formatted number string
- **Example**: `let formatted: string = format_number(3.14159, 2); // "3.14"`

#### String Validation

##### `is_numeric(str: string): bool`
Checks if a string represents a valid number.
- **Parameters**: `str` - String to validate
- **Returns**: True if numeric, false otherwise
- **Example**: `let valid: bool = is_numeric("123.45"); // true`

##### `is_alpha(str: string): bool`
Checks if a string contains only alphabetic characters.
- **Parameters**: `str` - String to validate
- **Returns**: True if alphabetic, false otherwise
- **Example**: `let valid: bool = is_alpha("hello"); // true`

##### `is_alphanumeric(str: string): bool`
Checks if a string contains only alphanumeric characters.
- **Parameters**: `str` - String to validate
- **Returns**: True if alphanumeric, false otherwise
- **Example**: `let valid: bool = is_alphanumeric("hello123"); // true`

##### `is_email(str: string): bool`
Validates if a string is a valid email address.
- **Parameters**: `str` - String to validate
- **Returns**: True if valid email, false otherwise
- **Example**: `let valid: bool = is_email("user@example.com"); // true`

#### Encoding and Decoding

##### `to_base64(str: string): string`
Encodes a string to Base64.
- **Parameters**: `str` - String to encode
- **Returns**: Base64 encoded string
- **Example**: `let encoded: string = to_base64("hello");`

##### `from_base64(str: string): string`
Decodes a Base64 string.
- **Parameters**: `str` - Base64 string to decode
- **Returns**: Decoded string
- **Example**: `let decoded: string = from_base64(encoded);`

##### `url_encode(str: string): string`
URL-encodes a string.
- **Parameters**: `str` - String to encode
- **Returns**: URL-encoded string
- **Example**: `let encoded: string = url_encode("hello world");`

##### `url_decode(str: string): string`
URL-decodes a string.
- **Parameters**: `str` - URL-encoded string
- **Returns**: Decoded string
- **Example**: `let decoded: string = url_decode("hello%20world");`

#### String Comparison

##### `compare(str1: string, str2: string): int`
Compares two strings lexicographically.
- **Parameters**: `str1`, `str2` - Strings to compare
- **Returns**: -1 if str1 < str2, 0 if equal, 1 if str1 > str2
- **Example**: `let result: int = compare("apple", "banana"); // -1`

##### `compare_ignore_case(str1: string, str2: string): int`
Compares two strings ignoring case.
- **Parameters**: `str1`, `str2` - Strings to compare
- **Returns**: -1 if str1 < str2, 0 if equal, 1 if str1 > str2
- **Example**: `let result: int = compare_ignore_case("Apple", "apple"); // 0`

##### `equals_ignore_case(str1: string, str2: string): bool`
Checks if two strings are equal ignoring case.
- **Parameters**: `str1`, `str2` - Strings to compare
- **Returns**: True if equal (case-insensitive)
- **Example**: `let equal: bool = equals_ignore_case("Hello", "HELLO"); // true`

---

## Collections Module

The collections module provides data structures for organizing and manipulating collections of data.

### Data Structures

#### List<T>
Dynamic array that can grow and shrink.

##### `tonlist_create(): TonList*`
Creates a new empty list.
- **Returns**: New list instance
- **Example**: `let list: TonList* = tonlist_create();`

##### `tonlist_push(list: TonList*, item: Value): int`
Adds an item to the end of the list.
- **Parameters**: 
  - `list` - Target list
  - `item` - Item to add
- **Returns**: 1 on success, 0 on failure
- **Example**: `tonlist_push(list, create_value_int(42));`

##### `tonlist_get(list: TonList*, index: int): Value`
Gets an item at a specific index.
- **Parameters**: 
  - `list` - Source list
  - `index` - Item index
- **Returns**: Item at index, or null value if invalid index
- **Example**: `let item: Value = tonlist_get(list, 0);`

##### `tonlist_set(list: TonList*, index: int, item: Value): int`
Sets an item at a specific index.
- **Parameters**: 
  - `list` - Target list
  - `index` - Item index
  - `item` - New item value
- **Returns**: 1 on success, 0 on failure
- **Example**: `tonlist_set(list, 0, create_value_string("new value"));`

##### `tonlist_pop(list: TonList*): Value`
Removes and returns the last item from the list.
- **Parameters**: `list` - Target list
- **Returns**: Removed item, or null value if empty
- **Example**: `let removed: Value = tonlist_pop(list);`

##### `tonlist_size(list: TonList*): int`
Returns the number of items in the list.
- **Parameters**: `list` - Source list
- **Returns**: List size
- **Example**: `let size: int = tonlist_size(list);`

##### `tonlist_destroy(list: TonList*): void`
Destroys the list and frees memory.
- **Parameters**: `list` - List to destroy
- **Example**: `tonlist_destroy(list);`

#### Map<K, V>
Key-value pair collection with unique keys.

##### `tonmap_create(): TonMap*`
Creates a new empty map.
- **Returns**: New map instance
- **Example**: `let map: TonMap* = tonmap_create();`

##### `tonmap_set(map: TonMap*, key: string, value: Value): int`
Adds or updates a key-value pair.
- **Parameters**: 
  - `map` - Target map
  - `key` - Key (string)
  - `value` - Value
- **Returns**: 1 on success, 0 on failure
- **Example**: `tonmap_set(map, "count", create_value_int(42));`

##### `tonmap_get(map: TonMap*, key: string): Value`
Gets the value associated with a key.
- **Parameters**: 
  - `map` - Source map
  - `key` - Key to look up
- **Returns**: Associated value, or null value if key not found
- **Example**: `let count: Value = tonmap_get(map, "count");`

##### `tonmap_has(map: TonMap*, key: string): int`
Checks if the map contains a specific key.
- **Parameters**: 
  - `map` - Source map
  - `key` - Key to find
- **Returns**: 1 if key exists, 0 otherwise
- **Example**: `let exists: int = tonmap_has(map, "count");`

##### `tonmap_remove(map: TonMap*, key: string): int`
Removes a key-value pair from the map.
- **Parameters**: 
  - `map` - Target map
  - `key` - Key to remove
- **Returns**: 1 if removed, 0 if key not found
- **Example**: `let removed: int = tonmap_remove(map, "count");`

##### `tonmap_size(map: TonMap*): int`
Returns the number of key-value pairs.
- **Parameters**: `map` - Source map
- **Returns**: Map size
- **Example**: `let size: int = tonmap_size(map);`

##### `tonmap_destroy(map: TonMap*): void`
Destroys the map and frees memory.
- **Parameters**: `map` - Map to destroy
- **Example**: `tonmap_destroy(map);`

##### `Map_is_empty<K, V>(map: Map<K, V>): bool`
Checks if the map is empty.
- **Parameters**: `map` - Source map
- **Returns**: True if empty, false otherwise
- **Example**: `let empty: bool = Map_is_empty(map);`

##### `Map_keys<K, V>(map: Map<K, V>): K[]`
Returns an array of all keys.
- **Parameters**: `map` - Source map
- **Returns**: Array of keys
- **Example**: `let keys: string[] = Map_keys(map);`

##### `Map_values<K, V>(map: Map<K, V>): V[]`
Returns an array of all values.
- **Parameters**: `map` - Source map
- **Returns**: Array of values
- **Example**: `let values: int[] = Map_values(map);`

#### Set<T>
Collection of unique items.

##### `tonset_create(): TonSet*`
Creates a new empty set.
- **Returns**: New set instance
- **Example**: `let set: TonSet* = tonset_create();`

##### `tonset_add(set: TonSet*, item: string): int`
Adds an item to the set.
- **Parameters**: 
  - `set` - Target set
  - `item` - Item to add (string)
- **Returns**: 1 if added, 0 if already exists or on failure
- **Example**: `let added: int = tonset_add(set, "hello");`

##### `tonset_has(set: TonSet*, item: string): int`
Checks if the set contains an item.
- **Parameters**: 
  - `set` - Source set
  - `item` - Item to find
- **Returns**: 1 if found, 0 otherwise
- **Example**: `let found: int = tonset_has(set, "hello");`

##### `tonset_remove(set: TonSet*, item: string): int`
Removes an item from the set.
- **Parameters**: 
  - `set` - Target set
  - `item` - Item to remove
- **Returns**: 1 if removed, 0 if not found
- **Example**: `let removed: int = tonset_remove(set, "hello");`

##### `tonset_size(set: TonSet*): int`
Returns the number of items in the set.
- **Parameters**: `set` - Source set
- **Returns**: Set size
- **Example**: `let size: int = tonset_size(set);`

##### `tonset_destroy(set: TonSet*): void`
Destroys the set and frees memory.
- **Parameters**: `set` - Set to destroy
- **Example**: `tonset_destroy(set);`

#### Queue<T>
First-in-first-out (FIFO) collection.

##### `Queue_new<T>(): Queue<T>`
Creates a new empty queue.
- **Returns**: New queue instance
- **Example**: `let queue: Queue<string> = Queue_new();`

##### `Queue_enqueue<T>(queue: Queue<T>, item: T): void`
Adds an item to the back of the queue.
- **Parameters**: 
  - `queue` - Target queue
  - `item` - Item to add
- **Example**: `Queue_enqueue(queue, "hello");`

##### `Queue_dequeue<T>(queue: Queue<T>): T`
Removes and returns the front item.
- **Parameters**: `queue` - Target queue
- **Returns**: Front item
- **Example**: `let item: string = Queue_dequeue(queue);`

##### `Queue_peek<T>(queue: Queue<T>): T`
Returns the front item without removing it.
- **Parameters**: `queue` - Source queue
- **Returns**: Front item
- **Example**: `let front: string = Queue_peek(queue);`

##### `Queue_is_empty<T>(queue: Queue<T>): bool`
Checks if the queue is empty.
- **Parameters**: `queue` - Source queue
- **Returns**: True if empty, false otherwise
- **Example**: `let empty: bool = Queue_is_empty(queue);`

#### Stack<T>
Last-in-first-out (LIFO) collection.

##### `Stack_new<T>(): Stack<T>`
Creates a new empty stack.
- **Returns**: New stack instance
- **Example**: `let stack: Stack<string> = Stack_new();`

##### `Stack_push<T>(stack: Stack<T>, item: T): void`
Pushes an item onto the top of the stack.
- **Parameters**: 
  - `stack` - Target stack
  - `item` - Item to push
- **Example**: `Stack_push(stack, "hello");`

##### `Stack_pop<T>(stack: Stack<T>): T`
Pops and returns the top item.
- **Parameters**: `stack` - Target stack
- **Returns**: Top item
- **Example**: `let item: string = Stack_pop(stack);`

##### `Stack_peek<T>(stack: Stack<T>): T`
Returns the top item without removing it.
- **Parameters**: `stack` - Source stack
- **Returns**: Top item
- **Example**: `let top: string = Stack_peek(stack);`

##### `Stack_is_empty<T>(stack: Stack<T>): bool`
Checks if the stack is empty.
- **Parameters**: `stack` - Source stack
- **Returns**: True if empty, false otherwise
- **Example**: `let empty: bool = Stack_is_empty(stack);`

##### `Stack_size<T>(stack: Stack<T>): int`
Returns the number of items in the stack.
- **Parameters**: `stack` - Source stack
- **Returns**: Stack size
- **Example**: `let size: int = Stack_size(stack);`

---

## IO Module

The IO module provides file system operations, directory management, and input/output functionality.

### Constants

```ton
const FILE_MODE_READ: string = "r";
const FILE_MODE_WRITE: string = "w";
const FILE_MODE_APPEND: string = "a";
const FILE_MODE_READ_WRITE: string = "r+";
const FILE_MODE_WRITE_READ: string = "w+";
const FILE_MODE_APPEND_READ: string = "a+";
```

### Data Structures

#### File
```ton
struct File {
    handle: int;
    path: string;
    mode: string;
    is_open: bool;
}
```

#### DirEntry
```ton
struct DirEntry {
    name: string;
    path: string;
    is_directory: bool;
    size: int;
    modified_time: int;
}
```

### Functions

#### File Operations

##### `file_open(path: string, mode: string): File`
Opens a file for reading or writing.
- **Parameters**: 
  - `path` - File path
  - `mode` - File mode (see constants)
- **Returns**: File handle
- **Example**: `let file: File = file_open("data.txt", FILE_MODE_READ);`

##### `file_close(file: File): bool`
Closes an open file.
- **Parameters**: `file` - File to close
- **Returns**: True if successful
- **Example**: `let success: bool = file_close(file);`

##### `file_read(file: File, buffer: string, size: int): int`
Reads data from a file into a buffer.
- **Parameters**: 
  - `file` - File to read from
  - `buffer` - Buffer to read into
  - `size` - Maximum bytes to read
- **Returns**: Number of bytes read
- **Example**: `let bytes_read: int = file_read(file, buffer, 1024);`

##### `file_read_all(path: string): string`
Reads the entire contents of a file.
- **Parameters**: `path` - File path
- **Returns**: File contents as string
- **Example**: `let content: string = file_read_all("data.txt");`

##### `file_write(file: File, data: string): int`
Writes data to a file.
- **Parameters**: 
  - `file` - File to write to
  - `data` - Data to write
- **Returns**: Number of bytes written
- **Example**: `let bytes_written: int = file_write(file, "Hello World");`

##### `file_write_all(path: string, data: string): bool`
Writes data to a file, creating it if necessary.
- **Parameters**: 
  - `path` - File path
  - `data` - Data to write
- **Returns**: True if successful
- **Example**: `let success: bool = file_write_all("output.txt", "Hello World");`

##### `file_append(path: string, data: string): bool`
Appends data to a file.
- **Parameters**: 
  - `path` - File path
  - `data` - Data to append
- **Returns**: True if successful
- **Example**: `let success: bool = file_append("log.txt", "New entry\n");`

##### `file_delete(path: string): bool`
Deletes a file.
- **Parameters**: `path` - File path
- **Returns**: True if successful
- **Example**: `let deleted: bool = file_delete("temp.txt");`

##### `file_copy(source: string, destination: string): bool`
Copies a file to a new location.
- **Parameters**: 
  - `source` - Source file path
  - `destination` - Destination file path
- **Returns**: True if successful
- **Example**: `let copied: bool = file_copy("source.txt", "backup.txt");`

##### `file_move(source: string, destination: string): bool`
Moves/renames a file.
- **Parameters**: 
  - `source` - Source file path
  - `destination` - Destination file path
- **Returns**: True if successful
- **Example**: `let moved: bool = file_move("old.txt", "new.txt");`

##### `file_exists(path: string): bool`
Checks if a file exists.
- **Parameters**: `path` - File path
- **Returns**: True if file exists
- **Example**: `let exists: bool = file_exists("data.txt");`

##### `file_size(path: string): int`
Gets the size of a file in bytes.
- **Parameters**: `path` - File path
- **Returns**: File size in bytes
- **Example**: `let size: int = file_size("data.txt");`

#### Directory Operations

##### `dir_create(path: string): bool`
Creates a directory.
- **Parameters**: `path` - Directory path
- **Returns**: True if successful
- **Example**: `let created: bool = dir_create("new_folder");`

##### `dir_create_recursive(path: string): bool`
Creates a directory and all parent directories.
- **Parameters**: `path` - Directory path
- **Returns**: True if successful
- **Example**: `let created: bool = dir_create_recursive("path/to/folder");`

##### `dir_delete(path: string): bool`
Deletes an empty directory.
- **Parameters**: `path` - Directory path
- **Returns**: True if successful
- **Example**: `let deleted: bool = dir_delete("empty_folder");`

##### `dir_delete_recursive(path: string): bool`
Deletes a directory and all its contents.
- **Parameters**: `path` - Directory path
- **Returns**: True if successful
- **Example**: `let deleted: bool = dir_delete_recursive("folder_with_files");`

##### `dir_exists(path: string): bool`
Checks if a directory exists.
- **Parameters**: `path` - Directory path
- **Returns**: True if directory exists
- **Example**: `let exists: bool = dir_exists("my_folder");`

##### `dir_list(path: string): DirEntry[]`
Lists the contents of a directory.
- **Parameters**: `path` - Directory path
- **Returns**: Array of directory entries
- **Example**: `let entries: DirEntry[] = dir_list(".");`

#### Path Operations

##### `path_join(parts: string[]): string`
Joins path components using the system path separator.
- **Parameters**: `parts` - Array of path components
- **Returns**: Joined path
- **Example**: `let path: string = path_join(["home", "user", "documents"]);`

##### `path_absolute(path: string): string`
Converts a relative path to an absolute path.
- **Parameters**: `path` - Relative path
- **Returns**: Absolute path
- **Example**: `let abs_path: string = path_absolute("../data");`

##### `path_normalize(path: string): string`
Normalizes a path by resolving . and .. components.
- **Parameters**: `path` - Path to normalize
- **Returns**: Normalized path
- **Example**: `let norm_path: string = path_normalize("./folder/../file.txt");`

##### `path_get_directory(path: string): string`
Gets the directory portion of a path.
- **Parameters**: `path` - File path
- **Returns**: Directory path
- **Example**: `let dir: string = path_get_directory("/home/user/file.txt"); // "/home/user"`

##### `path_get_filename(path: string): string`
Gets the filename portion of a path.
- **Parameters**: `path` - File path
- **Returns**: Filename
- **Example**: `let name: string = path_get_filename("/home/user/file.txt"); // "file.txt"`

##### `path_get_extension(path: string): string`
Gets the file extension from a path.
- **Parameters**: `path` - File path
- **Returns**: File extension (including dot)
- **Example**: `let ext: string = path_get_extension("file.txt"); // ".txt"`

#### Stream Operations

##### `stdin_read_line(): string`
Reads a line from standard input.
- **Returns**: Input line as string
- **Example**: `let input: string = stdin_read_line();`

##### `stdout_write(text: string): void`
Writes text to standard output.
- **Parameters**: `text` - Text to write
- **Example**: `stdout_write("Hello World\n");`

##### `stderr_write(text: string): void`
Writes text to standard error.
- **Parameters**: `text` - Text to write
- **Example**: `stderr_write("Error occurred\n");`

#### Utility Functions

##### `get_temp_directory(): string`
Gets the system temporary directory path.
- **Returns**: Temporary directory path
- **Example**: `let temp_dir: string = get_temp_directory();`

##### `file_get_permissions(path: string): int`
Gets file permissions as a numeric value.
- **Parameters**: `path` - File path
- **Returns**: Permission bits
- **Example**: `let perms: int = file_get_permissions("file.txt");`

##### `file_set_permissions(path: string, permissions: int): bool`
Sets file permissions.
- **Parameters**: 
  - `path` - File path
  - `permissions` - Permission bits
- **Returns**: True if successful
- **Example**: `let success: bool = file_set_permissions("file.txt", 644);`

##### `file_get_modified_time(path: string): int`
Gets the last modification time of a file.
- **Parameters**: `path` - File path
- **Returns**: Modification time as timestamp
- **Example**: `let mod_time: int = file_get_modified_time("file.txt");`

---

## JSON Module

The JSON module provides comprehensive JSON parsing, serialization, and manipulation capabilities.

### Constants

```ton
const JSON_TYPE_NULL: int = 0;
const JSON_TYPE_BOOL: int = 1;
const JSON_TYPE_NUMBER: int = 2;
const JSON_TYPE_STRING: int = 3;
const JSON_TYPE_ARRAY: int = 4;
const JSON_TYPE_OBJECT: int = 5;
```

### Data Structures

#### JsonValue
```ton
struct JsonValue {
    type: int;
    data: any;
}
```

#### JsonResult
```ton
struct JsonResult {
    success: bool;
    value: JsonValue;
    error_message: string;
    error_line: int;
    error_column: int;
}
```

#### JsonFormatOptions
```ton
struct JsonFormatOptions {
    indent: string;
    compact: bool;
    sort_keys: bool;
    escape_unicode: bool;
}
```

### Functions

#### Parsing Functions

##### `json_parse(text: string): JsonResult`
Parses a JSON string into a JsonValue.
- **Parameters**: `text` - JSON string to parse
- **Returns**: Parse result with value or error
- **Example**: `let result: JsonResult = json_parse('{"name": "John"}');`

##### `json_parse_file(path: string): JsonResult`
Parses JSON from a file.
- **Parameters**: `path` - File path containing JSON
- **Returns**: Parse result with value or error
- **Example**: `let result: JsonResult = json_parse_file("data.json");`

##### `json_validate(text: string): bool`
Validates if a string is valid JSON.
- **Parameters**: `text` - JSON string to validate
- **Returns**: True if valid JSON
- **Example**: `let valid: bool = json_validate('{"valid": true}');`

#### Serialization Functions

##### `json_stringify(value: JsonValue): string`
Converts a JsonValue to a JSON string.
- **Parameters**: `value` - JsonValue to serialize
- **Returns**: JSON string
- **Example**: `let json_str: string = json_stringify(json_obj);`

##### `json_stringify_pretty(value: JsonValue): string`
Converts a JsonValue to a formatted JSON string.
- **Parameters**: `value` - JsonValue to serialize
- **Returns**: Pretty-formatted JSON string
- **Example**: `let pretty_json: string = json_stringify_pretty(json_obj);`

##### `json_write_file(path: string, value: JsonValue): bool`
Writes a JsonValue to a file as JSON.
- **Parameters**: 
  - `path` - Output file path
  - `value` - JsonValue to write
- **Returns**: True if successful
- **Example**: `let success: bool = json_write_file("output.json", json_obj);`

##### `json_write_file_pretty(path: string, value: JsonValue): bool`
Writes a JsonValue to a file as formatted JSON.
- **Parameters**: 
  - `path` - Output file path
  - `value` - JsonValue to write
- **Returns**: True if successful
- **Example**: `let success: bool = json_write_file_pretty("output.json", json_obj);`

#### Value Creation Functions

##### `json_null(): JsonValue`
Creates a JSON null value.
- **Returns**: JsonValue representing null
- **Example**: `let null_val: JsonValue = json_null();`

##### `json_bool(value: bool): JsonValue`
Creates a JSON boolean value.
- **Parameters**: `value` - Boolean value
- **Returns**: JsonValue representing the boolean
- **Example**: `let bool_val: JsonValue = json_bool(true);`

##### `json_number(value: float): JsonValue`
Creates a JSON number value.
- **Parameters**: `value` - Numeric value
- **Returns**: JsonValue representing the number
- **Example**: `let num_val: JsonValue = json_number(42.5);`

##### `json_string(value: string): JsonValue`
Creates a JSON string value.
- **Parameters**: `value` - String value
- **Returns**: JsonValue representing the string
- **Example**: `let str_val: JsonValue = json_string("hello");`

##### `json_array(): JsonValue`
Creates an empty JSON array.
- **Returns**: JsonValue representing an empty array
- **Example**: `let arr_val: JsonValue = json_array();`

##### `json_object(): JsonValue`
Creates an empty JSON object.
- **Returns**: JsonValue representing an empty object
- **Example**: `let obj_val: JsonValue = json_object();`

#### Value Access Functions

##### `json_get_type(value: JsonValue): int`
Gets the type of a JSON value.
- **Parameters**: `value` - JsonValue to check
- **Returns**: Type constant (JSON_TYPE_*)
- **Example**: `let type: int = json_get_type(value);`

##### `json_is_null(value: JsonValue): bool`
Checks if a value is null.
- **Parameters**: `value` - JsonValue to check
- **Returns**: True if null
- **Example**: `let is_null: bool = json_is_null(value);`

##### `json_is_bool(value: JsonValue): bool`
Checks if a value is a boolean.
- **Parameters**: `value` - JsonValue to check
- **Returns**: True if boolean
- **Example**: `let is_bool: bool = json_is_bool(value);`

##### `json_is_number(value: JsonValue): bool`
Checks if a value is a number.
- **Parameters**: `value` - JsonValue to check
- **Returns**: True if number
- **Example**: `let is_num: bool = json_is_number(value);`

##### `json_is_string(value: JsonValue): bool`
Checks if a value is a string.
- **Parameters**: `value` - JsonValue to check
- **Returns**: True if string
- **Example**: `let is_str: bool = json_is_string(value);`

##### `json_is_array(value: JsonValue): bool`
Checks if a value is an array.
- **Parameters**: `value` - JsonValue to check
- **Returns**: True if array
- **Example**: `let is_arr: bool = json_is_array(value);`

##### `json_is_object(value: JsonValue): bool`
Checks if a value is an object.
- **Parameters**: `value` - JsonValue to check
- **Returns**: True if object
- **Example**: `let is_obj: bool = json_is_object(value);`

##### `json_get_bool(value: JsonValue, default_value: bool): bool`
Gets a boolean value with a default.
- **Parameters**: 
  - `value` - JsonValue to extract from
  - `default_value` - Default if not boolean
- **Returns**: Boolean value or default
- **Example**: `let bool_val: bool = json_get_bool(value, false);`

##### `json_get_number(value: JsonValue, default_value: float): float`
Gets a numeric value with a default.
- **Parameters**: 
  - `value` - JsonValue to extract from
  - `default_value` - Default if not number
- **Returns**: Numeric value or default
- **Example**: `let num_val: float = json_get_number(value, 0.0);`

##### `json_get_string(value: JsonValue, default_value: string): string`
Gets a string value with a default.
- **Parameters**: 
  - `value` - JsonValue to extract from
  - `default_value` - Default if not string
- **Returns**: String value or default
- **Example**: `let str_val: string = json_get_string(value, "");`

#### Array Operations

##### `json_array_length(array: JsonValue): int`
Gets the length of a JSON array.
- **Parameters**: `array` - JsonValue array
- **Returns**: Array length
- **Example**: `let length: int = json_array_length(arr);`

##### `json_array_get(array: JsonValue, index: int): JsonValue`
Gets an element from a JSON array.
- **Parameters**: 
  - `array` - JsonValue array
  - `index` - Element index
- **Returns**: JsonValue at index
- **Example**: `let element: JsonValue = json_array_get(arr, 0);`

##### `json_array_add(array: JsonValue, value: JsonValue): void`
Adds an element to a JSON array.
- **Parameters**: 
  - `array` - JsonValue array
  - `value` - JsonValue to add
- **Example**: `json_array_add(arr, json_string("hello"));`

##### `json_array_set(array: JsonValue, index: int, value: JsonValue): void`
Sets an element in a JSON array.
- **Parameters**: 
  - `array` - JsonValue array
  - `index` - Element index
  - `value` - New JsonValue
- **Example**: `json_array_set(arr, 0, json_number(42));`

##### `json_array_remove(array: JsonValue, index: int): JsonValue`
Removes an element from a JSON array.
- **Parameters**: 
  - `array` - JsonValue array
  - `index` - Element index
- **Returns**: Removed JsonValue
- **Example**: `let removed: JsonValue = json_array_remove(arr, 0);`

#### Object Operations

##### `json_object_size(object: JsonValue): int`
Gets the number of properties in a JSON object.
- **Parameters**: `object` - JsonValue object
- **Returns**: Number of properties
- **Example**: `let size: int = json_object_size(obj);`

##### `json_object_has_key(object: JsonValue, key: string): bool`
Checks if a JSON object has a specific key.
- **Parameters**: 
  - `object` - JsonValue object
  - `key` - Key to check
- **Returns**: True if key exists
- **Example**: `let has_key: bool = json_object_has_key(obj, "name");`

##### `json_object_get(object: JsonValue, key: string): JsonValue`
Gets a property value from a JSON object.
- **Parameters**: 
  - `object` - JsonValue object
  - `key` - Property key
- **Returns**: JsonValue for the key
- **Example**: `let value: JsonValue = json_object_get(obj, "name");`

##### `json_object_set(object: JsonValue, key: string, value: JsonValue): void`
Sets a property in a JSON object.
- **Parameters**: 
  - `object` - JsonValue object
  - `key` - Property key
  - `value` - JsonValue to set
- **Example**: `json_object_set(obj, "name", json_string("John"));`

##### `json_object_remove(object: JsonValue, key: string): JsonValue`
Removes a property from a JSON object.
- **Parameters**: 
  - `object` - JsonValue object
  - `key` - Property key to remove
- **Returns**: Removed JsonValue
- **Example**: `let removed: JsonValue = json_object_remove(obj, "name");`

##### `json_object_keys(object: JsonValue): string[]`
Gets all keys from a JSON object.
- **Parameters**: `object` - JsonValue object
- **Returns**: Array of key strings
- **Example**: `let keys: string[] = json_object_keys(obj);`

#### Utility Functions

##### `json_clone(value: JsonValue): JsonValue`
Creates a deep copy of a JSON value.
- **Parameters**: `value` - JsonValue to clone
- **Returns**: Cloned JsonValue
- **Example**: `let copy: JsonValue = json_clone(original);`

##### `json_equals(value1: JsonValue, value2: JsonValue): bool`
Compares two JSON values for equality.
- **Parameters**: `value1`, `value2` - JsonValues to compare
- **Returns**: True if equal
- **Example**: `let equal: bool = json_equals(val1, val2);`

##### `json_merge(target: JsonValue, source: JsonValue): JsonValue`
Merges two JSON objects.
- **Parameters**: 
  - `target` - Target JsonValue object
  - `source` - Source JsonValue object
- **Returns**: Merged JsonValue
- **Example**: `let merged: JsonValue = json_merge(obj1, obj2);`

##### `json_path_get(value: JsonValue, path: string): JsonValue`
Gets a value using a JSON path expression.
- **Parameters**: 
  - `value` - Root JsonValue
  - `path` - JSON path (e.g., "user.name")
- **Returns**: JsonValue at path
- **Example**: `let name: JsonValue = json_path_get(root, "user.name");`

##### `json_path_set(value: JsonValue, path: string, new_value: JsonValue): bool`
Sets a value using a JSON path expression.
- **Parameters**: 
  - `value` - Root JsonValue
  - `path` - JSON path
  - `new_value` - JsonValue to set
- **Returns**: True if successful
- **Example**: `let success: bool = json_path_set(root, "user.age", json_number(25));`

---

## Network Module

The network module provides HTTP client/server functionality, socket operations, and network utilities.

### Constants

```ton
// HTTP Methods
const HTTP_GET: string = "GET";
const HTTP_POST: string = "POST";
const HTTP_PUT: string = "PUT";
const HTTP_DELETE: string = "DELETE";
const HTTP_PATCH: string = "PATCH";
const HTTP_HEAD: string = "HEAD";
const HTTP_OPTIONS: string = "OPTIONS";

// HTTP Status Codes
const HTTP_STATUS_OK: int = 200;
const HTTP_STATUS_CREATED: int = 201;
const HTTP_STATUS_NO_CONTENT: int = 204;
const HTTP_STATUS_BAD_REQUEST: int = 400;
const HTTP_STATUS_UNAUTHORIZED: int = 401;
const HTTP_STATUS_FORBIDDEN: int = 403;
const HTTP_STATUS_NOT_FOUND: int = 404;
const HTTP_STATUS_INTERNAL_ERROR: int = 500;

// Socket Types
const SOCKET_TCP: int = 1;
const SOCKET_UDP: int = 2;
```

### Data Structures

#### HttpRequest
```ton
struct HttpRequest {
    method: string;
    url: string;
    headers: Map<string, string>;
    body: string;
    timeout: int;
}
```

#### HttpResponse
```ton
struct HttpResponse {
    status_code: int;
    status_message: string;
    headers: Map<string, string>;
    body: string;
    content_length: int;
}
```

#### Socket
```ton
struct Socket {
    handle: int;
    type: int;
    is_connected: bool;
    local_address: string;
    local_port: int;
    remote_address: string;
    remote_port: int;
}
```

#### UrlComponents
```ton
struct UrlComponents {
    scheme: string;
    host: string;
    port: int;
    path: string;
    query: string;
    fragment: string;
}
```

### Functions

#### HTTP Client Operations

##### `http_request_create(): HttpRequest`
Creates a new HTTP request.
- **Returns**: New HttpRequest instance
- **Example**: `let request: HttpRequest = http_request_create();`

##### `http_request_set_method(request: HttpRequest, method: string): void`
Sets the HTTP method for a request.
- **Parameters**: 
  - `request` - HttpRequest to modify
  - `method` - HTTP method (GET, POST, etc.)
- **Example**: `http_request_set_method(request, HTTP_POST);`

##### `http_request_set_url(request: HttpRequest, url: string): void`
Sets the URL for a request.
- **Parameters**: 
  - `request` - HttpRequest to modify
  - `url` - Target URL
- **Example**: `http_request_set_url(request, "https://api.example.com/data");`

##### `http_request_add_header(request: HttpRequest, name: string, value: string): void`
Adds a header to a request.
- **Parameters**: 
  - `request` - HttpRequest to modify
  - `name` - Header name
  - `value` - Header value
- **Example**: `http_request_add_header(request, "Content-Type", "application/json");`

##### `http_request_set_body(request: HttpRequest, body: string): void`
Sets the request body.
- **Parameters**: 
  - `request` - HttpRequest to modify
  - `body` - Request body content
- **Example**: `http_request_set_body(request, json_data);`

##### `http_send_request(request: HttpRequest): HttpResponse`
Sends an HTTP request and returns the response.
- **Parameters**: `request` - HttpRequest to send
- **Returns**: HttpResponse from server
- **Example**: `let response: HttpResponse = http_send_request(request);`

##### `http_get(url: string): HttpResponse`
Performs a simple HTTP GET request.
- **Parameters**: `url` - Target URL
- **Returns**: HttpResponse from server
- **Example**: `let response: HttpResponse = http_get("https://api.example.com/users");`

##### `http_post(url: string, body: string): HttpResponse`
Performs a simple HTTP POST request.
- **Parameters**: 
  - `url` - Target URL
  - `body` - Request body
- **Returns**: HttpResponse from server
- **Example**: `let response: HttpResponse = http_post("https://api.example.com/users", user_data);`

##### `http_post_json(url: string, json_data: JsonValue): HttpResponse`
Performs an HTTP POST request with JSON data.
- **Parameters**: 
  - `url` - Target URL
  - `json_data` - JSON data to send
- **Returns**: HttpResponse from server
- **Example**: `let response: HttpResponse = http_post_json("https://api.example.com/users", user_json);`

#### HTTP Response Operations

##### `http_response_get_header(response: HttpResponse, name: string): string`
Gets a header value from a response.
- **Parameters**: 
  - `response` - HttpResponse to read from
  - `name` - Header name
- **Returns**: Header value or empty string
- **Example**: `let content_type: string = http_response_get_header(response, "Content-Type");`

##### `http_response_is_success(response: HttpResponse): bool`
Checks if a response indicates success (2xx status).
- **Parameters**: `response` - HttpResponse to check
- **Returns**: True if successful
- **Example**: `let success: bool = http_response_is_success(response);`

##### `http_response_is_error(response: HttpResponse): bool`
Checks if a response indicates an error (4xx or 5xx status).
- **Parameters**: `response` - HttpResponse to check
- **Returns**: True if error
- **Example**: `let error: bool = http_response_is_error(response);`

#### URL Operations

##### `url_parse(url: string): UrlComponents`
Parses a URL into its components.
- **Parameters**: `url` - URL string to parse
- **Returns**: UrlComponents structure
- **Example**: `let components: UrlComponents = url_parse("https://example.com:8080/path?query=value");`

##### `url_build(components: UrlComponents): string`
Builds a URL string from components.
- **Parameters**: `components` - UrlComponents to build from
- **Returns**: Complete URL string
- **Example**: `let url: string = url_build(components);`

##### `url_encode_query(params: Map<string, string>): string`
Encodes query parameters for a URL.
- **Parameters**: `params` - Map of parameter names and values
- **Returns**: Encoded query string
- **Example**: `let query: string = url_encode_query(params);`

##### `url_decode_query(query: string): Map<string, string>`
Decodes query parameters from a URL.
- **Parameters**: `query` - Query string to decode
- **Returns**: Map of parameter names and values
- **Example**: `let params: Map<string, string> = url_decode_query("name=John&age=30");`

#### TCP Socket Operations

##### `tcp_socket_create(): Socket`
Creates a new TCP socket.
- **Returns**: New Socket instance
- **Example**: `let socket: Socket = tcp_socket_create();`

##### `tcp_socket_connect(socket: Socket, host: string, port: int): bool`
Connects a TCP socket to a remote host.
- **Parameters**: 
  - `socket` - Socket to connect
  - `host` - Remote host address
  - `port` - Remote port number
- **Returns**: True if connection successful
- **Example**: `let connected: bool = tcp_socket_connect(socket, "example.com", 80);`

##### `tcp_socket_listen(socket: Socket, host: string, port: int): bool`
Binds a TCP socket to listen for connections.
- **Parameters**: 
  - `socket` - Socket to bind
  - `host` - Local host address
  - `port` - Local port number
- **Returns**: True if binding successful
- **Example**: `let listening: bool = tcp_socket_listen(socket, "localhost", 8080);`

##### `tcp_socket_accept(socket: Socket): Socket`
Accepts an incoming connection on a listening socket.
- **Parameters**: `socket` - Listening socket
- **Returns**: New socket for the accepted connection
- **Example**: `let client_socket: Socket = tcp_socket_accept(server_socket);`

##### `tcp_socket_send(socket: Socket, data: string): int`
Sends data through a TCP socket.
- **Parameters**: 
  - `socket` - Connected socket
  - `data` - Data to send
- **Returns**: Number of bytes sent
- **Example**: `let bytes_sent: int = tcp_socket_send(socket, "Hello World");`

##### `tcp_socket_receive(socket: Socket, buffer: string, size: int): int`
Receives data from a TCP socket.
- **Parameters**: 
  - `socket` - Connected socket
  - `buffer` - Buffer to receive into
  - `size` - Maximum bytes to receive
- **Returns**: Number of bytes received
- **Example**: `let bytes_received: int = tcp_socket_receive(socket, buffer, 1024);`

##### `tcp_socket_close(socket: Socket): bool`
Closes a TCP socket.
- **Parameters**: `socket` - Socket to close
- **Returns**: True if successful
- **Example**: `let closed: bool = tcp_socket_close(socket);`

#### UDP Socket Operations

##### `udp_socket_create(): Socket`
Creates a new UDP socket.
- **Returns**: New Socket instance
- **Example**: `let socket: Socket = udp_socket_create();`

##### `udp_socket_bind(socket: Socket, host: string, port: int): bool`
Binds a UDP socket to a local address.
- **Parameters**: 
  - `socket` - Socket to bind
  - `host` - Local host address
  - `port` - Local port number
- **Returns**: True if binding successful
- **Example**: `let bound: bool = udp_socket_bind(socket, "localhost", 8080);`

##### `udp_socket_send_to(socket: Socket, data: string, host: string, port: int): int`
Sends data to a specific address via UDP.
- **Parameters**: 
  - `socket` - UDP socket
  - `data` - Data to send
  - `host` - Target host address
  - `port` - Target port number
- **Returns**: Number of bytes sent
- **Example**: `let bytes_sent: int = udp_socket_send_to(socket, "Hello", "example.com", 8080);`

##### `udp_socket_receive_from(socket: Socket, buffer: string, size: int): int`
Receives data from any address via UDP.
- **Parameters**: 
  - `socket` - UDP socket
  - `buffer` - Buffer to receive into
  - `size` - Maximum bytes to receive
- **Returns**: Number of bytes received
- **Example**: `let bytes_received: int = udp_socket_receive_from(socket, buffer, 1024);`

#### Network Utility Functions

##### `ping(host: string, timeout: int): bool`
Pings a host to check connectivity.
- **Parameters**: 
  - `host` - Host to ping
  - `timeout` - Timeout in milliseconds
- **Returns**: True if host responds
- **Example**: `let reachable: bool = ping("google.com", 5000);`

##### `resolve_hostname(hostname: string): string`
Resolves a hostname to an IP address.
- **Parameters**: `hostname` - Hostname to resolve
- **Returns**: IP address string
- **Example**: `let ip: string = resolve_hostname("google.com");`

##### `get_local_ip(): string`
Gets the local machine's IP address.
- **Returns**: Local IP address
- **Example**: `let local_ip: string = get_local_ip();`

##### `is_port_open(host: string, port: int): bool`
Checks if a port is open on a host.
- **Parameters**: 
  - `host` - Host to check
  - `port` - Port number
- **Returns**: True if port is open
- **Example**: `let open: bool = is_port_open("example.com", 80);`

##### `download_file(url: string, local_path: string): bool`
Downloads a file from a URL.
- **Parameters**: 
  - `url` - File URL
  - `local_path` - Local save path
- **Returns**: True if successful
- **Example**: `let downloaded: bool = download_file("https://example.com/file.txt", "local_file.txt");`

---

## Testing Module

The testing module provides a comprehensive unit testing framework with assertions, test suites, mocking, and benchmarking.

### Constants

```ton
const TEST_RESULT_PASS: int = 0;
const TEST_RESULT_FAIL: int = 1;
const TEST_RESULT_SKIP: int = 2;
const TEST_RESULT_ERROR: int = 3;
```

### Data Structures

#### TestResult
```ton
struct TestResult {
    name: string;
    result: int;
    message: string;
    execution_time: float;
    assertions_count: int;
}
```

#### TestSuite
```ton
struct TestSuite {
    name: string;
    tests: TestResult[];
    setup_function: string;
    teardown_function: string;
    passed_count: int;
    failed_count: int;
    skipped_count: int;
    total_time: float;
}
```

### Functions

#### Assertion Functions

##### `assert(condition: bool, message: string): void`
Basic assertion that checks if a condition is true.
- **Parameters**: 
  - `condition` - Condition to test
  - `message` - Error message if assertion fails
- **Example**: `assert(result == 42, "Expected result to be 42");`

##### `assert_equals(actual: any, expected: any, message: string): void`
Asserts that two values are equal.
- **Parameters**: 
  - `actual` - Actual value
  - `expected` - Expected value
  - `message` - Error message if assertion fails
- **Example**: `assert_equals(calculate_sum(2, 3), 5, "Sum should be 5");`

##### `assert_not_equals(actual: any, expected: any, message: string): void`
Asserts that two values are not equal.
- **Parameters**: 
  - `actual` - Actual value
  - `expected` - Value that should not match
  - `message` - Error message if assertion fails
- **Example**: `assert_not_equals(result, 0, "Result should not be zero");`

##### `assert_null(value: any, message: string): void`
Asserts that a value is null.
- **Parameters**: 
  - `value` - Value to check
  - `message` - Error message if assertion fails
- **Example**: `assert_null(optional_value, "Optional value should be null");`

##### `assert_not_null(value: any, message: string): void`
Asserts that a value is not null.
- **Parameters**: 
  - `value` - Value to check
  - `message` - Error message if assertion fails
- **Example**: `assert_not_null(user_data, "User data should not be null");`

##### `assert_true(condition: bool, message: string): void`
Asserts that a condition is true.
- **Parameters**: 
  - `condition` - Condition to test
  - `message` - Error message if assertion fails
- **Example**: `assert_true(is_valid_email(email), "Email should be valid");`

##### `assert_false(condition: bool, message: string): void`
Asserts that a condition is false.
- **Parameters**: 
  - `condition` - Condition to test
  - `message` - Error message if assertion fails
- **Example**: `assert_false(is_empty(list), "List should not be empty");`

##### `assert_throws(function_call: string, message: string): void`
Asserts that a function call throws an exception.
- **Parameters**: 
  - `function_call` - Function call that should throw
  - `message` - Error message if assertion fails
- **Example**: `assert_throws("divide_by_zero()", "Should throw division by zero error");`

##### `assert_contains(container: string, item: string, message: string): void`
Asserts that a string contains a substring.
- **Parameters**: 
  - `container` - String to search in
  - `item` - Substring to find
  - `message` - Error message if assertion fails
- **Example**: `assert_contains(response_text, "success", "Response should contain success message");`

#### Test Suite Management

##### `test_suite_create(name: string): TestSuite`
Creates a new test suite.
- **Parameters**: `name` - Name of the test suite
- **Returns**: New TestSuite instance
- **Example**: `let suite: TestSuite = test_suite_create("Math Tests");`

##### `test_suite_add_test(suite: TestSuite, test_name: string, test_function: string): void`
Adds a test to a test suite.
- **Parameters**: 
  - `suite` - Target test suite
  - `test_name` - Name of the test
  - `test_function` - Function name to execute
- **Example**: `test_suite_add_test(suite, "Addition Test", "test_addition");`

##### `test_suite_run(suite: TestSuite): TestSuite`
Runs all tests in a test suite.
- **Parameters**: `suite` - Test suite to run
- **Returns**: Updated test suite with results
- **Example**: `let results: TestSuite = test_suite_run(suite);`

##### `test_suite_set_setup(suite: TestSuite, setup_function: string): void`
Sets a setup function to run before each test.
- **Parameters**: 
  - `suite` - Target test suite
  - `setup_function` - Setup function name
- **Example**: `test_suite_set_setup(suite, "setup_test_data");`

##### `test_suite_set_teardown(suite: TestSuite, teardown_function: string): void`
Sets a teardown function to run after each test.
- **Parameters**: 
  - `suite` - Target test suite
  - `teardown_function` - Teardown function name
- **Example**: `test_suite_set_teardown(suite, "cleanup_test_data");`

#### Test Runner Functions

##### `test_run_single(test_name: string, test_function: string): TestResult`
Runs a single test function.
- **Parameters**: 
  - `test_name` - Name of the test
  - `test_function` - Function name to execute
- **Returns**: Test result
- **Example**: `let result: TestResult = test_run_single("Basic Test", "test_basic_functionality");`

##### `test_skip(message: string): void`
Skips the current test with a message.
- **Parameters**: `message` - Reason for skipping
- **Example**: `test_skip("Feature not implemented yet");`

##### `test_fail(message: string): void`
Explicitly fails the current test.
- **Parameters**: `message` - Failure reason
- **Example**: `test_fail("Critical condition not met");`

#### Utility Functions

##### `measure_time(function_call: string): float`
Measures the execution time of a function call.
- **Parameters**: `function_call` - Function call to measure
- **Returns**: Execution time in milliseconds
- **Example**: `let time: float = measure_time("expensive_operation()");`

##### `test_report_console(suite: TestSuite): void`
Prints test results to the console.
- **Parameters**: `suite` - Test suite with results
- **Example**: `test_report_console(results);`

##### `test_report_html(suite: TestSuite, output_path: string): bool`
Generates an HTML test report.
- **Parameters**: 
  - `suite` - Test suite with results
  - `output_path` - Path for HTML report
- **Returns**: True if successful
- **Example**: `let success: bool = test_report_html(results, "test_report.html");`

---

## Memory Module

The memory module provides memory management, garbage collection, and memory debugging capabilities.

### Constants

```ton
const MEMORY_ALLOC_STANDARD: int = 0;
const MEMORY_ALLOC_ZERO: int = 1;
const MEMORY_ALLOC_ALIGNED: int = 2;
```

### Data Structures

#### MemoryStats
```ton
struct MemoryStats {
    total_allocated: int;
    current_usage: int;
    peak_usage: int;
    allocation_count: int;
    free_count: int;
    leak_count: int;
}
```

### Functions

#### Basic Memory Allocation

##### `alloc(size: int): any`
Allocates memory of specified size.
- **Parameters**: `size` - Size in bytes
- **Returns**: Pointer to allocated memory
- **Example**: `let ptr: any = alloc(1024);`

##### `free(ptr: any): void`
Frees previously allocated memory.
- **Parameters**: `ptr` - Pointer to memory to free
- **Example**: `free(ptr);`

##### `realloc(ptr: any, new_size: int): any`
Reallocates memory to a new size.
- **Parameters**: 
  - `ptr` - Existing memory pointer
  - `new_size` - New size in bytes
- **Returns**: Pointer to reallocated memory
- **Example**: `let new_ptr: any = realloc(ptr, 2048);`

#### Memory Statistics

##### `memory_get_stats(): MemoryStats`
Gets current memory usage statistics.
- **Returns**: MemoryStats structure
- **Example**: `let stats: MemoryStats = memory_get_stats();`

##### `memory_get_usage(): int`
Gets current memory usage in bytes.
- **Returns**: Current memory usage
- **Example**: `let usage: int = memory_get_usage();`

##### `memory_get_peak_usage(): int`
Gets peak memory usage in bytes.
- **Returns**: Peak memory usage
- **Example**: `let peak: int = memory_get_peak_usage();`

#### Memory Utilities

##### `memory_copy(dest: any, src: any, size: int): void`
Copies memory from source to destination.
- **Parameters**: 
  - `dest` - Destination pointer
  - `src` - Source pointer
  - `size` - Number of bytes to copy
- **Example**: `memory_copy(dest_ptr, src_ptr, 256);`

##### `memory_set(ptr: any, value: int, size: int): void`
Sets memory to a specific value.
- **Parameters**: 
  - `ptr` - Memory pointer
  - `value` - Value to set (0-255)
  - `size` - Number of bytes to set
- **Example**: `memory_set(ptr, 0, 1024); // Zero out memory`

##### `memory_compare(ptr1: any, ptr2: any, size: int): int`
Compares two memory regions.
- **Parameters**: 
  - `ptr1` - First memory pointer
  - `ptr2` - Second memory pointer
  - `size` - Number of bytes to compare
- **Returns**: 0 if equal, negative if ptr1 < ptr2, positive if ptr1 > ptr2
- **Example**: `let result: int = memory_compare(ptr1, ptr2, 128);`

---

## System Module

The system module provides system information, process management, and operating system interactions.

### Constants

```ton
const OS_WINDOWS: int = 1;
const OS_LINUX: int = 2;
const OS_MACOS: int = 3;
const OS_UNKNOWN: int = 0;

const PROCESS_STATUS_RUNNING: int = 1;
const PROCESS_STATUS_STOPPED: int = 2;
const PROCESS_STATUS_ZOMBIE: int = 3;
```

### Data Structures

#### SystemInfo
```ton
struct SystemInfo {
    os_type: int;
    os_name: string;
    os_version: string;
    architecture: string;
    hostname: string;
    username: string;
    cpu_count: int;
    total_memory: int;
    uptime: int;
}
```

#### ProcessInfo
```ton
struct ProcessInfo {
    pid: int;
    name: string;
    status: int;
    cpu_usage: float;
    memory_usage: int;
    start_time: int;
}
```

### Functions

#### System Information

##### `system_get_info(): SystemInfo`
Gets comprehensive system information.
- **Returns**: SystemInfo structure
- **Example**: `let info: SystemInfo = system_get_info();`

##### `system_get_os_type(): int`
Gets the operating system type.
- **Returns**: OS type constant
- **Example**: `let os: int = system_get_os_type();`

##### `system_get_hostname(): string`
Gets the system hostname.
- **Returns**: Hostname string
- **Example**: `let hostname: string = system_get_hostname();`

##### `system_get_username(): string`
Gets the current username.
- **Returns**: Username string
- **Example**: `let user: string = system_get_username();`

#### Environment Variables

##### `env_get(name: string): string`
Gets an environment variable value.
- **Parameters**: `name` - Variable name
- **Returns**: Variable value or empty string
- **Example**: `let path: string = env_get("PATH");`

##### `env_set(name: string, value: string): bool`
Sets an environment variable.
- **Parameters**: 
  - `name` - Variable name
  - `value` - Variable value
- **Returns**: True if successful
- **Example**: `let success: bool = env_set("MY_VAR", "my_value");`

##### `env_exists(name: string): bool`
Checks if an environment variable exists.
- **Parameters**: `name` - Variable name
- **Returns**: True if variable exists
- **Example**: `let exists: bool = env_exists("HOME");`

#### Process Management

##### `process_get_pid(): int`
Gets the current process ID.
- **Returns**: Process ID
- **Example**: `let pid: int = process_get_pid();`

##### `process_get_info(pid: int): ProcessInfo`
Gets information about a process.
- **Parameters**: `pid` - Process ID
- **Returns**: ProcessInfo structure
- **Example**: `let info: ProcessInfo = process_get_info(1234);`

##### `process_exists(pid: int): bool`
Checks if a process exists.
- **Parameters**: `pid` - Process ID
- **Returns**: True if process exists
- **Example**: `let exists: bool = process_exists(1234);`

##### `process_execute(command: string): int`
Executes a system command.
- **Parameters**: `command` - Command to execute
- **Returns**: Exit code
- **Example**: `let exit_code: int = process_execute("ls -la");`

#### Time Functions

##### `system_get_time(): int`
Gets current system time as timestamp.
- **Returns**: Unix timestamp
- **Example**: `let timestamp: int = system_get_time();`

##### `system_sleep(milliseconds: int): void`
Pauses execution for specified milliseconds.
- **Parameters**: `milliseconds` - Sleep duration
- **Example**: `system_sleep(1000); // Sleep for 1 second`

---

## Usage Examples

### Basic Example
```ton
// Import modules
import "tonlib/core.ton";
import "tonlib/math.ton";
import "tonlib/string.ton";

function main(): void {
    // Core operations
    let num_str: string = "42";
    let num: int = string_to_int(num_str);
    
    // Math operations
    let result: float = pow(2.0, 3.0);
    let rounded: float = round(3.14159);
    
    // String operations
    let message: string = concat("Hello", " World");
    let upper: string = to_upper(message);
    
    print("Number: " + int_to_string(num));
    print("Power result: " + float_to_string(result));
    print("Message: " + upper);
}
```

### Advanced Example
```ton
// Import modules
import "tonlib/collections.ton";
import "tonlib/json.ton";
import "tonlib/network.ton";

function fetch_and_process_data(): void {
    // Create a list to store results
    let results: List<string> = List_new();
    
    // Make HTTP request
    let response: HttpResponse = http_get("https://api.example.com/data");
    
    if (http_response_is_success(response)) {
        // Parse JSON response
        let json_result: JsonResult = json_parse(response.body);
        
        if (json_result.success) {
            // Process JSON data
            let data: JsonValue = json_result.value;
            
            if (json_is_array(data)) {
                let length: int = json_array_length(data);
                
                for (let i: int = 0; i < length; i = i + 1) {
                    let item: JsonValue = json_array_get(data, i);
                    let item_str: string = json_get_string(item, "");
                    List_add(results, item_str);
                }
            }
        }
    }
    
    // Print results
    let size: int = List_size(results);
    print("Processed " + int_to_string(size) + " items");
}
```

---

**Author: v3nn7**

This API reference provides comprehensive documentation for all TonLib modules. For more examples and usage patterns, see the examples directory in the TonLib repository.