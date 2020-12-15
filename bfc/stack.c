//
//  stack.c
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#include "stack.h"

void stack_init(struct stack *b, size_t cup) {
    b->cup = cup;
    b->ptr = malloc(cup * sizeof(size_t));
    b->size = 0;
}
void stack_free(struct stack *b) {
    free(b->ptr);
}
void stack_extend(struct stack *b) { // double size the space
    void *new_ptr = malloc(b->cup * sizeof(size_t) * 2);
    memcpy(new_ptr, b->ptr, b->size * sizeof(size_t));
    stack_free(b);
    b->cup *= 2;
    b->ptr = new_ptr;
}
int stack_append(struct stack *b, size_t elem) {
    int extended = 0;
    if (b->size == b->cup) {
        stack_extend(b);
        extended = 1;
    }
    b->ptr[b->size++] = elem;
    return extended;
}
size_t stack_pop(struct stack *b) {
    return b->ptr[--b->size];
}
