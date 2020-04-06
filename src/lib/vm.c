#include "vm.h"
#include "memory.h"
#include "primitives.h"

#include <stdlib.h>
#include <stdio.h>

void print_pc_address(WORD *memory, WORD *pc){
    printf("pc = 0x%06X\n", ((unsigned)pc)-((unsigned)memory));
}

void print_value_at_address(WORD *memory, unsigned int address){
    printf("memory[0x%06X] = %c | %d\n",
        address,
        memory[address],
        memory[address]);
}

int load_pc(struct virtual_machine *vm){
    vm->pc = vm->memory + (vm->memory[PC_HIGH_ADDRESS] << DOUBLE_WORD_SIZE
                    | vm->memory[PC_MIDDLE_ADDRESS] << WORD_SIZE
                    | vm->memory[PC_LOW_ADDRESS]);
    return 0;
}

struct virtual_machine* new_vm(){
    struct virtual_machine* vm = (struct virtual_machine *) malloc(sizeof(struct virtual_machine));
    if(vm == NULL_VM){
        fprintf(stderr, "Malloc failed, unable to create VM.\n");
    }
    vm->status = VIRTUAL_MACHINE_RUN;
    return vm;
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
    if(vm->memory != NULL_MEMORY){
        free(vm->memory);
    }
    free(vm);
}

int get_pc_address(struct virtual_machine *vm){
    return ((unsigned)vm->pc)-((unsigned)vm->memory);
}

int set_pc_address(struct virtual_machine *vm, int pc_address){
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
    primitive_id = vm->memory[PRIMITIVE_CALL_ID_ADDRESS];
    switch(primitive_id){
        case(PRIMITIVE_ID_NOPE):
            primitive_nop(vm);
            break;
        case(PRIMITIVE_ID_FAIL):
            primitive_fail(vm);
            break;
        case(PRIMITIVE_ID_PUT_CHAR):
            primitive_get_char(vm);
            break;
        default: // In case no primitive is associated to an id, the call fails.
            primitive_fail(vm);
            break;
    }

    // Set the value of primitive to execute to PRIMITIVE_ID_NOPE
    // like that, if the code sets activate the primitive handler by accident,
    // nope primitive will be executed which is handy as it does nothing.
    vm->memory[PRIMITIVE_CALL_ID_ADDRESS] = PRIMITIVE_ID_NOPE;

    // Set back the primitive trigger to PRIMITIVE_NOT_READY.
    vm->memory[PRIMITIVE_IS_READY_ADDRESS] = PRIMITIVE_NOT_READY;
    return 0;
}

int execute_instruction(struct virtual_machine *vm){
    unsigned int from_address, to_address, jump_address;
    // Checks if vm needs to execute a primitive before executing an
    // instruction.
    if(vm->memory[PRIMITIVE_IS_READY_ADDRESS] == PRIMITIVE_READY){
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
    return 0;
}

int load_snapshot(struct virtual_machine *vm, char *filename){
    long length;
    FILE * f = fopen (filename, "rb");

    if (f){
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        vm->memory = malloc(MAX_MEMORY_SIZE);
        if (vm->memory)
        {
            fread(vm->memory, 1, length, f);
        }
        fclose (f);
    }

    if (! vm->memory){
        return -1;
    }
    return 0;
}
