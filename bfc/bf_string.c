//
//  bf_string.c
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#include "bf_string.h"

void bf_string_init(struct bf_string *b, size_t cup) {
    b->cup = cup;
    b->ptr = malloc(cup);
    b->size = 0;
}
void bf_string_free(struct bf_string *b) {
    free(b->ptr);
}
void bf_string_extend(struct bf_string *b) { // double size the space
    void *new_ptr = malloc(b->cup * 2);
    memcpy(new_ptr, b->ptr, b->size);
    bf_string_free(b);
    b->cup *= 2;
    b->ptr = new_ptr;
}
int bf_string_append(struct bf_string *b, unsigned char elem) {
    int extended = 0;
    if (b->size == b->cup) {
        bf_string_extend(b);
        extended = 1;
    }
    b->ptr[b->size++] = elem;
    return extended;
}

struct buffer* bf_string_compile(struct bf_string *bf_str, size_t len) {
    struct buffer *buf = malloc(sizeof(struct buffer));
    buffer_init(buf, bf_str->size * 4);
#if __arm64__
    // sub    sp, sp, #0x20
    buffer_append4(buf, 0xff, 0x83, 0x00, 0xd1);

    // stp    x19, x20, [sp, #0x8]
    buffer_append4(buf, 0xf3, 0xd3, 0x00, 0xa9);

    // stp    x21, x30, [sp, #0x18]
    buffer_append4(buf, 0xf5, 0xfb, 0x01, 0xa9);

    // ldr x0, =len
    buffer_append_d(buf, 0xd2800000 | (((unsigned short*)&len)[0] << 5));
    buffer_append_d(buf, 0xf2a00000 | (((unsigned short*)&len)[1] << 5));
    buffer_append_d(buf, 0xf2c00000 | (((unsigned short*)&len)[2] << 5));
    buffer_append_d(buf, 0xf2e00000 | (((unsigned short*)&len)[3] << 5));

    // ldr x8, =malloc
    void *malloc_p = malloc;
    buffer_append_d(buf, 0xd2800008 | (((unsigned short*)&malloc_p)[0] << 5));
    buffer_append_d(buf, 0xf2a00008 | (((unsigned short*)&malloc_p)[1] << 5));
    buffer_append_d(buf, 0xf2c00008 | (((unsigned short*)&malloc_p)[2] << 5));
    buffer_append_d(buf, 0xf2e00008 | (((unsigned short*)&malloc_p)[3] << 5));

    // blr    x8
    buffer_append4(buf, 0x00, 0x01, 0x3f, 0xd6);

    // mov    x19, x0
    buffer_append4(buf, 0xf3, 0x03, 0x00, 0xaa);

    // ldr x20, =putunsigned char
    void *putchar_p = putchar;
    buffer_append_d(buf, 0xd2800014 | (((unsigned short*)&putchar_p)[0] << 5));
    buffer_append_d(buf, 0xf2a00014 | (((unsigned short*)&putchar_p)[1] << 5));
    buffer_append_d(buf, 0xf2c00014 | (((unsigned short*)&putchar_p)[2] << 5));
    buffer_append_d(buf, 0xf2e00014 | (((unsigned short*)&putchar_p)[3] << 5));

    // ldr x21, =getunsigned char
    void *getchar_p = getchar;
    buffer_append_d(buf, 0xd2800015 | (((unsigned short*)&getchar_p)[0] << 5));
    buffer_append_d(buf, 0xf2a00015 | (((unsigned short*)&getchar_p)[1] << 5));
    buffer_append_d(buf, 0xf2c00015 | (((unsigned short*)&getchar_p)[2] << 5));
    buffer_append_d(buf, 0xf2e00015 | (((unsigned short*)&getchar_p)[3] << 5));

    // Brainfuck starts now
    struct stack *s = malloc(sizeof(struct stack)); // Stack to track '['
    stack_init(s, 64); // at most 64 pairs of '[' and ']', extending automatically when necessary
    for (size_t i = 0; i < bf_str->size; i++) {
        switch (bf_str->ptr[i]) {
            case '>':
                buffer_append4(buf, 0x73, 0x12, 0x00, 0x91);
                break;

            case '<':
                buffer_append4(buf, 0x73, 0x12, 0x00, 0xd1);
                break;

            case '+':
                // ldr    w8, [x19]
                buffer_append4(buf, 0x68, 0x02, 0x40, 0xb9);
                // add    w8, w8, #0x1
                buffer_append4(buf, 0x08, 0x05, 0x00, 0x11);
                // str    w8, [x19]
                buffer_append4(buf, 0x68, 0x02, 0x00, 0xb9);
                break;
            case '-':
                // ldr    w8, [x19]
                buffer_append4(buf, 0x68, 0x02, 0x40, 0xb9);
                // sub    w8, w8, #0x1
                buffer_append4(buf, 0x08, 0x05, 0x00, 0x51);
                // str    w8, [x19]
                buffer_append4(buf, 0x68, 0x02, 0x00, 0xb9);
                break;
            case '.':
                // ldr    w0, [x19]
                buffer_append4(buf, 0x60, 0x02, 0x40, 0xb9);
                // blr    x20
                buffer_append4(buf, 0x80, 0x02, 0x3f, 0xd6);
                break;
            case ',':
                // blr    x21
                buffer_append4(buf, 0xa0, 0x02, 0x3f, 0xd6);
                // str    w0, [x19]
                buffer_append4(buf, 0x60, 0x02, 0x00, 0xb9);
                break;
            case '[':
                // ldr    w0, [x19]
                buffer_append4(buf, 0x60, 0x02, 0x40, 0xb9);

                // cmp    w0, #0x0
                buffer_append4(buf, 0x1f, 0x00, 0x00, 0x71);

                // b.eq    <UNKNOWN>
                buffer_append4(buf, 0x00, 0x00, 0x00, 0x54);

                stack_append(s, buf->size); // Store the current size
                break;
            case ']':
                // ldr    w0, [x19]
                buffer_append4(buf, 0x60, 0x02, 0x40, 0xb9);

                // cmp    w0, #0x0
                buffer_append4(buf, 0x1f, 0x00, 0x00, 0x71);

                // b.ne    <UNKNOWN>
                buffer_append4(buf, 0x01, 0x00, 0x00, 0x54);

                size_t size = stack_pop(s);
                unsigned int dif = (unsigned int)(buf->size - size) / 4 + 1; // I don't like 32 bit offset, but this is the only thing we have
                //prunsigned intf("%d\n", dif);
                *(unsigned int *)&(((unsigned char *)buf->ptr)[size - 4]) |= dif << 5; // Positive offset for jumping to ']'
                unsigned int difX = -dif + 2;
                *(unsigned int *)&(((unsigned char *)buf->ptr)[buf->size - 4]) |= ((difX & 0x7ffff) | 0x40000) << 5; // Negative offset for jumping to ']'

                break;

            default:
                break; // Do nothing and skip any otherunsigned character
        }
    }
    stack_free(s);
    free(s);
    // */
    // ldp    x19, x20, [sp, #0x8]
    buffer_append4(buf, 0xf3, 0xd3, 0x40, 0xa9);
    // ldp    x21, x30, [sp, #0x18]
    buffer_append4(buf, 0xf5, 0xfb, 0x41, 0xa9);
    // add    sp, sp, #0x20
    buffer_append4(buf, 0xff, 0x83, 0x00, 0x91);
    // mov    x0, #0x0
    buffer_append4(buf, 0x00, 0x00, 0x80, 0xd2);
    // ret
    buffer_append4(buf, 0xc0, 0x03, 0x5f, 0xd6);

#endif
#if __x86_64__
    // pushq    %rbx
    buffer_append(buf, 0x53); // Keep %rbx
    // pushq    %r12
    buffer_append2(buf, 0x41, 0x54); // Keep %r12
    // pushq    %r13
    buffer_append2(buf, 0x41, 0x55); // Keep %r13

    // movq    len, %rdi
    buffer_append2(buf, 0x48, 0xbf);
    buffer_append_ptr(buf, (void *) (len * 4));

    // movabsq malloc, %rcx
    buffer_append2(buf, 0x48, 0xb9);
    buffer_append_ptr(buf, malloc); // Pass the address of malloc to asm at runtime

    // callq    *%rcx
    buffer_append2(buf, 0xff, 0xd1); // Call malloc

    // movq  %rax, %rbx
    buffer_append3(buf, 0x48, 0x89, 0xc3); // use %rbx to store pounsigned inter,
                                                               // preventing modify when calling other function

    // movabsq putunsigned char, %r12
    buffer_append2(buf, 0x49, 0xbc);
    buffer_append_ptr(buf, putchar); // Pass the address of putunsigned char to asm at runtime

    // movabsq getunsigned char, %r13
    buffer_append2(buf, 0x49, 0xbd);
    buffer_append_ptr(buf, getchar); // Pass the address of getunsigned char to asm at runtime

    // Brainfuck starts now
    struct stack *s = malloc(sizeof(struct stack)); // Stack to track '['
    stack_init(s, 64); // at most 64 pairs of '[' and ']', extending automatically when necessary
    for (size_t i = 0; i < bf_str->size; i++) {
        switch (bf_str->ptr[i]) {
            case '>':
                // addq    $0x4, %rbx
                buffer_append4(buf, 0x48, 0x83, 0xc3, 0x04); //Just increase the pounsigned inter
                break;

            case '<':
                // addq    $-0x4, %rbx
                buffer_append4(buf, 0x48, 0x83, 0xc3, 0xfc); // Just decrease the pounsigned inter
                break;

            case '+':
                // addl    $0x1, (%rbx)
                buffer_append3(buf, 0x83, 0x03, 0x01); // Increase the cell the pounsigned inter pounsigned inted to
                break;
            case '-':
                // addl    $-0x1, (%rbx)
                buffer_append3(buf, 0x83, 0x03, 0xff); // Decrease the cell the pounsigned inter pounsigned inted to
                break;
            case '.':
                // movl    (%rbx), %edi
                buffer_append2(buf, 0x8b, 0x3b); // Pass to putunsigned char

                // callq    *%r12
                buffer_append3(buf, 0x41, 0xff, 0xd4); // Call putunsigned char
                break;
            case ',':
                // callq    *%r13
                buffer_append3(buf, 0x41, 0xff, 0xd5); // Call getunsigned char

                // movl    %eax, (%rbx)
                buffer_append2(buf, 0x89, 0x03); // Get the return value
                break;
            case '[':
                // cmpl    $0x0, (%rbx)
                buffer_append3(buf, 0x83, 0x3b, 0x00);
                // je <UNKNOWN>
                buffer_append2(buf, 0x0f, 0x84);
                buffer_append4(buf, 0x00, 0x00, 0x00, 0x00); // We don't know the offset now
                                                                                       // but when we got the paired ']'
                                                                                       // we will fix this
                stack_append(s, buf->size); // Store the current size
                break;
            case ']':
                // cmpl    $0x0, (%rbx)
                buffer_append3(buf, 0x83, 0x3b, 0x00);
                // jne <UNKNOWN>
                buffer_append2(buf, 0x0f, 0x85);
                buffer_append4(buf, 0x00, 0x00, 0x00, 0x00); // We are almost there.
                size_t size = stack_pop(s);
                unsigned int dif = (unsigned int)(buf->size - size); // I don't like 32 bit offset, but this is the only thing we have
                //prunsigned intf("%d\n", dif);
                ((unsigned char *)buf->ptr)[size - 4] = ((unsigned char*)&dif)[0]; // Positive offset for jumping to ']'
                ((unsigned char *)buf->ptr)[size - 3] = ((unsigned char*)&dif)[1];
                ((unsigned char *)buf->ptr)[size - 2] = ((unsigned char*)&dif)[2];
                ((unsigned char *)buf->ptr)[size - 1] = ((unsigned char*)&dif)[3];
                dif = -dif;
                ((unsigned char *)buf->ptr)[buf->size - 4] = ((unsigned char*)&dif)[0]; // Negative offset for jumping to ']'
                ((unsigned char *)buf->ptr)[buf->size - 3] = ((unsigned char*)&dif)[1];
                ((unsigned char *)buf->ptr)[buf->size - 2] = ((unsigned char*)&dif)[2];
                ((unsigned char *)buf->ptr)[buf->size - 1] = ((unsigned char*)&dif)[3];
                break;

            default:
                break; // Do nothing and skip any otherunsigned character
        }
    }
    stack_free(s);
    free(s);

    // xorl    %eax, %eax
    buffer_append2(buf, 0x31, 0xc0); // Return 0

    // popq    %r13
    buffer_append2(buf, 0x41, 0x5d); // Pop our kept %r13
    // popq    %r12
    buffer_append2(buf, 0x41, 0x5c); // Pop our kept %r12

    // popq    %rbx
    // retq
    buffer_append2(buf, 0x5b, 0xc3); // Pop our kept %rbx, finally return

#endif
    return buf;
}
