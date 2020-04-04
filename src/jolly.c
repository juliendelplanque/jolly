#include "vm.h"
#include "memory.h"
#include "primitives.h"

#include <stdlib.h>
#include <stdio.h>

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