#include "type/type.h"
#include <stdlib.h>
#include <stdio.h>

static Type prim_int = {.kind = TYPE_PRIMITIVE, .as.primitive = TYPE_INT};
static Type prim_float = {.kind = TYPE_PRIMITIVE, .as.primitive = TYPE_FLOAT};
static Type prim_string = {.kind = TYPE_PRIMITIVE, .as.primitive = TYPE_STRING};
static Type prim_char = {.kind = TYPE_PRIMITIVE, .as.primitive = TYPE_CHAR};
static Type prim_bool = {.kind = TYPE_PRIMITIVE, .as.primitive = TYPE_BOOL};
static Type prim_void = {.kind = TYPE_PRIMITIVE, .as.primitive = TYPE_VOID};

Type *type_primitive(VarType prim)
{
    switch (prim)
    {
    case TYPE_INT:
        return &prim_int;
    case TYPE_FLOAT:
        return &prim_float;
    case TYPE_STRING:
        return &prim_string;
    case TYPE_CHAR:
        return &prim_char;
    case TYPE_BOOL:
        return &prim_bool;
    case TYPE_VOID:
        return &prim_void;
    default:
        return NULL;
    }
}

Type *type_array(Type *element, int size)
{
    Type *t = malloc(sizeof(Type));
    if (!t)
        return NULL;
    t->kind = TYPE_ARRAY;
    t->as.array.element = element;
    t->as.array.size = size;
    return t;
}

bool type_is_primitive(Type *t, VarType prim)
{
    return t && t->kind == TYPE_PRIMITIVE && t->as.primitive == prim;
}

bool type_equal(Type *a, Type *b)
{
    if (!a || !b)
        return false;
    if (a->kind != b->kind)
        return false;
    if (a->kind == TYPE_PRIMITIVE)
        return a->as.primitive == b->as.primitive;
    if (a->kind == TYPE_ARRAY)
    {
        return a->as.array.size == b->as.array.size &&
               type_equal(a->as.array.element, b->as.array.element);
    }
    return false;
}

const char *type_to_string(Type *t)
{
    if (!t)
        return "unknown";
    if (t->kind == TYPE_PRIMITIVE)
    {
        switch (t->as.primitive)
        {
        case TYPE_INT:
            return "int";
        case TYPE_FLOAT:
            return "float";
        case TYPE_STRING:
            return "string";
        case TYPE_CHAR:
            return "char";
        case TYPE_BOOL:
            return "bool";
        case TYPE_VOID:
            return "void";
        default:
            return "unknown";
        }
    }
    if (t->kind == TYPE_ARRAY)
    {
        static char buf[64];
        snprintf(buf, sizeof(buf), "%s[%d]",
                 type_to_string(t->as.array.element),
                 t->as.array.size);
        return buf;
    }
    return "unknown";
}

const char *type_to_c_string(Type *t)
{
    if (!t)
        return "void*";
    if (t->kind == TYPE_PRIMITIVE)
    {
        switch (t->as.primitive)
        {
        case TYPE_INT:
            return "int";
        case TYPE_FLOAT:
            return "double";
        case TYPE_STRING:
            return "char *";
        case TYPE_CHAR:
            return "char";
        case TYPE_BOOL:
            return "bool";
        case TYPE_VOID:
            return "void";
        default:
            return "void*";
        }
    }
    if (t->kind == TYPE_ARRAY)
    {
        // array declarations handled separately in codegen
        return type_to_c_string(t->as.array.element);
    }
    return "void*";
}