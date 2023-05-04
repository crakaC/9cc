#include "9cc.h"

static Type* new_type(TypeKind ty, int size) {
    Type* ret = calloc(1, sizeof(Type));
    ret->ty = ty;
    ret->size = size;
    ret->align = size;
    return ret;
}

Type* ptr_to(Type* base) {
    Type* ty = new_type(PTR, 8);
    ty->ptr_to = base;
    return ty;
}

Type* int_type() {
    return new_type(INT, 4);
}

Type* void_type() {
    return new_type(VOID, 1);
}

bool same_type(Type* x, Type* y) {
    if (x->ty != y->ty) {
        return false;
    }

    switch (x->ty) {
    case PTR:
        return same_type(x->ptr_to, y->ptr_to);
    default:
        return true;
    }
}