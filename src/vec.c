#include "9cc.h"

Vec* new_vec(void) {
    int initial_capacity = 16;
    Vec* v = malloc(sizeof(Vec));
    v->capacity = initial_capacity;
    v->data = malloc(sizeof(void*) * initial_capacity);
    v->size = 0;
    return v;
}

void vec_push(Vec* v, void* element) {
    if (v->size == v->capacity) {
        v->capacity *= 2;
        v->data = realloc(v->data, sizeof(void*) * v->capacity);
    }
    v->data[v->size] = element;
    v->size++;
}