#include "codegen/codegen.h"
#include "symtab/symtab.h"
#include "error/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static void emit_statement(ASTNode *node, FILE *out, int indent);
static void emit_expression(ASTNode *node, FILE *out);
static void emit_runtime_helpers(FILE *out);
static void emit_function_definition(ASTNode *node, FILE *out);

// Returns the Quasar variable type of an expression node.
static VarType infer_type(ASTNode *node)
{
    if (!node)
        return TYPE_INT; // safe default
    switch (node->type)
    {
    case AST_INTEGER:
        return TYPE_INT;
    case AST_FLOAT:
        return TYPE_FLOAT;
    case AST_STRING:
        return TYPE_STRING;
    case AST_CHAR:
        return TYPE_CHAR;
    case AST_BOOL:
        return TYPE_BOOL;
    case AST_FUNC_CALL:
    {
        FuncInfo *fi = symtab_lookup_func(node->data.func_call.name);
        if (fi)
            return fi->return_type;
        return TYPE_INT; // fallback
    }
    case AST_VARIABLE:
        return node->varType;

    case AST_BINARY:
    {
        VarType left = infer_type(node->data.binary.left);
        VarType right = infer_type(node->data.binary.right);
        BinaryOp op = node->data.binary.op;
        if (op == OP_EQ || op == OP_NE || op == OP_LT || op == OP_GT || op == OP_LE || op == OP_GE || op == OP_AND || op == OP_OR)
        {
            return TYPE_BOOL;
        }

        if (op == OP_POW)
            return TYPE_FLOAT;

        if (op == OP_ADD && left == TYPE_STRING && right == TYPE_STRING)
            return TYPE_STRING;
        if (op == OP_MUL)
        {
            if ((left == TYPE_STRING && right == TYPE_INT) ||
                (left == TYPE_INT && right == TYPE_STRING))
                return TYPE_STRING;
        }
        // Arithmetic operators: if either operand is float, result is float; otherwise int.
        if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV ||
            op == OP_MOD || op == OP_FLDIV)
        {
            if (left == TYPE_FLOAT || right == TYPE_FLOAT)
                return TYPE_FLOAT;
            return TYPE_INT; // both ints -> int
        }

        if (op == OP_ASSIGN)
        {
            return infer_type(node->data.binary.left); // type of left side
        }

        // compound assignment yields the type of the left operand
        if (op == OP_ADD_ASSIGN || op == OP_SUB_ASSIGN || op == OP_MUL_ASSIGN ||
            op == OP_DIV_ASSIGN || op == OP_MOD_ASSIGN || op == OP_POW_ASSIGN ||
            op == OP_FLDIV_ASSIGN)
        {
            return infer_type(node->data.binary.left);
        }

        return TYPE_INT; // fallback
    }
    case AST_UNARY:
        if (node->data.unary.op == UNARY_NOT)
            return TYPE_BOOL;
        if (node->data.unary.op == UNARY_PRE_INC || node->data.unary.op == UNARY_PRE_DEC ||
            node->data.unary.op == UNARY_POST_INC || node->data.unary.op == UNARY_POST_DEC)
            return infer_type(node->data.unary.operand);
        return TYPE_INT;

    case AST_INPUT:
        return TYPE_STRING;

    case AST_TYPE_CONV:
        return node->data.typeconv.target;

    default:
        return TYPE_INT; // <- caused mental damage
    }
}

static const char *op_to_cstring(BinaryOp op)
{
    switch (op)
    {
    case OP_ADD:
        return "+";
    case OP_SUB:
        return "-";
    case OP_MUL:
        return "*";
    case OP_DIV:
        return "/";
    case OP_MOD:
        return "%";
    case OP_POW:
        return "pow"; // we'll emit pow()
    case OP_FLDIV:
        return "/"; // floor division in C is just / with ints; but we'll handle type later
    case OP_EQ:
        return "==";
    case OP_NE:
        return "!=";
    case OP_LT:
        return "<";
    case OP_GT:
        return ">";
    case OP_LE:
        return "<=";
    case OP_GE:
        return ">=";
    case OP_AND:
        return "&&";
    case OP_OR:
        return "||";
    case OP_ASSIGN:
        return "=";
    case OP_ADD_ASSIGN:
        return "+=";
    case OP_SUB_ASSIGN:
        return "-=";
    case OP_MUL_ASSIGN:
        return "*=";
    case OP_DIV_ASSIGN:
        return "/=";
    case OP_MOD_ASSIGN:
        return "%=";
    case OP_POW_ASSIGN:
        return "**=";
    case OP_FLDIV_ASSIGN:
        return "//=";
    default:
        return "???";
    }
}

static void indent(FILE *out, int level)
{
    for (int i = 0; i < level; i++)
    {
        fprintf(out, "  ");
    }
}

void generate_code(ASTNode *program, FILE *out)
{
    if (program->type != AST_PROGRAM)
    {
        error_report("root node must be AST_PROGRAM\n");
        exit(1);
    }

    /* Standard headers */
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <string.h>\n");
    fprintf(out, "#include <stdbool.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <math.h>\n\n");

    /* Runtime helpers */
    emit_runtime_helpers(out);

    /* Function prototypes */
    for (int i = 0; i < program->data.program.count; i++)
    {
        ASTNode *stmt = program->data.program.statements[i];
        if (stmt->type == AST_FUNC_DEF)
        {
            fprintf(out, "%s %s(", ctype_string(stmt->data.func_def.return_type),
                    stmt->data.func_def.name);
            for (int j = 0; j < stmt->data.func_def.param_count; j++)
            {
                if (j > 0)
                    fprintf(out, ", ");
                fprintf(out, "%s %s", ctype_string(stmt->data.func_def.params[j].type),
                        stmt->data.func_def.params[j].name);
            }
            fprintf(out, ");\n");
        }
    }
    fprintf(out, "\n");

    /* Function definitions */
    for (int i = 0; i < program->data.program.count; i++)
    {
        ASTNode *stmt = program->data.program.statements[i];
        if (stmt->type == AST_FUNC_DEF)
        {
            emit_function_definition(stmt, out);
            fprintf(out, "\n");
        }
    }

    /* Main function */
    fprintf(out, "int main(void) {\n");

    // Emit top-level statements (skip function definitions)
    for (int i = 0; i < program->data.program.count; i++)
    {
        ASTNode *stmt = program->data.program.statements[i];
        if (stmt->type != AST_FUNC_DEF)
        {
            emit_statement(stmt, out, 1);
        }
    }

    fprintf(out, "\treturn 0;\n");
    fprintf(out, "}\n");
}
static void emit_function_definition(ASTNode *node, FILE *out)
{
    if (node->type != AST_FUNC_DEF)
        return;

    fprintf(out, "%s %s(", ctype_string(node->data.func_def.return_type),
            node->data.func_def.name);

    for (int i = 0; i < node->data.func_def.param_count; i++)
    {
        if (i > 0)
            fprintf(out, ", ");
        fprintf(out, "%s %s", ctype_string(node->data.func_def.params[i].type),
                node->data.func_def.params[i].name);
    }
    fprintf(out, ") {\n");
    emit_statement(node->data.func_def.body, out, 1); // body is AST_BLOCK
    fprintf(out, "}\n");
}

static void emit_expression_maybe_bool(ASTNode *node, FILE *out) // boolean helper
{
    if (infer_type(node) == TYPE_BOOL)
    {
        fprintf(out, "((");
        emit_expression(node, out);
        fprintf(out, ") ? \"true\" : \"false\")");
    }
    else
    {
        emit_expression(node, out);
    }
}

static void emit_statement(ASTNode *node, FILE *out, int indent_level)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case AST_PRINT:
    {
        indent(out, indent_level);
        if (node->data.print.count == 0)
        {
            fprintf(out, "printf(\"\\n\");\n");
            break;
        }

        // Emit pre‑computed format string
        fprintf(out, "printf(\"");
        fputs(node->data.print.format, out);
        fprintf(out, "\"");

        for (int i = 0; i < node->data.print.count; i++)
        {
            fprintf(out, ", ");
            emit_expression_maybe_bool(node->data.print.expressions[i], out);
        }
        fprintf(out, ");\n");
        break;
    }

    case AST_LET:
        indent(out, indent_level);
        fprintf(out, "%s %s", ctype_string(node->data.let.vartype),
                node->data.let.name);
        if (node->data.let.init)
        {
            fprintf(out, " = ");
            emit_expression(node->data.let.init, out);
        }
        fprintf(out, ";\n");
        break;
        break;

    case AST_ASSIGN:
        indent(out, indent_level);
        fprintf(out, "%s = ", node->data.assign.name);
        emit_expression(node->data.assign.value, out);
        fprintf(out, ";\n");
        break;

    case AST_BLOCK:
        indent(out, indent_level);
        fprintf(out, "{\n");
        for (int i = 0; i < node->data.block.count; i++)
        {
            emit_statement(node->data.block.statements[i], out, indent_level + 1);
        }
        indent(out, indent_level);
        fprintf(out, "}\n");
        break; // <-- gave me mental torture

    case AST_IF:
    {
        ASTNode *current = node;
        bool first = true;
        while (current)
        {
            if (current->data.ifelse.condition)
            {
                indent(out, indent_level);
                fprintf(out, "%s (", first ? "if" : "else if");
                emit_expression(current->data.ifelse.condition, out);
                fprintf(out, ")\n");
                emit_statement(current->data.ifelse.body, out, indent_level);
            }
            else
            {
                indent(out, indent_level);
                fprintf(out, "else\n");
                emit_statement(current->data.ifelse.body, out, indent_level);
            }
            first = false;
            current = current->data.ifelse.next;
        }
        break;
    }

    case AST_WHILE:
    {
        indent(out, indent_level);
        fprintf(out, "while (");
        emit_expression(node->data.whileloop.condition, out);
        fprintf(out, ")\n");
        emit_statement(node->data.whileloop.body, out, indent_level);
        break;
    }

    case AST_REPEAT_UNTIL:
    {
        indent(out, indent_level);
        fprintf(out, "do\n");
        emit_statement(node->data.repeatuntil.body, out, indent_level);
        indent(out, indent_level);
        fprintf(out, "while (!(");
        emit_expression(node->data.repeatuntil.condition, out);
        fprintf(out, "));\n");
        break;
    }

    case AST_FOR:
    {
        fprintf(out, "for (");
        // init
        if (node->data.forloop.init)
        {
            if (node->data.forloop.init->type == AST_LET)
            {
                ASTNode *let_node = node->data.forloop.init;
                fprintf(out, "%s %s = ", ctype_string(let_node->data.let.vartype), let_node->data.let.name);
                emit_expression(let_node->data.let.init, out);
            }
            else
            {
                emit_expression(node->data.forloop.init, out);
            }
        }
        fprintf(out, "; ");
        // condition
        if (node->data.forloop.condition)
        {
            emit_expression(node->data.forloop.condition, out);
        }
        else
        {
            fprintf(out, "1");
        }
        fprintf(out, "; ");
        // update
        if (node->data.forloop.update)
        {
            emit_expression(node->data.forloop.update, out);
        }
        fprintf(out, ")\n");
        emit_statement(node->data.forloop.body, out, indent_level);
        break;
    }

    case AST_BREAK:
        indent(out, indent_level);
        fprintf(out, "break;\n");
        break;

    case AST_CONTINUE:
        indent(out, indent_level);
        fprintf(out, "continue;\n");
        break;

    case AST_MATCH:
    {
        indent(out, indent_level);
        fprintf(out, "switch (");
        emit_expression(node->data.match.discriminant, out);
        fprintf(out, ") {\n");

        for (int i = 0; i < node->data.match.case_count; i++)
        {
            ASTNode *value = node->data.match.cases[i].value;
            ASTNode *body = node->data.match.cases[i].body;

            if (i > 0)
            {
                indent(out, indent_level);
                fprintf(out, "/* fall through */\n");
            }

            if (value)
            {
                indent(out, indent_level);
                fprintf(out, "case ");
                // emit the value (should be a literal)
                emit_expression(value, out);
                fprintf(out, ":\n");
            }
            else
            {
                indent(out, indent_level);
                fprintf(out, "default:\n");
            }
            // emit the body statements with increased indent
            // body is an AST_BLOCK, which contains the statements; we can just call emit_statement on the block, which will handle the indentation.
            emit_statement(body, out, indent_level + 1);
        }

        indent(out, indent_level);
        fprintf(out, "}\n");
        break;
    }

    case AST_MULTI_LET:
    {
        for (int i = 0; i < node->data.multilet.count; i++)
        {
            // emit each declaration as a statement (they are AST_LET nodes)
            emit_statement(node->data.multilet.declarations[i], out, indent_level);
        }
        break;
    }

    case AST_EXPR_STATEMENT:
        indent(out, indent_level);
        emit_expression(node->data.expr, out);
        fprintf(out, ";\n");
        break;

    case AST_RETURN:
        indent(out, indent_level);
        if (node->data.return_expr)
        {
            fprintf(out, "return ");
            emit_expression(node->data.return_expr, out);
            fprintf(out, ";\n");
        }
        else
        {
            fprintf(out, "return;\n");
        }
        break;

    case AST_FALLTHROUGH:
        indent(out, indent_level);
        fprintf(out, "/* fall through */\n");
        break;

    default:
        fprintf(stderr, "Error : unknown statement type %d\n", node->type);
        exit(1);
    }
}

static void emit_expression(ASTNode *node, FILE *out)
{
    switch (node->type)
    {
    case AST_INTEGER:
        fprintf(out, "%d", node->data.intValue);
        break;
    case AST_FLOAT:
        fprintf(out, "%f", node->data.floatValue);
        break;
    case AST_STRING:
    {
        fputc('"', out);
        for (char *p = node->data.strValue; *p; p++)
        {
            switch (*p)
            {
            case '\n':
                fputs("\\n", out);
                break;
            case '\t':
                fputs("\\t", out);
                break;
            case '\r':
                fputs("\\r", out);
                break;
            case '\\':
                fputs("\\\\", out);
                break;
            case '"':
                fputs("\\\"", out);
                break;
            case '\0':
                fputs("\\0", out);
                break; // null character
            case '\a':
                fputs("\\a", out);
                break; // alert (bell)
            case '\b':
                fputs("\\b", out);
                break; // backspace
            case '\f':
                fputs("\\f", out);
                break; // form feed
            case '\v':
                fputs("\\v", out);
                break; // vertical tab
            default:
                fputc(*p, out);
                break;
            }
        }
        fputc('"', out);
        break;
    }
    case AST_CHAR:
    {
        // Emit as a C char literal, escaping if needed
        fputc('\'', out);
        char c = node->data.charValue;
        switch (c)
        {
        case '\n':
            fputs("\\n", out);
            break;
        case '\t':
            fputs("\\t", out);
            break;
        case '\r':
            fputs("\\r", out);
            break;
        case '\\':
            fputs("\\\\", out);
            break;
        case '\'':
            fputs("\\'", out);
            break;
        case '\0':
            fputs("\\0", out);
            break;
        case '\a':
            fputs("\\a", out);
            break;
        case '\b':
            fputs("\\b", out);
            break;
        case '\f':
            fputs("\\f", out);
            break;
        case '\v':
            fputs("\\v", out);
            break;
        default:
            fputc(c, out);
            break;
        }
        fputc('\'', out);
        break;
    }
    case AST_BOOL:
        fprintf(out, "%d", node->data.boolValue);
        break;
    case AST_VARIABLE:
        fprintf(out, "%s", node->data.varName);
        break;
    case AST_BINARY:
    {
        BinaryOp op = node->data.binary.op;
        ASTNode *left = node->data.binary.left;
        ASTNode *right = node->data.binary.right;
        VarType ltype = infer_type(left);
        VarType rtype = infer_type(right);

        if ((op == OP_EQ || op == OP_NE) && ltype == TYPE_STRING && rtype == TYPE_STRING)
        {
            fprintf(out, "(strcmp(");
            emit_expression(left, out);
            fprintf(out, ", ");
            emit_expression(right, out);
            fprintf(out, ") %s 0)", op == OP_EQ ? "==" : "!=");
            break;
        }

        /* String concatenation */
        if (op == OP_ADD && ltype == TYPE_STRING && rtype == TYPE_STRING)
        {
            fprintf(out, "quasar_strcat(");
            emit_expression(left, out);
            fprintf(out, ", ");
            emit_expression(right, out);
            fprintf(out, ")");
            break;
        }
        /* String repetition */
        if (op == OP_MUL)
        {
            if (ltype == TYPE_STRING && rtype == TYPE_INT)
            {
                fprintf(out, "quasar_strrep(");
                emit_expression(left, out);
                fprintf(out, ", ");
                emit_expression(right, out);
                fprintf(out, ")");
                break;
            }
            if (ltype == TYPE_INT && rtype == TYPE_STRING)
            {
                fprintf(out, "quasar_strrep(");
                emit_expression(right, out);
                fprintf(out, ", ");
                emit_expression(left, out);
                fprintf(out, ")");
                break;
            }
        }
        // Special case for exponent: use pow() function
        if (op == OP_POW)
        {
            fprintf(out, "pow(");
            emit_expression(node->data.binary.left, out);
            fprintf(out, ", ");
            emit_expression(node->data.binary.right, out);
            fprintf(out, ")");
        }
        else
        {
            fprintf(out, "(");
            emit_expression(node->data.binary.left, out);
            fprintf(out, " %s ", op_to_cstring(node->data.binary.op));
            emit_expression(node->data.binary.right, out);
            fprintf(out, ")");
        }
        break;
    }
    case AST_UNARY:
        if (node->data.unary.op == UNARY_NOT)
        {
            fprintf(out, "!(");
            emit_expression(node->data.unary.operand, out);
            fprintf(out, ")");
        }
        else if (node->data.unary.op == UNARY_MINUS)
        {
            fprintf(out, "-(");
            emit_expression(node->data.unary.operand, out);
            fprintf(out, ")");
        }
        else if (node->data.unary.op == UNARY_PLUS)
        {
            emit_expression(node->data.unary.operand, out);
        }
        else if (node->data.unary.op == UNARY_PRE_INC)
        {
            fprintf(out, "++(");
            emit_expression(node->data.unary.operand, out);
            fprintf(out, ")");
        }
        else if (node->data.unary.op == UNARY_PRE_DEC)
        {
            fprintf(out, "--(");
            emit_expression(node->data.unary.operand, out);
            fprintf(out, ")");
        }
        else if (node->data.unary.op == UNARY_POST_INC)
        {
            fprintf(out, "(");
            emit_expression(node->data.unary.operand, out);
            fprintf(out, ")++");
        }
        else if (node->data.unary.op == UNARY_POST_DEC)
        {
            fprintf(out, "(");
            emit_expression(node->data.unary.operand, out);
            fprintf(out, ")--");
        }
        break;
    case AST_INPUT:
        if (node->data.prompt)
        {
            fprintf(out, "quasar_input(");
            emit_expression(node->data.prompt, out);
            fprintf(out, ")");
        }
        else
        {
            fprintf(out, "quasar_input(NULL)");
        }
        break;
    case AST_TYPE_CONV:
    {
        VarType target = node->data.typeconv.target;
        ASTNode *arg = node->data.typeconv.source;
        VarType src_type = infer_type(arg);

        switch (target)
        {
        case TYPE_INT:
            if (src_type == TYPE_STRING)
            {
                fprintf(out, "atoi(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_FLOAT)
            {
                fprintf(out, "(int)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_CHAR)
            {
                fprintf(out, "(int)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_BOOL)
            {
                fprintf(out, "(int)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else
            {
                emit_expression(arg, out); // already int
            }
            break;

        case TYPE_FLOAT:
            if (src_type == TYPE_STRING)
            {
                fprintf(out, "atof(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_INT)
            {
                fprintf(out, "(double)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_CHAR)
            {
                fprintf(out, "(double)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_BOOL)
            {
                fprintf(out, "(double)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else
            {
                emit_expression(arg, out); // already float/double
            }
            break;

        case TYPE_STRING:
            if (src_type == TYPE_STRING)
            {
                emit_expression(arg, out);
            }
            else
            {
                fprintf(out, "quasar_to_string(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            break;

        case TYPE_CHAR:
            if (src_type == TYPE_INT)
            {
                fprintf(out, "(char)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            else if (src_type == TYPE_STRING)
            {
                fprintf(out, "(");
                emit_expression(arg, out);
                fprintf(out, ")[0]");
            }
            else
            {
                fprintf(out, "(char)(");
                emit_expression(arg, out);
                fprintf(out, ")");
            }
            break;

        case TYPE_BOOL:
            if (src_type == TYPE_BOOL)
            {
                emit_expression(arg, out);
            }
            else if (src_type == TYPE_STRING)
            {
                fprintf(out, "((strcmp(");
                emit_expression(arg, out);
                fprintf(out, ", \"true\") == 0) || (atoi(");
                emit_expression(arg, out);
                fprintf(out, ") != 0))");
            }
            else
            {
                fprintf(out, "(");
                emit_expression(arg, out);
                fprintf(out, " != 0)");
            }
            break;

        default:
            break;
        }
        break;
    }
    case AST_FUNC_CALL:
    {
        fprintf(out, "%s(", node->data.func_call.name);
        for (int i = 0; i < node->data.func_call.arg_count; i++)
        {
            if (i > 0)
                fprintf(out, ", ");
            emit_expression(node->data.func_call.args[i], out);
        }
        fprintf(out, ")");
        break;
    }

    default:
        break;
    }
}

static void emit_runtime_helpers(FILE *out)
{
    fprintf(out, "/* Quasar runtime helpers */\n");

    /* input() */
    fprintf(out, "char *quasar_input(const char *prompt) {\n");
    fprintf(out, "    if (prompt) printf(\"%%s\", prompt);\n");
    fprintf(out, "    char buffer[1024];\n");
    fprintf(out, "    if (fgets(buffer, sizeof(buffer), stdin)) {\n");
    fprintf(out, "        size_t len = strlen(buffer);\n");
    fprintf(out, "        if (len > 0 && buffer[len-1] == '\\n') buffer[len-1] = '\\0';\n");
    fprintf(out, "        return strdup(buffer);\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return strdup(\"\");\n");
    fprintf(out, "}\n\n");

    /* to_string() */
    fprintf(out, "char *quasar_to_string(double x) {\n");
    fprintf(out, "    char buffer[128];\n");
    fprintf(out, "    snprintf(buffer, sizeof(buffer), \"%%g\", x);\n");
    fprintf(out, "    return strdup(buffer);\n");
    fprintf(out, "}\n\n");

    /* String concatenation */
    fprintf(out, "char *quasar_strcat(const char *a, const char *b) {\n");
    fprintf(out, "    size_t len = strlen(a) + strlen(b) + 1;\n");
    fprintf(out, "    char *result = (char*)malloc(len);\n");
    fprintf(out, "    if (result) {\n");
    fprintf(out, "        strcpy(result, a);\n");
    fprintf(out, "        strcat(result, b);\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return result;\n");
    fprintf(out, "}\n\n");

    /* String repetition */
    fprintf(out, "char *quasar_strrep(const char *s, int n) {\n");
    fprintf(out, "    if (n <= 0) return strdup(\"\");\n");
    fprintf(out, "    size_t len = strlen(s) * n + 1;\n");
    fprintf(out, "    char *result = (char*)malloc(len);\n");
    fprintf(out, "    if (result) {\n");
    fprintf(out, "        result[0] = '\\0';\n");
    fprintf(out, "        for (int i = 0; i < n; i++) strcat(result, s);\n");
    fprintf(out, "    }\n");
    fprintf(out, "    return result;\n");
    fprintf(out, "}\n\n");
}
