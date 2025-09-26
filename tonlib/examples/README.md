# TonLib Examples

**Author: v3nn7**

This directory contains comprehensive examples demonstrating how to use the TonLib library modules. The examples are designed to showcase both basic usage patterns and advanced real-world scenarios.

## Example Files

### 1. `basic_usage.ton`
Demonstrates fundamental usage of all TonLib modules:

- **Core Module**: Type conversions, utility functions, error handling
- **Math Module**: Basic operations, trigonometry, logarithms, random numbers
- **String Module**: Text manipulation, searching, validation, formatting
- **Collections Module**: Lists, Maps, Sets, Queues, Stacks
- **IO Module**: File operations, directory management, path utilities
- **JSON Module**: Parsing, serialization, object/array manipulation
- **Network Module**: HTTP requests, URL parsing, socket operations
- **Testing Module**: Unit testing, assertions, mocking, benchmarking
- **Memory Module**: Memory allocation, pools, garbage collection
- **System Module**: OS information, environment variables, process management

### 2. `advanced_examples.ton`
Shows complex usage patterns and real-world scenarios:

- **Web Server**: HTTP server setup with multiple route handlers
- **JSON API Client**: RESTful API consumption with error handling
- **File Processing**: Batch CSV processing with data transformation
- **Data Analysis**: Statistical analysis, histograms, data visualization
- **Concurrent Processing**: Multi-threaded task processing simulation
- **Configuration Manager**: Multi-source configuration loading and validation
- **Logging System**: Structured logging with multiple appenders and levels
- **Cache Implementation**: Memory, LRU, and TTL cache implementations

## Running the Examples

To run these examples, you'll need a Ton language interpreter or compiler. The examples are designed to be self-contained and demonstrate the expected API usage.

```bash
# Basic usage examples
ton run basic_usage.ton

# Advanced examples
ton run advanced_examples.ton
```

## Code Structure

Each example follows these conventions:

1. **Clear Documentation**: Each function and section is well-documented
2. **Error Handling**: Proper error checking and validation
3. **Resource Management**: Appropriate cleanup and resource disposal
4. **Best Practices**: Following Ton language conventions and patterns

## Example Categories

### Beginner Examples
- Basic function calls
- Simple data manipulation
- File I/O operations
- String processing

### Intermediate Examples
- JSON API integration
- Data structure usage
- Error handling patterns
- Configuration management

### Advanced Examples
- Concurrent programming
- Performance optimization
- System integration
- Complex data processing

## Key Concepts Demonstrated

### 1. Module Usage
```ton
// Import and use TonLib modules
let result: string = string_function("input");
let data: List<string> = List_new();
```

### 2. Error Handling
```ton
let result: JsonResult = json_parse(json_text);
if (result.success) {
    // Handle success
} else {
    // Handle error
    print("Error: " + result.error_message);
}
```

### 3. Resource Management
```ton
let file: File = file_open("data.txt", FILE_MODE_READ);
if (file_is_valid(file)) {
    let content: string = file_read_all(file);
    file_close(file); // Always clean up
}
```

### 4. Data Processing
```ton
// Process collections efficiently
let numbers: List<int> = List_new();
for (let i: int = 0; i < 100; i++) {
    List_add(numbers, i * 2);
}
```

## Testing the Examples

The examples include built-in testing demonstrations:

```ton
// Unit testing example
assert_equals("hello", "hello", "String equality test");
assert_true(List_contains(list, "item"), "List contains item");

// Benchmarking example
let bench_result: BenchmarkResult = benchmark_run("operation", 1000);
benchmark_print_results(bench_result);
```

## Performance Considerations

The examples demonstrate several performance best practices:

1. **Memory Management**: Proper allocation and deallocation
2. **Efficient Data Structures**: Choosing the right collection types
3. **Batch Operations**: Processing data in chunks
4. **Resource Pooling**: Reusing expensive resources
5. **Caching**: Implementing various caching strategies

## Integration Patterns

### HTTP API Integration
```ton
// RESTful API client pattern
let response: HttpResponse = http_get("https://api.example.com/data");
if (http_response_is_success(response)) {
    let data: JsonResult = json_parse(response.body);
    // Process data
}
```

### Configuration Management
```ton
// Multi-source configuration
let config: ConfigManager = create_config_manager();
load_config_from_file(config, "app.json");
load_config_from_env(config);
let port: int = config_get_int(config, "server.port", 8080);
```

### Logging and Monitoring
```ton
// Structured logging
let logger: Logger = create_logger("MyApp");
log_info_with_context(logger, "Operation completed", context);
log_metric(logger, "response_time_ms", 150.5);
```

## Common Patterns

### 1. Builder Pattern
```ton
let request: HttpRequest = http_request_create()
    .set_url("https://api.example.com")
    .set_method("POST")
    .add_header("Content-Type", "application/json")
    .set_body(json_data);
```

### 2. Factory Pattern
```ton
let cache: Cache = create_lru_cache(1000);
let logger: Logger = create_logger("component");
let server: HttpServer = http_server_create("localhost", 8080);
```

### 3. Observer Pattern
```ton
let handler: HttpHandler = create_handler();
http_server_add_handler(server, "GET", "/api/data", handler);
```

## Troubleshooting

### Common Issues

1. **Import Errors**: Ensure TonLib modules are properly installed
2. **Type Mismatches**: Check function signatures and return types
3. **Resource Leaks**: Always close files, sockets, and other resources
4. **Memory Issues**: Use appropriate data structures for your use case

### Debugging Tips

1. Use the logging system for debugging information
2. Implement proper error handling for all operations
3. Use assertions to validate assumptions
4. Monitor memory usage in long-running applications

## Contributing

To add new examples:

1. Follow the existing code style and documentation patterns
2. Include both basic and advanced usage scenarios
3. Add proper error handling and resource management
4. Document any special requirements or dependencies

## License

These examples are part of the TonLib project and follow the same licensing terms.

---

For more information about TonLib, see the main documentation in the project root directory.