#include "primitives.h"
#include "log.h"
#include <stdio.h>

void primitive_fail(struct virtual_machine *vm){
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
}

void primitive_nop(struct virtual_machine *vm){
    // Does nothing on purpose.
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_OK_RESULT_CODE;
}

void primitive_get_char(struct virtual_machine *vm){
    unsigned int result_address;
    
    result_address = 
        vm->memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] << DOUBLE_WORD_SIZE 
        | vm->memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] << WORD_SIZE
        | vm->memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS];

    vm->memory[result_address] = (WORD)getchar();
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_OK_RESULT_CODE;
}

void primitive_put_char(struct virtual_machine *vm){
    unsigned int result_address;
    int primitive_result;
    
    result_address = 
        vm->memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] << DOUBLE_WORD_SIZE 
        | vm->memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] << WORD_SIZE
        | vm->memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS];

    log_debug("Address of char to pass to put_char(): 0x%06X", result_address);
    log_debug("Char to pass to put_char(): %c", vm->memory[result_address]);
    
    primitive_result = putchar(vm->memory[result_address]);
    fflush(stdout);
    if(primitive_result == EOF){
        log_warn("put_char() primitive failed.");
        vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
    } else{
        vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_OK_RESULT_CODE;
    }
}

void primitive_save_snapshot(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_stop(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}
