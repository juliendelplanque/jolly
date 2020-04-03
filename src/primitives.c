#include "primitives.h"
#include <stdio.h>

void primitive_fail(WORD *memory){
    memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
}

void primitive_nop(WORD *memory){
    // Does nothing on purpose.
}

void primitive_get_char(WORD *memory){
    unsigned int result_address;
    
    result_address = 
        memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] << DOUBLE_WORD_SIZE 
        | memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] << WORD_SIZE
        | memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS];

    memory[result_address] = (WORD)getchar();
}

void primitive_put_char(WORD *memory){
    //TODO
    primitive_fail(memory);
}