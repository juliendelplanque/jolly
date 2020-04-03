#include "primitives.h"

void primitive_fail(unsigned char *memory){
    memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
}

void primitive_nop(unsigned char *memory){
    // Does nothing on purpose.
}