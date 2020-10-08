#include "primitives.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ENABLE_LOGGING
#ifdef ENABLE_LOGGING
#include "log.h"
#else
#include "nolog.h"
#endif

#define ERROR_NO_STREAM_AVAILABLE (-1)

unsigned int extract_address(struct virtual_machine *vm, unsigned int address){
    return vm->memory[address] << DOUBLE_WORD_SIZE
        | vm->memory[address+1] << WORD_SIZE
        | vm->memory[address+2];
}

unsigned int extract_result_address(struct virtual_machine *vm){
    return extract_address(vm, PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS);
}

void primitive_ok(struct virtual_machine *vm){
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_OK_RESULT_CODE;
}

int initialize_primitives_data(struct virtual_machine *vm){
    // Initialize special file streams.
    vm->file_streams[PRIMITIVE_FILE_STREAM_STDIN] = stdin;
    vm->file_streams[PRIMITIVE_FILE_STREAM_STDOUT] = stdout;
    vm->file_streams[PRIMITIVE_FILE_STREAM_STDERR] = stderr;

    // Initialize other file streams to be null.
    for(int i=PRIMITIVE_FILE_STREAM_STDERR+1;
        i<FILE_STREAMS_SIZE;
        i++){
        vm->file_streams[i] = NULL;
    }
    return PRIMITIVE_OK;
}

int finalize_primitives_data(struct virtual_machine *vm){
    // Remove reference to special streams.
    vm->file_streams[PRIMITIVE_FILE_STREAM_STDIN] = NULL;
    vm->file_streams[PRIMITIVE_FILE_STREAM_STDOUT] = NULL;
    vm->file_streams[PRIMITIVE_FILE_STREAM_STDERR] = NULL;
    // Close input file streams still open.
    for(int i=PRIMITIVE_FILE_STREAM_STDERR+1;
        i<FILE_STREAMS_SIZE;
        i++){
        int error;
        if(vm->file_streams[i] != NULL){
            if((error=fclose(vm->file_streams[i])) != 0){
                log_error(
                    "Error with code %d while closing output file stream %d.",
                    error, i
                );
            } else{
                vm->file_streams[i] = NULL;
            }
        }
    }
    return PRIMITIVE_OK;
}

void primitive_fail(struct virtual_machine *vm){
    vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] = PRIMITIVE_FAILED_RESULT_CODE;
}

void primitive_nop(struct virtual_machine *vm){
    // Does nothing on purpose.
    primitive_ok(vm);
}

void primitive_get_char(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned char stream_id;
    int fgetc_result;
    FILE * input_stream;
    
    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);

    stream_id = vm->memory[result_address];
    input_stream = vm->file_streams[stream_id];

    if(input_stream == NULL){
        log_debug(
            "   Attempt to read non allocated stream with id=%d.",
            stream_id
            );
        primitive_fail(vm);
        return;
    }

    log_debug("   filestream=%d.", stream_id);
    fgetc_result = fgetc(input_stream);
    if(fgetc_result == EOF){
        primitive_fail(vm);
        return;
    }
    vm->memory[result_address] = (WORD)fgetc_result;
    log_debug( "    char=%c.", vm->memory[result_address]);
    primitive_ok(vm);
}

void primitive_put_char(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned char stream_id;
    int primitive_result;
    char char_to_put;
    FILE * output_stream;
    
    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);

    char_to_put = vm->memory[result_address];
    log_debug( "   char_to_put=%c.", char_to_put);

    stream_id = vm->memory[result_address+1];
    output_stream = vm->file_streams[stream_id];

    if(output_stream == NULL){
        log_debug(
            "   Attempt to write to non allocated stream with id=%d.",
            stream_id
            );
        primitive_fail(vm);
        return;
    }
    
    log_debug("   filestream=%d.", stream_id);
    primitive_result = fputc(char_to_put, output_stream);
    
    if(primitive_result == EOF){
        log_debug("    %s", strerror(errno));
        primitive_fail(vm);
    } else{
        primitive_ok(vm);
    }
}

void primitive_stop(struct virtual_machine *vm){
    vm->status = VIRTUAL_MACHINE_STOP;
    primitive_ok(vm);
}

int find_available_stream_slot(struct virtual_machine *vm){
    for(int i=PRIMITIVE_FILE_STREAM_STDERR+1;
        i<FILE_STREAMS_SIZE;
        i++){
        if(vm->file_streams[i] == NULL){
            return i;
        }
    }
    return ERROR_NO_STREAM_AVAILABLE;
}

void primitive_open_file(struct virtual_machine *vm){
    unsigned int result_address;
    int stream_id;
    unsigned char file_open_mode_code;
    char *file_open_mode;
    char *file_path;
    // First, try to find an available slot in VM streams to allocate the new
    // file stream.
    stream_id = find_available_stream_slot(vm);
    if(stream_id == ERROR_NO_STREAM_AVAILABLE){
        log_debug(
            "   No more stream slot available for the virtual machine.",
            stream_id);
        primitive_fail(vm);
        return;
    }
    // Then read primitives arguments.
    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);
    // First, the file open mode.
    file_open_mode_code = vm->memory[result_address];
    switch(file_open_mode_code){
        case(PRIMITIVE_FILE_MODE_READ):
            file_open_mode = "r";
            break;
        case(PRIMITIVE_FILE_MODE_WRITE):
            file_open_mode = "w";
            break;
        case(PRIMITIVE_FILE_MODE_APPEND):
            file_open_mode = "a";
            break;
        default:
            log_error("   Unknown file open mode code: %d", file_open_mode_code);
            primitive_fail(vm);
            return;
    }
    // Then, the null terminated string located just after.
    // This string contains the path to the file.
    file_path = (char *)vm->memory + result_address + 1;

    vm->file_streams[stream_id] = fopen(file_path, file_open_mode);

    if (vm->file_streams[stream_id] == NULL){
        log_error("    fopen call failed.");
        log_debug("    %s", strerror(errno));
        primitive_fail(vm);
        return;
    }
    vm->memory[result_address] = stream_id;
    primitive_ok(vm);
}

void primitive_close_file(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned char stream_id;
    int primitive_result;
    FILE * file_stream;

    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);
    stream_id = vm->memory[result_address];
    file_stream = vm->file_streams[stream_id];

    if(file_stream == NULL){
        log_debug(
            "   Attempt to close non allocated stream with id=%d.",
            stream_id);
        primitive_fail(vm);
        return;
    }
    primitive_result = fclose(file_stream);
    if (primitive_result != 0){
        log_debug(
            "   fclose function failed with error code=%d.",
            primitive_result);
    }
    vm->file_streams[stream_id] = NULL;
    primitive_ok(vm);
}

void primitive_is_file_open(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned char stream_id;

    result_address = extract_result_address(vm);
    log_debug("    result_address = 0x%06X", result_address);
    stream_id = vm->memory[result_address];
    
    if(vm->file_streams[stream_id] == NULL){
        vm->memory[result_address] = PRIMITIVE_FILE_IS_CLOSED;
    } else{
        vm->memory[result_address] = PRIMITIVE_FILE_IS_OPEN;
    }
    primitive_ok(vm);
}

void primitive_argc(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_argv_size_at_index(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_argv(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_add_addresses(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned long x, y, sum;

    result_address = extract_result_address(vm);

    x = extract_address(vm, result_address);
    y = extract_address(vm, result_address+3);
    sum = x + y;
    vm->memory[result_address+3] = (sum & 0xFF000000) > 0;
    sum = sum % 0xFFFFFF;
    vm->memory[result_address] = (sum & 0xFF0000) >> DOUBLE_WORD_SIZE;
    vm->memory[result_address+1] = (sum & 0x00FF00) >> WORD_SIZE;
    vm->memory[result_address+2] = (sum & 0x0000FF);
    primitive_ok(vm);
}

void primitive_substract_addresses(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}

void primitive_decrement_address(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned long x, decremented;

    result_address = extract_result_address(vm);
    x = extract_address(vm, result_address);
    if(x == 0){
        decremented = 0xFFFFFF;
    } else{
        decremented = x - 1;
    }
    vm->memory[result_address] = (decremented & 0xFF0000) >> DOUBLE_WORD_SIZE;
    vm->memory[result_address+1] = (decremented & 0x00FF00) >> WORD_SIZE;
    vm->memory[result_address+2] = (decremented & 0x0000FF);
    primitive_ok(vm);
}

void primitive_increment_address(struct virtual_machine *vm){
    unsigned int result_address;
    unsigned long x, incremented;

    result_address = extract_result_address(vm);
    x = extract_address(vm, result_address);
    incremented = (x + 1) % 0x1000000;
    vm->memory[result_address] = (incremented & 0xFF0000) >> DOUBLE_WORD_SIZE;
    vm->memory[result_address+1] = (incremented & 0x00FF00) >> WORD_SIZE;
    vm->memory[result_address+2] = (incremented & 0x0000FF);
    primitive_ok(vm);
}

void primitive_extended(struct virtual_machine *vm){
    //TODO
    primitive_fail(vm);
}
