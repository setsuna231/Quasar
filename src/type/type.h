#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

typedef enum
{
    TYPE_VOID,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_CHAR,
    TYPE_BOOL
} VarType;

typedef enum
{
    TYPE_PRIMITIVE,
    TYPE_ARRAY
} TypeKind;

typedef struct Type
{
    TypeKind kind;
    union
    {
        VarType primitive;
        struct
        {
            struct Type *element;
            int size;
        } array;
    } as;
} Type;

Type *type_primitive(VarType prim);
Type *type_array(Type *element, int size);

bool type_is_primitive(Type *t, VarType prim);
bool type_equal(Type *a, Type *b);
const char *type_to_string(Type *t);
const char *type_to_c_string(Type *t);

#endif