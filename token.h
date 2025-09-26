#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    TOKEN_EOF = 0,          // End of file
    TOKEN_IDENTIFIER,       // e.g., variableName, functionName
    TOKEN_INT_LITERAL,      // e.g., 123, 0, 42
    TOKEN_FLOAT_LITERAL,    // e.g., 3.14, 0.5, 1.0
    TOKEN_STRING_LITERAL,   // e.g., "hello", "world"
    TOKEN_CHAR_LITERAL,     // e.g., 'a', 'Z'

    // Keywords
    TOKEN_LET,              // let
    TOKEN_CONST,            // const
    TOKEN_FN,               // fn
    TOKEN_IF,               // if
    TOKEN_ELSE,             // else
    TOKEN_LOOP,             // loop
    TOKEN_FROM,             // from
    TOKEN_TO,               // to
    TOKEN_IN,               // in
    TOKEN_RETURN,           // return
    TOKEN_VAR,              // var (from token.h, similar to let)
    TOKEN_WHILE,            // while (from token.h, similar to loop)
    TOKEN_FOR,              // for
    TOKEN_PRINT,            // print
    TOKEN_SWITCH,           // switch
    TOKEN_CASE,             // case
    TOKEN_DEFAULT,          // default
    TOKEN_BREAK,            // break
    TOKEN_CONTINUE,         // continue
    TOKEN_STRUCT,           // struct
    TOKEN_IMPORT,           // import
    // TOKEN_READ_LINE,        // read_line (Removed, handled as IDENTIFIER)
    TOKEN_TRUE,             // true
    TOKEN_FALSE,            // false
    TOKEN_TYPEOF,           // typeof

    // Access modifiers
    TOKEN_PUBLIC,           // public
    TOKEN_PRIVATE,          // private

    // Types
    TOKEN_TYPE_INT,         // int
    TOKEN_TYPE_FLOAT,       // float
    TOKEN_TYPE_BOOL,        // bool
    TOKEN_TYPE_STRING,      // string
    TOKEN_TYPE_CHAR,        // char
    TOKEN_TYPE_VOID,        // void

    // Operators
    TOKEN_ASSIGN,           // =
    TOKEN_PLUS_ASSIGN,      // +=
    TOKEN_MINUS_ASSIGN,     // -=
    TOKEN_STAR_ASSIGN,      // *=
    TOKEN_SLASH_ASSIGN,     // /=
    TOKEN_MODULO_ASSIGN,    // %=
    TOKEN_PLUS,             // +
    TOKEN_MINUS,            // -
    TOKEN_STAR,             // * (from lexer.h, same as MULTIPLY)
    TOKEN_SLASH,            // / (from lexer.h, same as DIVIDE)
    TOKEN_MODULO,           // %
    TOKEN_INCREMENT,        // ++
    TOKEN_DECREMENT,        // --
    TOKEN_EQ,               // ==
    TOKEN_NEQ,              // !=
    TOKEN_LT,               // <
    TOKEN_GT,               // >
    TOKEN_LE,               // <= (from lexer.h, same as LTE)
    TOKEN_GE,               // >= (from lexer.h, same as GTE)
    TOKEN_AND,              // &&
    TOKEN_OR,               // ||
    TOKEN_NOT,              // !
    TOKEN_AMPERSAND,        // & (address operator)
    TOKEN_QUESTION,         // ? (ternary operator)

    // Delimiters
    TOKEN_LPAREN,           // ( (from lexer.h, same as OPEN_PAREN)
    TOKEN_RPAREN,           // ) (from lexer.h, same as CLOSE_PAREN)
    TOKEN_LBRACE,           // { (from lexer.h, same as OPEN_BRACE)
    TOKEN_RBRACE,           // } (from lexer.h, same as CLOSE_BRACE)
    TOKEN_LBRACKET,         // [
    TOKEN_RBRACKET,         // ]
    TOKEN_COMMA,            // ,
    TOKEN_COLON,            // :
    TOKEN_SEMICOLON,        // ;
    TOKEN_DOT,              // .
    TOKEN_ARROW,            // ->

    // Special
    TOKEN_COMMENT,          // For comments
    TOKEN_ERROR             // For unrecognized tokens (from lexer.h, same as ILLEGAL)
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
} Token;

Token* create_token(TokenType type, const char* lexeme, int line, int column);
void free_token(Token* token);

#endif // TOKEN_H