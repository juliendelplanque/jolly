#include <stdio.h>

#define WORD_SIZE 8
#define DOUBLE_WORD_SIZE (WORD_SIZE * 2)
#define INNER_LOOP_ITERATIONS 65536

#define FROM_ADDRESS_LOW_OFFSET 0
#define FROM_ADDRESS_MIDDLE_OFFSET 1
#define FROM_ADDRESS_HIGH_OFFSET 2

#define TO_ADDRESS_LOW_OFFSET 3
#define TO_ADDRESS_MIDDLE_OFFSET 4
#define TO_ADDRESS_HIGH_OFFSET 5

#define JUMP_ADDRESS_LOW_OFFSET 6
#define JUMP_ADDRESS_MIDDLE_OFFSET 7
#define JUMP_ADDRESS_HIGH_OFFSET 8


int inner_loop(unsigned char *memory, unsigned char *pc){
    int i = INNER_LOOP_ITERATIONS;
    do {
        int from_address,to_address,jump_address;
        // Compute address to copy word from.
        from_address = pc[FROM_ADDRESS_LOW_OFFSET] << DOUBLE_WORD_SIZE
            | pc[FROM_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
            | pc[FROM_ADDRESS_HIGH_OFFSET];
        
        // Compute address to copy word to.
        to_address = pc[TO_ADDRESS_LOW_OFFSET] << DOUBLE_WORD_SIZE 
            | pc[TO_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
            | pc[TO_ADDRESS_HIGH_OFFSET];

        // Copy word pointed by from_address to to_address.
        memory[to_address] = memory[from_address];

        // Compute next address for program counter: the jump address.
        jump_address = pc[JUMP_ADDRESS_LOW_OFFSET] << DOUBLE_WORD_SIZE
            | pc[JUMP_ADDRESS_MIDDLE_OFFSET] << WORD_SIZE
            | pc[JUMP_ADDRESS_HIGH_OFFSET];
        
        // Update program counter according to jump_address (absolute jump).
        pc = memory + jump_address;
    } while (--i);
    return 0;
}

int execute_frame(unsigned char *memory){
    unsigned char *pc;
    pc = memory + (memory[2]<<16 | memory[3]<<8 | memory[4]);
    inner_loop(memory, pc);
    return 0;
}

int main (){   
    unsigned char memory[0x1000008];
    return 0;
}
