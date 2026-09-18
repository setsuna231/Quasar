#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>
#include "type/type.h"

typedef struct
{
    char name[64];
    Type *type;
} Symbol;

typedef struct
{
    char name[64];
    Type *return_type;
    Type **param_types;
    int param_count;
} FuncInfo;

void symtab_push_scope(void);
void symtab_pop_scope(void);
void symtab_add(const char *name, Type *type);
Type *symtab_lookup_type(const char *name);
bool symtab_has(const char *name);
Symbol *symtab_find(const char *name);

void symtab_add_func(const char *name, Type *return_type, Type **param_types, int param_count);
FuncInfo *symtab_lookup_func(const char *name);

#endif