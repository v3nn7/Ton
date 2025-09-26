# TonLib - Comprehensive Ton Language Library

**Author: v3nn7**

TonLib is a comprehensive library for the Ton programming language, providing essential functionality for modern application development.

## Quick Start

To use TonLib in your project, simply import the main library file:

```ton
import "tonlib/tonlib.ton";

function main(): void {
    tonlib_init();  // Initialize TonLib
    
    // Now you can use all TonLib functions
    let message: string = "Hello World";
    let upper: string = to_upper(message);
    print(upper);
    
    let area: float = PI * pow(5.0, 2.0);
    print("Circle area: " + float_to_string(area));
    
    tonlib_info();  // Display library information
}
```

## Alternative Import Methods

### Import All Modules (Recommended)
```ton
import "tonlib/tonlib.ton";  // Imports everything
```

### Import Specific Modules
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

## 📚 Modules

### 🔧 Core - Essential Functions
- Basic data types and conversions
- Helper utilities
- Error handling

### 🧮 Math - Mathematics
- Trigonometric functions
- Logarithms and powers
- Mathematical constants
- Statistical functions

### 🔤 String - String Operations
- Concatenation and formatting
- Search and replace
- Case conversions
- Validation and parsing

### 📦 Collections - Data Structures
- Dynamic lists
- Maps (dictionaries)
- Sets
- Queues and stacks

### 💾 IO - Input/Output
- File operations
- Data streams
- Output formatting
- Directory handling

### 🌐 Network - Networking
- HTTP client/server
- TCP/UDP sockets
- URL parsing
- Basic protocols

### 📄 JSON - JSON Support
- JSON parsing
- Object serialization
- Schema validation
- Pretty printing

### 🧪 Testing - Test Framework
- Assertions and unit tests
- Mocking and stubbing
- Result reporting
- Benchmarking

### 🧠 Memory - Memory Management
- Allocation and deallocation
- Garbage collection
- Memory pools
- Leak detection

### ⚙️ System - System Functions
- Environment variables
- Processes and threads
- Time and date
- System information

## 📖 Documentation

Each module contains detailed documentation with usage examples.

## 🤝 Contributing

TonLib is an open-source project. We welcome contributions!