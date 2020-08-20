#include "vm.h"
#include "primitives.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef ENABLE_LOGGING
#include "log.h"
#else
#include "nolog.h"
#endif

/* Debug functions declarations. ---------------------------------------------*/
void print_pc_address(struct virtual_machine *vm);
void print_value_at_address(struct virtual_machine *vm, unsigned int address);


/* Implementation. -----------------------------------------------------------*/
unsigned int get_primitive_call_id(struct virtual_machine *vm){
    return vm->memory[PRIMITIVE_CALL_ID_ADDRESS];
}

void set_primitive_call_id(struct virtual_machine *vm, WORD value){
    vm->memory[PRIMITIVE_CALL_ID_ADDRESS] = value;
}

void set_primitive_is_ready(struct virtual_machine *vm, WORD value){
    vm->memory[PRIMITIVE_IS_READY_ADDRESS] = value;
}

int is_primitive_ready(struct virtual_machine *vm){
    return vm->memory[PRIMITIVE_IS_READY_ADDRESS] == PRIMITIVE_READY;
}

int did_primitive_failed(struct virtual_machine *vm){
    return vm->memory[PRIMITIVE_RESULT_CODE_ADDRESS] == PRIMITIVE_FAILED_RESULT_CODE;
}

unsigned int extract_pc(struct virtual_machine *vm){
    return vm->memory[PC_HIGH_ADDRESS] << DOUBLE_WORD_SIZE
            | vm->memory[PC_MIDDLE_ADDRESS] << WORD_SIZE
            | vm->memory[PC_LOW_ADDRESS];
}

int load_pc(struct virtual_machine *vm){
    vm->pc = vm->memory + extract_pc(vm);
    return 0;
}

int new_vm(struct virtual_machine **vm){
    *vm = NULL_VM;
    *vm = (struct virtual_machine *) malloc(sizeof(struct virtual_machine));
    if(*vm == NULL_VM){
        return VM_ALLOCATION_FAILED;
    }
    if(initialize_primitives_data(*vm) != 0){
        return VM_ALLOCATION_FAILED; //TODO other error code.
    }
    (*vm)->status = VIRTUAL_MACHINE_RUN;
    return VM_OK;
}

int set_memory(struct virtual_machine* vm, WORD *memory){
    if(memory == NULL_MEMORY){
        return VM_INVALID_MEMORY;
    }
    vm->memory = memory;
    load_pc(vm);
    return VM_OK;
}

int create_empty_memory(struct virtual_machine* vm){
    WORD *memory;
    memory = (WORD *)calloc(1, MAX_MEMORY_SIZE);
    if(memory == NULL_MEMORY){
        return VM_MEMORY_ALLOCATION_FAILED;
    }
    return set_memory(vm, memory);
}

void free_vm(struct virtual_machine *vm){
    finalize_primitives_data(vm);
    if(vm->memory != NULL_MEMORY){
        free(vm->memory);
    }
    free(vm);
}

int get_pc_address(struct virtual_machine *vm){
    return ((unsigned)vm->pc)-((unsigned)vm->memory);
}

int set_pc_address(struct virtual_machine *vm, unsigned int pc_address){
    if(vm->memory == NULL_MEMORY){
        return VM_INVALID_MEMORY;
    }
    vm->pc = vm->memory + pc_address;
    return VM_OK;
}

int serialize_pc(struct virtual_machine *vm){
    int pc;
    if(vm->memory == NULL_MEMORY){
        return VM_MEMORY_UNINITIALIZED;
    }
    pc = get_pc_address(vm);
    vm->memory[PC_HIGH_ADDRESS] = (pc >> DOUBLE_WORD_SIZE)
                                    & WORD_BIT_MASK;
    vm->memory[PC_MIDDLE_ADDRESS] = (pc >> WORD_SIZE)
                                    & WORD_BIT_MASK;
    vm->memory[PC_LOW_ADDRESS] = pc & WORD_BIT_MASK;
    return VM_OK;
}

int execute_primitive(struct virtual_machine *vm){
    WORD primitive_id;
    // Retrieve the id of the primitive to be executed.
    primitive_id = get_primitive_call_id(vm);
    log_debug("Execute primitive %d", primitive_id);
    switch(primitive_id){
        case(PRIMITIVE_ID_NOPE):
            primitive_nop(vm);
            break;
        case(PRIMITIVE_ID_FAIL):
            primitive_fail(vm);
            break;
        case(PRIMITIVE_ID_PUT_CHAR):
            primitive_put_char(vm);
            break;
        case(PRIMITIVE_ID_GET_CHAR):
            primitive_get_char(vm);
            break;
        case(PRIMITIVE_ID_STOP_VM):
            primitive_stop(vm);
            break;
        case(PRIMITIVE_ID_OPEN_FILE):
            primitive_open_file(vm);
            break;
        case(PRIMITIVE_ID_CLOSE_FILE):
            primitive_close_file(vm);
            break;
        case(PRIMITIVE_ID_IS_FILE_OPEN):
            primitive_is_file_open(vm);
            break;
        default: // In case no primitive is associated to an id, the call fails.
            primitive_fail(vm);
            break;
    }

    if (did_primitive_failed(vm)){
        log_error("Primitive %d failed.\n", primitive_id);
    }

    // Set the value of primitive to execute to PRIMITIVE_ID_NOPE
    // like that, if the code sets activate the primitive handler by accident,
    // nope primitive will be executed which is handy as it does nothing.
    set_primitive_call_id(vm, PRIMITIVE_ID_NOPE);

    // Set back the primitive trigger to PRIMITIVE_NOT_READY.
    set_primitive_is_ready(vm, PRIMITIVE_NOT_READY);
    return 0;
}

int execute_instruction(struct virtual_machine *vm){
    unsigned int from_address, to_address, jump_address;
    // Checks if vm needs to execute a primitive before executing an
    // instruction.
    if(is_primitive_ready(vm)){
        execute_primitive(vm);
    }

    // Compute address to copy word from.
    from_address = vm->pc[FROM_ADDRESS_HIGH_OFFSET] << DOUBLE_WORD_SIZE
        | vm->pc[FROM_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
        | vm->pc[FROM_ADDRESS_LOW_OFFSET];
    
    // Compute address to copy word to.
    to_address = vm->pc[TO_ADDRESS_HIGH_OFFSET] << DOUBLE_WORD_SIZE 
        | vm->pc[TO_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
        | vm->pc[TO_ADDRESS_LOW_OFFSET];

    // Copy word pointed by from_address to to_address.
    vm->memory[to_address] = vm->memory[from_address];

    // Compute next address for program counter: the jump address.
    jump_address = vm->pc[JUMP_ADDRESS_HIGH_OFFSET] << DOUBLE_WORD_SIZE
        | vm->pc[JUMP_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
        | vm->pc[JUMP_ADDRESS_LOW_OFFSET];
    
    // Update program counter according to jump_address (absolute jump).
    vm->pc = vm->memory + jump_address;
    return VM_OK;
}

int run(struct virtual_machine *vm){
    while(vm->status == VIRTUAL_MACHINE_RUN){
        execute_instruction(vm);
    }
    return VM_OK;
}

int load_image(struct virtual_machine *vm, char *filename){
    long length;
    FILE * f = fopen (filename, "rb");

    if (f){
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        vm->memory = (WORD *)malloc(MAX_MEMORY_SIZE);
        if (vm->memory)
        {
            fread(vm->memory, 1, length, f);
        }
        fclose (f);
    } else{
        log_error("File does not exist %s", filename);
    }

    if (! vm->memory){
        return VM_MEMORY_ALLOCATION_FAILED;
    }
    return VM_OK;
}

/**
 * From here, a set of debug functions.
 */

void print_pc_address(struct virtual_machine *vm){
    printf("pc = 0x%06X\n", get_pc_address(vm));
}

void print_value_at_address(struct virtual_machine *vm, unsigned int address){
    printf("memory[0x%06X] = %c | %d\n",
        address,
        vm->memory[address],
        vm->memory[address]);
}