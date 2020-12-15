//
//  adapter.c
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#include "adapter.h"

struct buffer* buffer_from_file(FILE * file, size_t size) {
    struct buffer *b = malloc(sizeof(struct buffer));
    buffer_init(b, size);
     unsigned int raw[4];
    while(~fscanf(file, "%x %x %x %x", &raw[0], &raw[1], &raw[2], &raw[3])) {
        buffer_append4(b, raw[0], raw[1], raw[2], raw[3]);
    }
    return b;
}

struct buffer* buffer_from_brainfuck(FILE * file, size_t size) {
    struct bf_string *b = malloc(sizeof(struct bf_string));
    bf_string_init(b, 512);
    unsigned char ch;
    while(~fscanf(file, "%c", &ch)) {
        bf_string_append(b, ch);
    }
    clearerr(file);
    struct buffer *buf = bf_string_compile(b, size); // Compile Brainfuck code to opcode
    bf_string_free(b);
    free(b);
    return buf;
}

