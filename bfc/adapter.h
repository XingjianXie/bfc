//
//  adapter.h
//  bfc
//
//  Created by 谢行健 on 2020/12/15.
//

#ifndef adapter_h
#define adapter_h

#include <stdio.h>
#include "bf_string.h"

struct buffer* buffer_from_file(FILE *, size_t);
struct buffer* buffer_from_brainfuck(FILE *, size_t);

#endif /* adapter_h */
