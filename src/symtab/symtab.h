#ifndef SYMTAB_H
#define SYMTAB_H

typedef enum
{
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_CHAR,
    TYPE_BOOL,
} VarType;

typedef struct
{
    char name[64];
    VarType return_type;
    VarType *param_types;
    int param_count;
} FuncInfo;

void symtab_add_func(const char *name, VarType return_type, VarType *param_types, int param_count);
FuncInfo *symtab_lookup_func(const char *name);
void symtab_push_scope(void);
void symtab_pop_scope(void);
void symtab_add(const char *name, VarType type);
VarType symtab_lookup(const char *name);
const char *ctype_string(VarType type);
const char *ctype_spec_string(VarType type);

#endif