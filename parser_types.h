#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include "ast.h"
#include "parser.h"
#include "token.h"

// Function declarations for parsing types
TypeNode* parse_type(Parser* parser);

#endif // PARSER_TYPES_H