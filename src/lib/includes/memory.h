#ifndef MEMORY_H

#define MEMORY_H

#define WORD_BIT_MASK 0xFF
#define WORD_SIZE 8
#define DOUBLE_WORD_SIZE (WORD_SIZE * 2)

#define MAX_MEMORY_SIZE 0x1000008

/**
 * Memory layout for the virtual machine.
 */

/**
 * Address of the byte storing the most significant bits of the program counter.
 */
#define PC_HIGH_ADDRESS 0x000000

/**
 * Address of the byte storing the middle bits of the program counter.
 */
#define PC_MIDDLE_ADDRESS 0x000001

/**
 * Address of the byte storing the less significant bits of the program counter.
 */
#define PC_LOW_ADDRESS 0x000002

/**
 * Address of the byte storing a value that triggers primitive execution.
 */
#define PRIMITIVE_IS_READY_ADDRESS 0x000003

/**
 * Address of the byte storing a value that is the id of the primitive to
 * execute.
 */
#define PRIMITIVE_CALL_ID_ADDRESS 0x000004

/**
 * Address of the byte storing a value that is the result code of the last
 * primitive executed by the virtual machine.
 */
#define PRIMITIVE_RESULT_CODE_ADDRESS 0x000005

/**
 * Address of the byte storing the most significant bits of the result pointer.
 */
#define PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS 0x000006

/**
 * Address of the byte storing the middle bits of the result pointer.
 */
#define PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS 0x000007

/**
 * Address of the byte storing the less significant bits of the result pointer.
 */
#define PRIMITIVE_RESULT_POINTER_LOW_ADDRESS 0x000008

/**
 * Jolly is a 3*8-bit WordWordJump machine. It means that the machine uses 3
 * WORD per address and each WORD is 8 bits lengths.
 * The corresponding datatype for 8 bits in C is unsigned char.
 * Thus, we declare the following datatype.
 */
typedef unsigned char WORD;

/**
 * Let us define a null type for the memory of Jolly.
 * This macro just casts NULL pointer into (WORD *).
 */
#define NULL_MEMORY (WORD *) NULL

#endif
