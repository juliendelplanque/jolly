#include "vm.h"
#include "memory.h"
#include "primitives.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv){
    struct virtual_machine *jolly;

    log_set_level(LOG_FATAL);
    if(new_vm(&jolly) != VM_OK){
        fprintf(stderr, "Failed to create VM, aborting.\n");
        exit(-1);
    }
    if(load_image(jolly, "/Users/julien/Documents/Perso/2020/Jolly/jolly/images/hello_world.jolly") != VM_OK){
        fprintf(stderr, "Failed to load VM memory from file, aborting.\n");
        exit(-1);
    }
    load_pc(jolly);
    log_debug("Loaded PC=0x%06X", get_pc_address(jolly));
    // execute_instruction(jolly);
    run(jolly);

    // if(create_empty_memory(jolly) != VM_OK){
    //     fprintf(stderr, "Failed to create VM memory, aborting.\n");
    //     exit(-1);
    // }
    // jolly->memory[PC_HIGH_ADDRESS] = 0x0;
    // jolly->memory[PC_MIDDLE_ADDRESS] = 0x42;
    // jolly->memory[PC_LOW_ADDRESS] = 0x10;

    // jolly->memory[PRIMITIVE_IS_READY_ADDRESS] = PRIMITIVE_READY;
    // jolly->memory[PRIMITIVE_CALL_ID_ADDRESS] = PRIMITIVE_ID_GET_CHAR;
    // jolly->memory[PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS] = 0x00;
    // jolly->memory[PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS] = 0x01;
    // jolly->memory[PRIMITIVE_RESULT_POINTER_LOW_ADDRESS] = 0x00;
    // load_pc(jolly);
    // printf("0x%06X\n", get_pc_address(jolly));
    // print_pc_address(jolly->memory, jolly->pc);
    // execute_instruction(jolly);

    // print_value_at_address(jolly->memory, PRIMITIVE_IS_READY_ADDRESS);
    // print_value_at_address(jolly->memory, 0x000100);
    free_vm(jolly);
    return 0;
}
