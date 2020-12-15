//
//  stack.h
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#ifndef stack_h
#define stack_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stack {
    size_t *ptr;
    size_t size;
    size_t cup;
};
void stack_init(struct stack *, size_t);
void stack_free(struct stack *);
void stack_extend(struct stack *);
int stack_append(struct stack *, size_t);
size_t stack_pop(struct stack *);


#endif /* stack_h */

