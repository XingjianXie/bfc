//
//  bf_string.h
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#ifndef bf_string_h
#define bf_string_h

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include "buffer.h"
#include "stack.h"

struct bf_string {
   unsigned char *ptr;
    size_t size;
    size_t cup;
};

void bf_string_init(struct bf_string *, size_t);
void bf_string_free(struct bf_string *);
void bf_string_extend(struct bf_string *);
int bf_string_append(struct bf_string *, unsigned char);
struct buffer* bf_string_compile(struct bf_string *, size_t);


#endif /* bf_string_h */
