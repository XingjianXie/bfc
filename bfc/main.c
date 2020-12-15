//
//  main.c
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//


#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "adapter.h"

int main(int n, char **args) {
    FILE* f = stdin;
    if (n == 2) {
        f = fopen(args[1], "r");
    }
    pthread_jit_write_protect_np(0); // For Apple Silicon
    struct buffer* b = buffer_from_brainfuck(f, 1024 * 1024);
    pthread_jit_write_protect_np(1); // For Apple Silicon
    buffer_call(b); // Execute buffer
    buffer_free(b);
    free(b);
    return 0;
}
