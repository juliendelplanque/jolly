#include "primitives.h"
#include "log.h"
#include <stdio.h>

#define extract_result_address(vm) \
    vm->memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] << DOUBLE_WORD_SIZE \
        | vm->memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] << WORD_SIZE \
        | vm->memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS]

#define primitive_ok(vm) \
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_OK_RESULT_CODE

void primitive_fail(struct virtual_machine *vm){
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
}

void primitive_nop(struct virtual_machine *vm){
    // Does nothing on purpose.
    primitive_ok(vm);
}

void primitive_get_char(struct virtual_machine *vm){
    unsigned int result_address;
    
    result_address = extract_result_address(vm);

    vm->memory[result_address] = (WORD)getchar();
    primitive_ok(vm);
}

void primitive_put_char(struct virtual_machine *vm){
    unsigned int result_address;
    int primitive_result;
    
    result_address = extract_result_address(vm);
    
    primitive_result = putchar(vm->memory[result_address]);
    
    if(primitive_result == EOF){
        primitive_fail(vm);
    } else{
        primitive_ok(vm);
    }
}

void primitive_save_snapshot(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_stop(struct virtual_machine *vm){
    //TODO: handle exit code to return to the OS.
    vm->status = VIRTUAL_MACHINE_STOP;
    primitive_ok(vm);
}

void primitive_extended(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}
