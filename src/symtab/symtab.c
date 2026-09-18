#include "symtab.h"
#include "error/error.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_VARS 256
#define MAX_SCOPES 64
#define MAX_FUNCS 256

typedef struct
{
    Symbol symbols[MAX_VARS];
    int count;
} Scope;

static Scope scopes[MAX_SCOPES];
static int scope_top = -1;

static FuncInfo func_table[MAX_FUNCS];
static int func_count = 0;

void symtab_push_scope(void)
{
    if (scope_top < MAX_SCOPES - 1)
    {
        scope_top++;
        scopes[scope_top].count = 0;
    }
    else
    {
        error_report("fatal: scope stack overflow\n");
        exit(1);
    }
}

void symtab_pop_scope(void)
{
    if (scope_top >= 0)
        scope_top--;
}

void symtab_add(const char *name, Type *type)
{
    if (scope_top < 0)
        symtab_push_scope();
    Scope *cur = &scopes[scope_top];
    if (cur->count >= MAX_VARS)
    {
        error_report("too many variables in this scope\n");
        return;
    }
    strncpy(cur->symbols[cur->count].name, name, 63);
    cur->symbols[cur->count].name[63] = '\0';
    cur->symbols[cur->count].type = type;
    cur->count++;
}

Symbol *symtab_find(const char *name)
{
    for (int s = scope_top; s >= 0; s--)
    {
        for (int i = 0; i < scopes[s].count; i++)
        {
            if (strcmp(scopes[s].symbols[i].name, name) == 0)
                return &scopes[s].symbols[i];
        }
    }
    return NULL;
}

Type *symtab_lookup_type(const char *name)
{
    Symbol *sym = symtab_find(name);
    return sym ? sym->type : NULL;
}

bool symtab_has(const char *name)
{
    return symtab_find(name) != NULL;
}

void symtab_add_func(const char *name, Type *return_type, Type **param_types, int param_count)
{
    if (func_count >= MAX_FUNCS)
    {
        error_report("too many functions\n");
        return;
    }
    FuncInfo *f = &func_table[func_count++];
    strncpy(f->name, name, 63);
    f->name[63] = '\0';
    f->return_type = return_type;
    f->param_count = param_count;
    if (param_count > 0)
    {
        f->param_types = malloc(sizeof(Type *) * param_count);
        for (int i = 0; i < param_count; i++)
            f->param_types[i] = param_types[i];
    }
    else
    {
        f->param_types = NULL;
    }
}

FuncInfo *symtab_lookup_func(const char *name)
{
    for (int i = 0; i < func_count; i++)
    {
        if (strcmp(func_table[i].name, name) == 0)
            return &func_table[i];
    }
    return NULL;
}