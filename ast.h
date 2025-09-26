#ifndef AST_H
#define AST_H

#include "lexer.h"
#include <stdbool.h> // Dodaj to

// Forward declarations for AST node types
typedef struct ASTNode ASTNode;
typedef struct ProgramNode ProgramNode;
typedef struct VariableDeclarationNode VariableDeclarationNode;
typedef struct FunctionDeclarationNode FunctionDeclarationNode;
typedef struct BlockStatementNode BlockStatementNode;
typedef struct IfStatementNode IfStatementNode;
typedef struct LoopStatementNode LoopStatementNode;
typedef struct ForStatementNode ForStatementNode;
typedef struct WhileStatementNode WhileStatementNode;
typedef struct SwitchStatementNode SwitchStatementNode;
typedef struct CaseStatementNode CaseStatementNode;
typedef struct BreakStatementNode BreakStatementNode;
typedef struct StructDeclarationNode StructDeclarationNode;
typedef struct ImportStatementNode ImportStatementNode;
// Forward declarations for new AST node types
typedef struct ClassDeclarationNode ClassDeclarationNode;
typedef struct TryStatementNode TryStatementNode;
typedef struct CatchStatementNode CatchStatementNode;
typedef struct FinallyStatementNode FinallyStatementNode;
typedef struct ThrowStatementNode ThrowStatementNode;
typedef struct ModuleDeclarationNode ModuleDeclarationNode;
typedef struct ExportStatementNode ExportStatementNode;
typedef struct SuperExpressionNode SuperExpressionNode;
typedef struct ContinueStatementNode ContinueStatementNode;
typedef struct ReturnStatementNode ReturnStatementNode;
typedef struct PrintStatementNode PrintStatementNode;
typedef struct ExpressionStatementNode ExpressionStatementNode;
typedef struct BinaryExpressionNode BinaryExpressionNode;
typedef struct UnaryExpressionNode UnaryExpressionNode;
typedef struct ConditionalExpressionNode ConditionalExpressionNode;
typedef struct TypeofExpressionNode TypeofExpressionNode;
typedef struct LiteralExpressionNode LiteralExpressionNode;
typedef struct IdentifierExpressionNode IdentifierExpressionNode;
typedef struct FunctionCallExpressionNode FunctionCallExpressionNode;
typedef struct ArrayLiteralExpressionNode ArrayLiteralExpressionNode;
typedef struct ArrayAccessExpressionNode ArrayAccessExpressionNode;
typedef struct MemberAccessExpressionNode MemberAccessExpressionNode;
typedef struct NewExpressionNode NewExpressionNode;
typedef struct ParameterNode ParameterNode;
typedef struct TypeNode TypeNode;

// Enum for AST node types
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECLARATION,
    NODE_FN_DECLARATION,
    NODE_IMPORT_STATEMENT,
    NODE_BLOCK_STATEMENT,
    NODE_IF_STATEMENT,
    NODE_LOOP_STATEMENT,
    NODE_FOR_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_SWITCH_STATEMENT,
    NODE_CASE_STATEMENT,
    NODE_BREAK_STATEMENT,
    NODE_CONTINUE_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_PRINT_STATEMENT, // Add this line
    NODE_STRUCT_DECLARATION,
    NODE_CLASS_DECLARATION,     // New: class declaration
    NODE_TRY_STATEMENT,         // New: try statement
    NODE_CATCH_STATEMENT,       // New: catch statement
    NODE_FINALLY_STATEMENT,     // New: finally statement
    NODE_THROW_STATEMENT,       // New: throw statement
    NODE_MODULE_DECLARATION,    // New: module declaration
    NODE_EXPORT_STATEMENT,      // New: export statement
    NODE_SUPER_EXPRESSION,      // New: super expression
    NODE_EXPRESSION_STATEMENT,
    NODE_BINARY_EXPRESSION,
    NODE_UNARY_EXPRESSION,
    NODE_CONDITIONAL_EXPRESSION,
    NODE_TYPEOF_EXPRESSION,
    NODE_LITERAL_EXPRESSION,
    NODE_IDENTIFIER_EXPRESSION,
    NODE_FN_CALL_EXPRESSION,
    NODE_ARRAY_LITERAL_EXPRESSION,
    NODE_ARRAY_ACCESS_EXPRESSION,
    NODE_MEMBER_ACCESS_EXPRESSION,
    NODE_NEW_EXPRESSION,
    NODE_PARAMETER
} ASTNodeType;

typedef enum {
    VAR_TYPE_INT,
    VAR_TYPE_FLOAT,
    VAR_TYPE_BOOL,
    VAR_TYPE_STRING,
    VAR_TYPE_CHAR,
    VAR_TYPE_ARRAY,
    VAR_TYPE_POINTER, // Add pointer type
    VAR_TYPE_VOID, // Add TYPE_VOID
    VAR_TYPE_UNKNOWN
} VariableType;

// Base AST Node structure
typedef struct ASTNode {
    ASTNodeType type;
    // Common fields like line/column for error reporting
    int line;
    int column;
} ASTNode;

// Program Node: Root of the AST
struct ProgramNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode** statements; // Array of statement nodes
    int num_statements;
};

// Variable Declaration Node: let x: int = 10;
struct VariableDeclarationNode {
    ASTNode base; // Embed base ASTNode
    VariableType var_type;   // Use VariableType enum
    char* identifier; // Use char* for identifier name
    ASTNode* initializer; // Expression for the initial value (can be NULL)
    int is_const;      // 1 if const, 0 if let
    int is_array;      // 1 if array type, 0 if scalar
    int array_size;    // Size of array (if is_array is 1)
};

// Function Parameter Node
struct ParameterNode {
    ASTNodeType type;
    int line;
    int column;
    Token* identifier; // Parameter name
    VariableType param_type; // Parameter type, changed from Token* to VariableType
};

// Function Declaration Node: fn add(a: int, b: int): int { ... }
struct FunctionDeclarationNode {
    ASTNodeType type;
    int line;
    int column;
    Token* identifier; // Function name
    ParameterNode** parameters; // Array of parameter nodes
    int num_parameters;
    VariableType return_type; // Changed from Token* to VariableType
    BlockStatementNode* body; // Function body
};

// Block Statement Node: { ... statements ... }
struct BlockStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode** statements; // Array of statement nodes
    int num_statements;
};

// If Statement Node: if condition { ... } else { ... }
struct IfStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* condition; // Expression for the condition
    BlockStatementNode* consequence; // Block for the 'if' part
    BlockStatementNode* alternative; // Block for the 'else' part (can be NULL)
};

// Loop Statement Node: loop { ... }, loop i from 0 to 10 { ... }, loop item in collection { ... }
typedef enum {
    LOOP_INFINITE,
    LOOP_RANGE,
    LOOP_IN,
} LoopType;

struct LoopStatementNode {
    ASTNodeType type;
    int line;
    int column;
    LoopType loop_type;
    Token* iterator; // Identifier for 'i' or 'item' (can be NULL)
    ASTNode* start_expr; // For LOOP_RANGE (can be NULL)
    ASTNode* end_expr;   // For LOOP_RANGE (can be NULL)
    ASTNode* collection_expr; // For LOOP_IN (can be NULL)
    BlockStatementNode* body;
};

// For Statement Node: for (init; condition; update) { ... }
struct ForStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* init; // Initialization statement (can be NULL)
    ASTNode* condition; // Loop condition (can be NULL)
    ASTNode* update; // Update expression (can be NULL)
    BlockStatementNode* body; // Loop body
};

// While Statement Node: while (condition) { ... }
struct WhileStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* condition; // Expression for the condition
    BlockStatementNode* body; // Loop body
};

// Switch Statement Node: switch (expr) { case value: ... default: ... }
struct SwitchStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* expression; // Expression to switch on
    CaseStatementNode** cases; // Array of case statements
    int num_cases;
    BlockStatementNode* default_case; // Default case (can be NULL)
};

// Case Statement Node: case value: statements
struct CaseStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* value; // Value to match against
    ASTNode** statements; // Array of statements in this case
    int num_statements;
};

// Break Statement Node: break;
struct BreakStatementNode {
    ASTNodeType type;
    int line;
    int column;
};

// Continue Statement Node: continue;
struct ContinueStatementNode {
    ASTNodeType type;
    int line;
    int column;
};

// Return Statement Node: return expr;
struct ReturnStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* expression; // Expression to return (can be NULL)
};

// Print Statement Node: print expr;
struct PrintStatementNode {
    ASTNode base; // Embed base ASTNode
    ASTNode* expression; // Expression to print
};

// Expression Statement Node: An expression used as a statement (e.g., function call)
struct ExpressionStatementNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* expression;
};

// Import Statement Node: import "module_path";
struct ImportStatementNode {
    ASTNodeType type;
    int line;
    int column;
    char* module_path; // Path to the module to import
};

// Struct Declaration Node: struct Point { x: int, y: int }
struct StructDeclarationNode {
    ASTNodeType type;
    int line;
    int column;
    char* name;
    char** field_names;
    VariableType* field_types;
    int num_fields;
    // Methods support
    FunctionDeclarationNode** methods;
    int num_methods;
};

// Class Declaration Node: class Animal extends Object { ... }
struct ClassDeclarationNode {
    ASTNodeType type;
    int line;
    int column;
    char* name;
    char* parent_name;          // for inheritance (extends)
    char** field_names;
    VariableType* field_types;
    int* field_access;          // access modifiers for fields
    int num_fields;
    FunctionDeclarationNode** methods;
    int* method_access;         // access modifiers for methods
    int* method_flags;          // virtual, override, constructor, destructor flags
    int num_methods;
};

// Binary Expression Node: a + b
struct BinaryExpressionNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* left;
    Token* operator; // Token for the operator (e.g., TOKEN_PLUS)
    ASTNode* right;
};

// Unary Expression Node: !a, -a
struct UnaryExpressionNode {
    ASTNodeType type;
    int line;
    int column;
    Token* operator; // Token for the operator (e.g., TOKEN_NOT, TOKEN_MINUS)
    ASTNode* operand;
};

// Conditional Expression Node: condition ? true_expr : false_expr
struct ConditionalExpressionNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* condition;
    ASTNode* true_expr;
    ASTNode* false_expr;
};

// Typeof Expression Node: typeof(expression)
struct TypeofExpressionNode {
    ASTNodeType type;
    int line;
    int column;
    ASTNode* operand;
};

// Literal Expression Node: 10, 3.14, "hello", 'c'
struct LiteralExpressionNode {
    ASTNode base; // Embed base ASTNode
    Token* value; // The literal token itself (e.g., TOKEN_INT_LITERAL)
};

// Identifier Expression Node: myVariable
struct IdentifierExpressionNode {
    ASTNode base; // Embed base ASTNode
    char* identifier; // The identifier string
};

// Function Call Expression Node: myFunction(arg1, arg2)
struct FunctionCallExpressionNode {
    ASTNode base; // Embed base ASTNode
    ASTNode* callee; // IdentifierExpressionNode for the function name
    ASTNode** arguments; // Array of expression nodes for arguments
    int num_arguments;
};

// Array Literal Expression Node: [1, 2, 3]
struct ArrayLiteralExpressionNode {
    ASTNode base; // Embed base ASTNode
    ASTNode** elements; // Array of expression nodes
    int num_elements;
    VariableType element_type; // Type of array elements
};

// Array Access Expression Node: arr[index]
struct ArrayAccessExpressionNode {
    ASTNode base; // Embed base ASTNode
    ASTNode* array; // Array expression
    ASTNode* index; // Index expression
};

// Member Access Expression Node: obj.field, this.x
struct MemberAccessExpressionNode {
    ASTNode base; // Embed base ASTNode
    ASTNode* object; // Object expression (e.g., 'this', 'obj')
    char* member; // Member name (e.g., 'x', 'field')
};

// New Expression Node: new ClassName(args...)
struct NewExpressionNode {
    ASTNode base; // Embed base ASTNode
    char* class_name; // Name of the class to instantiate
    ASTNode** arguments; // Array of argument expressions
    int num_arguments; // Number of arguments
};

// Type Node: Represents a type (e.g., int, float, bool)
struct TypeNode {
    ASTNodeType type;
    int line;
    int column;
    VariableType var_type;
};

// Function to free AST nodes
void free_ast_node(ASTNode* node);

ASTNode* create_binary_expression_node(ASTNode* left, TokenType operator_type, ASTNode* right);
IdentifierExpressionNode* create_identifier_expression_node(const char* identifier, int line, int column);
ASTNode* create_typeof_expression_node(ASTNode* operand, int line, int column);
ASTNode* create_integer_literal_node(int value, int line, int column);
ASTNode* create_float_literal_node(double value, int line, int column);
ASTNode* create_string_literal_node(const char* value, int line, int column);
ASTNode* create_char_literal_node(char value, int line, int column);
ASTNode* create_boolean_literal_node(bool value, int line, int column);
ASTNode* create_new_expression_node(const char* class_name, ASTNode** arguments, int num_arguments, int line, int column);

#endif // AST_H