#ifndef VM_H

#define VM_H

#include "memory.h"

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

#define NULL_VM (struct virtual_machine *) NULL

// Error codes
#define VM_OK 0
#define VM_MEMORY_UNINITIALIZED 1
#define VM_INVALID_MEMORY 2
#define VM_MEMORY_ALLOCATION_FAILED 3
#define VM_ALLOCATION_FAILED 4

enum vm_status { VIRTUAL_MACHINE_RUN, VIRTUAL_MACHINE_STOP };

struct virtual_machine{
    WORD *memory;
    WORD *pc;
    enum vm_status status;
};

/**
 * Loads the program counter stored in vm's memory into pc field of the vm
 * structure.
 */
int load_pc(struct virtual_machine *vm);

/**
 * Creates a new virtual machine structure with uninitialized memory.
 */
int new_vm(struct virtual_machine **vm);

/**
 * Sets the memory for the virtual machine provided as argument.
 * Loads the program counter serialized in this memory and stores it in
 * the vm structure.
 */
int set_memory(struct virtual_machine* vm, WORD *memory);

/**
 * Allocate memory of correct size for the virtual machine and sets it
 * in the structure.
 */
int create_empty_memory(struct virtual_machine* vm);

/**
 * Frees the virtual machine provided as argument.
 */
void free_vm(struct virtual_machine *vm);

/**
 * Returns an integer being the address of the program counter in Jolly's
 * memory.
 */
int get_pc_address(struct virtual_machine *vm);

/**
 * Sets the program counter of the virtual machine provided as argument
 * to pc_address.
 * Does not modify the memory but needs it to be set before execution.
 */
int set_pc_address(struct virtual_machine *vm, int pc_address);

/**
 * Serializes actual program counter of the virtual machine provided as argument
 * in its memory.
 */
int serialize_pc(struct virtual_machine *vm);

/**
 * Lookup the primitive corresponding to the id stored at address
 * PRIMITIVE_CALL_ID_ADDRESS in memory and executes it.
 * 
 * After execution, overwrite value at PRIMITIVE_CALL_ID_ADDRESS in memory and
 * write PRIMITIVE_ID_NOPE instead.
 */
int execute_primitive(struct virtual_machine *vm);

int execute_instruction(struct virtual_machine *vm);

int run(struct virtual_machine *vm);

int load_image(struct virtual_machine *vm, char *filename);

void print_pc_address(WORD *memory, WORD *pc);

void print_value_at_address(WORD *memory, unsigned int address);

#endif
