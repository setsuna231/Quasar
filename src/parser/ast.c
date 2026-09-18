#define _POSIX_C_SOURCE 200809L
#include "ast.h"
#include <stdlib.h>
#include <string.h>

// For int
ASTNode *make_integer(int value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_INTEGER;
    node->data.intValue = value;
    return node;
}

// For Float
ASTNode *make_float(double value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_FLOAT;
    node->data.floatValue = value;
    return node;
}

// For print stmt
ASTNode *make_print_empty(void)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_PRINT;
    node->data.print.count = 0;
    node->data.print.capacity = 4; // start small
    node->data.print.expressions = malloc(sizeof(ASTNode *) * node->data.print.capacity);
    node->data.print.format = NULL;
    if (!node->data.print.expressions)
    {
        free(node);
        return NULL;
    }
    return node;
}

void print_add_expression(ASTNode *print_node, ASTNode *expr)
{
    if (print_node->data.print.count >= print_node->data.print.capacity)
    {
        print_node->data.print.capacity *= 2;
        ASTNode **newbuf = realloc(print_node->data.print.expressions,
                                   sizeof(ASTNode *) * print_node->data.print.capacity);
        if (!newbuf)
            return; // memory error, but we'll ignore for now
        print_node->data.print.expressions = newbuf;
    }
    print_node->data.print.expressions[print_node->data.print.count++] = expr;
}

// For string
ASTNode *make_string(const char *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_STRING;
    node->data.strValue = strdup(value);
    return node;
}

// For single chars
ASTNode *make_char(char value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_CHAR;
    node->data.charValue = value;
    return node;
}

// For bool (t/f)
ASTNode *make_bool(int value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_BOOL;
    node->data.boolValue = value;
    return node;
}

// For .qs
ASTNode *make_program(void)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_PROGRAM;
    node->data.program.capacity = 8;
    node->data.program.count = 0;
    node->data.program.statements = malloc(sizeof(ASTNode *) * node->data.program.capacity);
    if (!node->data.program.statements)
    {
        free(node);
        return NULL;
    }
    return node;
}

void program_add_statement(ASTNode *program, ASTNode *stmt)
{
    if (program->data.program.count >= program->data.program.capacity)
    {
        program->data.program.capacity *= 2;
        ASTNode **newbuf = realloc(program->data.program.statements,
                                   sizeof(ASTNode *) * program->data.program.capacity);
        if (!newbuf)
            return; // memory error; ignore for now
        program->data.program.statements = newbuf;
    }
    program->data.program.statements[program->data.program.count++] = stmt;
}

// for let and variable
ASTNode *make_let(const char *name, Type *type, ASTNode *init)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_LET;
    node->data.let.name = strdup(name);
    node->data.let.vartype = type;
    node->data.let.init = init;
    return node;
}

ASTNode *make_multilet(void)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_MULTI_LET;
    node->data.multilet.capacity = 4;
    node->data.multilet.count = 0;
    node->data.multilet.declarations = (struct ASTNode **)malloc(
        sizeof(struct ASTNode *) * node->data.multilet.capacity);
    if (!node->data.multilet.declarations)
    {
        free(node);
        return NULL;
    }
    return node;
}

void multilet_add(ASTNode *multilet, ASTNode *decl)
{
    if (multilet->data.multilet.count >= multilet->data.multilet.capacity)
    {
        multilet->data.multilet.capacity *= 2;
        multilet->data.multilet.declarations = (struct ASTNode **)realloc(
            multilet->data.multilet.declarations,
            sizeof(struct ASTNode *) * multilet->data.multilet.capacity);
        if (!multilet->data.multilet.declarations)
            return; // memory error – ignore for now
    }
    multilet->data.multilet.declarations[multilet->data.multilet.count++] = decl;
}

ASTNode *make_variable(const char *name, Type *type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_VARIABLE;
    node->data.varName = strdup(name);
    node->varType = type;
    return node;
}
// for reassignment of var
ASTNode *make_assign(const char *name, ASTNode *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_ASSIGN;
    node->data.assign.name = strdup(name);
    node->data.assign.value = value;
    return node;
}

// for binary ops
ASTNode *make_binary(BinaryOp op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_BINARY;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

// for {...}
ASTNode *make_block(void)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_BLOCK;
    node->data.block.statements = malloc(sizeof(ASTNode *) * 4);
    if (!node->data.block.statements)
    {
        free(node);
        return NULL;
    }
    node->data.block.capacity = 4;
    node->data.block.count = 0;
    return node;
}

void block_add_statement(ASTNode *block, ASTNode *stmt)
{
    if (block->data.block.count >= block->data.block.capacity)
    {
        block->data.block.capacity *= 2;
        ASTNode **newbuf = realloc(block->data.block.statements,
                                   sizeof(ASTNode *) * block->data.block.capacity);
        if (!newbuf)
            return; // memory error – we can improve later
        block->data.block.statements = newbuf;
    }
    block->data.block.statements[block->data.block.count++] = stmt;
}

// for unary ops - !, &, |
ASTNode *make_unary(UnaryOp op, ASTNode *operand)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_UNARY;
    node->data.unary.op = op;
    node->data.unary.operand = operand;
    return node;
}

// for if...elif...else
ASTNode *make_if(ASTNode *condition, ASTNode *body, ASTNode *next)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_IF;
    node->data.ifelse.condition = condition;
    node->data.ifelse.body = body;
    node->data.ifelse.next = next;
    return node;
}

// for while and repeat...until
ASTNode *make_while(ASTNode *condition, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_WHILE;
    node->data.whileloop.condition = condition;
    node->data.whileloop.body = body;
    return node;
}

ASTNode *make_repeat_until(ASTNode *condition, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_REPEAT_UNTIL;
    node->data.repeatuntil.condition = condition;
    node->data.repeatuntil.body = body;
    return node;
}

// for for loop
ASTNode *make_for(ASTNode *init, ASTNode *condition, ASTNode *update, ASTNode *body)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_FOR;
    node->data.forloop.init = init;
    node->data.forloop.condition = condition;
    node->data.forloop.update = update;
    node->data.forloop.body = body;
    return node;
}
ASTNode *make_expr_statement(ASTNode *expr)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_EXPR_STATEMENT;
    node->data.expr = expr;
    return node;
}

// for break and continue
ASTNode *make_break(void)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_BREAK;
    return node;
}

ASTNode *make_continue(void)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_CONTINUE;
    return node;
}

// for match...case...default
ASTNode *make_match(ASTNode *discriminant)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_MATCH;
    node->data.match.discriminant = discriminant;
    node->data.match.case_capacity = 4;
    node->data.match.cases = (MatchCase *)malloc(sizeof(MatchCase) * node->data.match.case_capacity);
    if (!node->data.match.cases)
    {
        free(node);
        return NULL;
    }
    node->data.match.case_count = 0;
    return node;
}

void match_add_case(ASTNode *match, ASTNode *value, ASTNode *body)
{
    if (match->data.match.case_count >= match->data.match.case_capacity)
    {
        match->data.match.case_capacity *= 2;
        match->data.match.cases = (MatchCase *)realloc(
            match->data.match.cases,
            sizeof(MatchCase) * match->data.match.case_capacity);
        if (!match->data.match.cases)
            return; // memory error – ignore for now
    }
    match->data.match.cases[match->data.match.case_count].value = value;
    match->data.match.cases[match->data.match.case_count].body = body;
    match->data.match.case_count++;
}

// for input
ASTNode *make_input(ASTNode *prompt)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_INPUT;
    node->data.prompt = prompt;
    return node;
}

// for type conversions
ASTNode *make_type_conv(Type *target, ASTNode *source)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_TYPE_CONV;
    node->data.typeconv.target = target;
    node->data.typeconv.source = source;
    return node;
}

// for return
ASTNode *make_return(ASTNode *expr)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_RETURN;
    node->data.return_expr = expr;
    return node;
}

// for fallthrough
ASTNode *make_fallthrough(void)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_FALLTHROUGH;
    return node;
}

// for functions
ASTNode *make_func_def(const char *name, Type *return_type, ASTNode *body)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_FUNC_DEF;
    node->data.func_def.name = strdup(name);
    node->data.func_def.return_type = return_type;
    node->data.func_def.body = body;
    node->data.func_def.param_capacity = 2;
    node->data.func_def.param_count = 0;
    node->data.func_def.params = malloc(sizeof(*node->data.func_def.params) *
                                        node->data.func_def.param_capacity);
    if (!node->data.func_def.params)
    {
        free(node->data.func_def.name);
        free(node);
        return NULL;
    }
    return node;
}

void func_def_add_param(ASTNode *func_def, const char *param_name, Type *param_type)
{
    if (!func_def || func_def->type != AST_FUNC_DEF)
        return;

    if (func_def->data.func_def.param_count >= func_def->data.func_def.param_capacity)
    {
        func_def->data.func_def.param_capacity *= 2;
        func_def->data.func_def.params = realloc(func_def->data.func_def.params,
                                                 sizeof(*func_def->data.func_def.params) * func_def->data.func_def.param_capacity);
        if (!func_def->data.func_def.params)
            return;
    }

    int i = func_def->data.func_def.param_count++;
    func_def->data.func_def.params[i].name = strdup(param_name);
    func_def->data.func_def.params[i].type = param_type;
}

ASTNode *make_func_call(const char *name)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_FUNC_CALL;
    node->data.func_call.name = strdup(name);
    node->data.func_call.arg_capacity = 4;
    node->data.func_call.arg_count = 0;
    node->data.func_call.args = malloc(sizeof(ASTNode *) * node->data.func_call.arg_capacity);
    if (!node->data.func_call.args)
    {
        free(node->data.func_call.name);
        free(node);
        return NULL;
    }
    return node;
}

void func_call_add_arg(ASTNode *func_call, ASTNode *arg)
{
    if (!func_call || func_call->type != AST_FUNC_CALL)
        return;

    if (func_call->data.func_call.arg_count >= func_call->data.func_call.arg_capacity)
    {
        func_call->data.func_call.arg_capacity *= 2;
        func_call->data.func_call.args = realloc(func_call->data.func_call.args,
                                                 sizeof(ASTNode *) * func_call->data.func_call.arg_capacity);
        if (!func_call->data.func_call.args)
            return;
    }
    func_call->data.func_call.args[func_call->data.func_call.arg_count++] = arg;
}

// for arrays : making and accessing
ASTNode *make_array_literal(void)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_ARRAY_LITERAL;
    node->data.array_literal.capacity = 4;
    node->data.array_literal.count = 0;
    node->data.array_literal.elements = malloc(sizeof(ASTNode *) * node->data.array_literal.capacity);
    if (!node->data.array_literal.elements)
    {
        free(node);
        return NULL;
    }
    return node;
}

void array_literal_add(ASTNode *literal, ASTNode *element)
{
    if (literal->data.array_literal.count >= literal->data.array_literal.capacity)
    {
        literal->data.array_literal.capacity *= 2;
        literal->data.array_literal.elements = realloc(literal->data.array_literal.elements,
                                                       sizeof(ASTNode *) * literal->data.array_literal.capacity);
        if (!literal->data.array_literal.elements)
            return;
    }
    literal->data.array_literal.elements[literal->data.array_literal.count++] = element;
}

ASTNode *make_array_access(ASTNode *array, ASTNode *index, Type *element_type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node)
        return NULL;
    node->type = AST_ARRAY_ACCESS;
    node->data.array_access.array = array;
    node->data.array_access.index = index;
    node->data.array_access.element_type = element_type;
    return node;
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;
    switch (node->type)
    {
    case AST_PRINT:
        free(node->data.print.format);
        for (int i = 0; i < node->data.print.count; i++)
            free_ast(node->data.print.expressions[i]);
        free(node->data.print.expressions);
        break;
    case AST_STRING:
        free(node->data.strValue);
        break;
    case AST_PROGRAM:
        for (int i = 0; i < node->data.program.count; i++)
            free_ast(node->data.program.statements[i]);
        free(node->data.program.statements);
        break;
    case AST_INTEGER:
    case AST_FLOAT:
        // nothing to free
        break;
    case AST_LET:
        free(node->data.let.name);
        free_ast(node->data.let.init);
        break;
    case AST_VARIABLE:
        free(node->data.varName);
        break;
    case AST_ASSIGN:
        free(node->data.assign.name);
        free_ast(node->data.assign.value);
        break;
    case AST_BINARY:
        free_ast(node->data.binary.left);
        free_ast(node->data.binary.right);
        break;
    case AST_BLOCK:
        for (int i = 0; i < node->data.block.count; i++)
            free_ast(node->data.block.statements[i]);
        free(node->data.block.statements);
        break;
    case AST_UNARY:
        free_ast(node->data.unary.operand);
        break;
    case AST_IF:
        free_ast(node->data.ifelse.condition);
        free_ast(node->data.ifelse.body);
        free_ast(node->data.ifelse.next);
        break;
    case AST_WHILE:
        free_ast(node->data.whileloop.condition);
        free_ast(node->data.whileloop.body);
        break;
    case AST_REPEAT_UNTIL:
        free_ast(node->data.repeatuntil.condition);
        free_ast(node->data.repeatuntil.body);
        break;
    case AST_FOR:
        if (node->data.forloop.init)
            free_ast(node->data.forloop.init);
        if (node->data.forloop.condition)
            free_ast(node->data.forloop.condition);
        if (node->data.forloop.update)
            free_ast(node->data.forloop.update);
        if (node->data.forloop.body)
            free_ast(node->data.forloop.body);
        break;

    case AST_EXPR_STATEMENT:
        if (node->data.expr)
            free_ast(node->data.expr);
        break;

    case AST_MATCH:
        free_ast(node->data.match.discriminant);
        for (int i = 0; i < node->data.match.case_count; i++)
        {
            free_ast(node->data.match.cases[i].value); // could be NULL
            free_ast(node->data.match.cases[i].body);
        }
        free(node->data.match.cases);
        break;

    case AST_MULTI_LET:
        for (int i = 0; i < node->data.multilet.count; i++)
        {
            free_ast(node->data.multilet.declarations[i]);
        }
        free(node->data.multilet.declarations);
        break;
    case AST_INPUT:
        free_ast(node->data.prompt);
        break;
    case AST_TYPE_CONV:
        free_ast(node->data.typeconv.source);
        break;
    case AST_RETURN:
        free_ast(node->data.return_expr);
        break;
    case AST_FUNC_DEF:
        free(node->data.func_def.name);
        for (int i = 0; i < node->data.func_def.param_count; i++)
        {
            free(node->data.func_def.params[i].name);
        }
        free(node->data.func_def.params);
        free_ast(node->data.func_def.body);
        break;

    case AST_FUNC_CALL:
        free(node->data.func_call.name);
        for (int i = 0; i < node->data.func_call.arg_count; i++)
            free_ast(node->data.func_call.args[i]);
        free(node->data.func_call.args);
        break;

    case AST_ARRAY_LITERAL:
        for (int i = 0; i < node->data.array_literal.count; i++)
            free_ast(node->data.array_literal.elements[i]);
        free(node->data.array_literal.elements);
        break;

    case AST_ARRAY_ACCESS:
        free_ast(node->data.array_access.array);
        free_ast(node->data.array_access.index);
        break;
    default:
        break;
    }
    free(node);
}