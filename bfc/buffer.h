//
//  buffer.h
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#ifndef buffer_h
#define buffer_h

#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <libkern/OSCacheControl.h>

struct buffer {
    void *ptr;
    size_t size;
    size_t cup;
};

void buffer_init(struct buffer *, size_t);
void buffer_free(struct buffer *);
void buffer_extend(struct buffer *);
int buffer_append(struct buffer *, unsigned char);
int buffer_append2(struct buffer *, unsigned char, unsigned char);
int buffer_append3(struct buffer *, unsigned char, unsigned char, unsigned char);
int buffer_append4(struct buffer *, unsigned char, unsigned char, unsigned char, unsigned char);
int buffer_append8(struct buffer *, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
int buffer_append_d(struct buffer *,  unsigned int);
int buffer_append_ptr(struct buffer *, void *);

int buffer_call(struct buffer *);


#define jit_mmap(cup) (mmap(NULL, (cup), PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE | MAP_JIT, -1, 0))

#endif /* buffer_h */
