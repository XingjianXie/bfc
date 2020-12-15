//
//  buffer.c
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#include "buffer.h"


void buffer_init(struct buffer *b, size_t cup) {
    b->cup = cup;
    b->ptr = jit_mmap(b->cup);
    b->size = 0;
}
void buffer_free(struct buffer *b) {
    munmap(b->ptr, b->cup);
}
void buffer_extend(struct buffer *b) { // double size the space
    void *new_ptr = jit_mmap(b->cup * 2);
    memcpy(new_ptr, b->ptr, b->size);
    buffer_free(b);
    b->cup *= 2;
    b->ptr = new_ptr;
}
// Opcode appending functions
int buffer_append(struct buffer *b, unsigned char elem) {
    int extended = 0;
    if (b->size == b->cup) {
        buffer_extend(b);
        extended = 1;
    }
    ((unsigned char *)b->ptr)[b->size++] = elem;
    return extended;
}
int buffer_append2(struct buffer *b, unsigned char elem0, unsigned char elem1) {
    return buffer_append(b, elem0) + buffer_append(b, elem1);
}
int buffer_append3(struct buffer *b, unsigned char elem0, unsigned char elem1, unsigned char elem2) {
    return buffer_append(b, elem0) + buffer_append(b, elem1) + buffer_append(b, elem2);
}
int buffer_append4(struct buffer *b, unsigned char elem0, unsigned char elem1, unsigned char elem2, unsigned char elem3) {
    return buffer_append(b, elem0) + buffer_append(b, elem1) + buffer_append(b, elem2) + buffer_append(b, elem3);
}
int buffer_append8(struct buffer *b, unsigned char elem0, unsigned char elem1, unsigned char elem2, unsigned char elem3, unsigned char elem4, unsigned char elem5, unsigned char elem6, unsigned char elem7) {
    return buffer_append(b, elem0) + buffer_append(b, elem1) + buffer_append(b, elem2) + buffer_append(b, elem3) + buffer_append(b, elem4) + buffer_append(b, elem5) + buffer_append(b, elem6) + buffer_append(b, elem7);
}
int buffer_append_d(struct buffer *b, unsigned int p) {
    return buffer_append4(b, ((unsigned char *)&p)[0], ((unsigned char *)&p)[1], ((unsigned char *)&p)[2], ((unsigned char *)&p)[3]);
}
int buffer_append_ptr(struct buffer *b, void *p) {
    return buffer_append8(b, ((unsigned char *)&p)[0], ((unsigned char *)&p)[1], ((unsigned char *)&p)[2], ((unsigned char *)&p)[3], ((unsigned char *)&p)[4], ((unsigned char *)&p)[5], ((unsigned char *)&p)[6], ((unsigned char *)&p)[7]);
}

int buffer_call(struct buffer *b) {
    sys_icache_invalidate(b->ptr, b->size);
    int (*func)(void) = b->ptr;
    return func();
}
