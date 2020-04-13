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
    FILE * input_stream;
    
    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);
    switch(vm->memory[result_address+1]){
        case(PRIMITIVE_FILE_INPUT_STREAM_STDIN):
            input_stream = stdin;
            break;
        default: // Unknown output stream.
            log_debug(
                "   Unknown input stream with id=%d.",
                vm->memory[result_address+1]
            );
            primitive_fail(vm);
            return;
    }

    vm->memory[result_address] = (WORD)fgetc(input_stream);
    log_debug( "    char=%c.", vm->memory[result_address]);
    primitive_ok(vm);
}

void primitive_put_char(struct virtual_machine *vm){
    unsigned int result_address;
    int primitive_result;
    char char_to_put;
    FILE * output_stream;
    
    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);

    char_to_put = vm->memory[result_address];
    log_debug( "   char_to_put=%c.", char_to_put);

    switch(vm->memory[result_address+1]){
        case(PRIMITIVE_FILE_OUTPUT_STREAM_STDOUT):
            output_stream = stdout;
            break;
        case(PRIMITIVE_FILE_OUTPUT_STREAM_STDERR):
            output_stream = stderr;
            break;
        default: // Unknown output stream.
            log_debug(
                "   Unknown output stream with id=%d.",
                vm->memory[result_address+1]
            );
            primitive_fail(vm);
            return;
    }
    log_debug(
        "   filestream=%d.",
        vm->memory[result_address+1]);
    primitive_result = fputc(char_to_put, output_stream);
    
    if(primitive_result == EOF){
        primitive_fail(vm);
    } else{
        primitive_ok(vm);
    }
}

void primitive_stop(struct virtual_machine *vm){
    //TODO: handle exit code to return to the OS.
    vm->status = VIRTUAL_MACHINE_STOP;
    primitive_ok(vm);
}

void primitive_write_file(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_read_file(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_extended(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}
