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

int execute_primitive(WORD *memory){
    WORD primitive_id;
    // Retrieve the id of the primitive to be executed.
    primitive_id = memory[PRIMITIVE_CALL_ID_ADDRESS];
    switch(primitive_id){
        case(PRIMITIVE_ID_NOPE):
            primitive_nop(memory);
            break;
        case(PRIMITIVE_ID_FAIL):
            primitive_fail(memory);
            break;
        case(PRIMITIVE_ID_PUT_CHAR):
            primitive_get_char(memory);
            break;
        default: // In case no primitive is associated to an id, the call fails.
            primitive_fail(memory);
            break;
    }

    // Set the value of primitive to execute to PRIMITIVE_ID_NOPE
    // like that, if the code sets activate the primitive handler by accident,
    // nope primitive will be executed which is handy as it does nothing.
    memory[PRIMITIVE_CALL_ID_ADDRESS] = PRIMITIVE_ID_NOPE;

    // Set back the primitive trigger to PRIMITIVE_NOT_READY.
    memory[PRIMITIVE_IS_READY_ADDRESS] = PRIMITIVE_NOT_READY;
    return 0;
}

int execute_instruction(WORD *memory, WORD *pc){
    unsigned int from_address, to_address, jump_address;
    if(memory[PRIMITIVE_IS_READY_ADDRESS] == PRIMITIVE_READY){
        execute_primitive(memory);
    }

    // Compute address to copy word from.
    from_address = pc[FROM_ADDRESS_HIGH_OFFSET] << DOUBLE_WORD_SIZE
        | pc[FROM_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
        | pc[FROM_ADDRESS_LOW_OFFSET];
    
    // Compute address to copy word to.
    to_address = pc[TO_ADDRESS_HIGH_OFFSET] << DOUBLE_WORD_SIZE 
        | pc[TO_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
        | pc[TO_ADDRESS_LOW_OFFSET];

    // Copy word pointed by from_address to to_address.
    memory[to_address] = memory[from_address];

    // Compute next address for program counter: the jump address.
    jump_address = pc[JUMP_ADDRESS_HIGH_OFFSET] << DOUBLE_WORD_SIZE
        | pc[JUMP_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
        | pc[JUMP_ADDRESS_LOW_OFFSET];
    
    // Update program counter according to jump_address (absolute jump).
    pc = memory + jump_address;
    return 0;
}

int load_pc(WORD *memory, WORD **pc){
    *pc = memory + (memory[PC_HIGH_ADDRESS] << DOUBLE_WORD_SIZE
                    | memory[PC_MIDDLE_ADDRESS] << WORD_SIZE
                    | memory[PC_LOW_ADDRESS]);
    return 0;
}

int load_snapshot(char *filename, WORD **memory){
    long length;
    FILE * f = fopen (filename, "rb");

    if (f){
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        *memory = malloc(MAX_MEMORY_SIZE);
        if (*memory)
        {
            fread(*memory, 1, length, f);
        }
        fclose (f);
    }

    if (! *memory){
        return -1;
    }
    return 0;
}

int main(){
    WORD *memory, *pc;
    memory = (WORD *)malloc(MAX_MEMORY_SIZE);
    if(memory == NULL){
        fprintf(stderr, "Malloc failed.\n");
    }
    memory[PC_HIGH_ADDRESS] = 0x0;
    memory[PC_MIDDLE_ADDRESS] = 0x42;
    memory[PC_LOW_ADDRESS] = 0x10;

    memory[PRIMITIVE_IS_READY_ADDRESS] = PRIMITIVE_READY;
    memory[PRIMITIVE_CALL_ID_ADDRESS] = PRIMITIVE_ID_PUT_CHAR;
    memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] = 0x00;
    memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] = 0x01;
    memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS] = 0x00;
    load_pc(memory, &pc);
    print_pc_address(memory, pc);
    execute_instruction(memory, pc);

    print_value_at_address(memory, PRIMITIVE_IS_READY_ADDRESS);
    print_value_at_address(memory, 0x000100);
    free(memory);
    return 0;
}
