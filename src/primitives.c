#include "primitives.h"

/**
 * A primitive that always fails.
 * Writes the PRIMITIVE_FAILED_RESULT_CODE at PRIMITIVE_RESULT_CODE_ADDRESS.
 */
void primitive_fail(unsigned char *memory){
    memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
}

void primitive_nop(unsigned char *memory){
    unsigned int result_address;
    result_address = 
        memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] << DOUBLE_WORD_SIZE
            | memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] << WORD_SIZE
            | memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS];
    // TODO
    primitive_fail(memory);
}

int execute_primitive(unsigned char *memory){
    unsigned char primitive_id;
    // Retrieve the id of the primitive to be executed.
    primitive_id = memory[PRIMITIVE_CALL_ID_ADDRESS];
    switch(primitive_id){
        case(PRIMITIVE_ID_NOPE):
            primitive_nop(memory);
            break;
        case(PRIMITIVE_ID_FAIL):
            primitive_fail(memory);
            break;
        default: // In case no primitive is associated to an id, the call fails.
            primitive_fail(memory);
            break;
    }
    return 0;
}