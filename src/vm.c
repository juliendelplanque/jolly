#include "memory.h"
#include "primitives.h"

#include <stdlib.h>
#include <stdio.h>

#define INNER_LOOP_ITERATIONS 65536

// Offsets to read an instruction.
#define FROM_ADDRESS_HIGH_OFFSET 0
#define FROM_ADDRESS_MIDDLE_OFFSET 1
#define FROM_ADDRESS_LOW_OFFSET 2

#define TO_ADDRESS_HIGH_OFFSET 3
#define TO_ADDRESS_MIDDLE_OFFSET 4
#define TO_ADDRESS_LOW_OFFSET 5

#define JUMP_ADDRESS_HIGH_OFFSET 6
#define JUMP_ADDRESS_MIDDLE_OFFSET 7
#define JUMP_ADDRESS_LOW_OFFSET 8

void print_pc_address(unsigned char *memory, unsigned char *pc){
    printf("pc = 0x%06X\n", ((unsigned)pc)-((unsigned)memory));
}

int inner_loop(unsigned char *memory, unsigned char *pc){
    unsigned int i = INNER_LOOP_ITERATIONS;
    do {
        unsigned int from_address, to_address, jump_address;
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
        print_pc_address(memory, pc);
    } while (--i);
    return 0;
}

int execute_frame(unsigned char *memory){
    unsigned char *pc;
    pc = memory + (memory[2] << DOUBLE_WORD_SIZE
                    | memory[3] << WORD_SIZE
                    | memory[4]);
    inner_loop(memory, pc);
    return 0;
}

int main(){   
    unsigned char *memory;
    memory = (unsigned char *)malloc(MAX_MEMORY_SIZE);
    if(memory == NULL){
        fprintf(stderr, "Malloc failed.\n");
    }
    execute_frame(memory);
    free(memory);
    return 0;
}
