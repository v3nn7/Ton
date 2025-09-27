#ifndef PARSER_DECLARATIONS_H
#define PARSER_DECLARATIONS_H

#include "ast.h"
#include "parser.h"
#include "token.h"

// Function declarations for parsing declarations
ASTNode* parse_variable_declaration(Parser* parser);
// Add other declaration parsing functions here as they are moved

#endif // PARSER_DECLARATIONS_H