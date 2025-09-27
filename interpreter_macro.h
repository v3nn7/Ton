#ifndef INTERPRETER_MACRO_H
#define INTERPRETER_MACRO_H

#include "ast.h"
#include "value.h"
#include "environment.h"

// Forward declarations
typedef struct MacroCallExpressionNode MacroCallExpressionNode;
typedef struct MacroDeclarationNode MacroDeclarationNode;

// Funkcja do ekspansji wywołania makra
ASTNode* expand_macro_call(MacroCallExpressionNode* macro_call, Environment* env);

// Funkcja do ewaluacji wywołania makra w interpreterze
Value evaluate_macro_call_expression(MacroCallExpressionNode* node, Environment* env);

// Funkcja do deklaracji makra w środowisku
void declare_macro(MacroDeclarationNode* macro_decl, Environment* env);

#endif // INTERPRETER_MACRO_H