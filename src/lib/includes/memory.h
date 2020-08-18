#ifndef MEMORY_H

#define MEMORY_H

/**
 * Jolly is a 3*8-bit WordWordJump machine. It means that the machine uses 3
 * WORD per address and each WORD is 8 bits lengths.
 * The corresponding datatype for 8 bits in C is unsigned char.
 * Thus, we declare the following datatype.
 */
typedef unsigned char WORD;

/**
 * The bit mask for a WORD, the 8 bits set to 1.
 */
#define WORD_BIT_MASK 0xFF

/**
 * The number of bits in a WORD, 8.
 */
#define WORD_SIZE 8

/**
 * Twice the number of bits in a WORD.
 */
#define DOUBLE_WORD_SIZE (WORD_SIZE * 2)

/**
 * The maximal size of addressable memory for a Jolly program.
 * As an address is 3 WORDs width (i.e. 24 bits), it can point to a byte located
 * in address range [0, 0x1000000].
 * However, Jolly instructions are made of 3 addresses consecutive in memory.
 * Thus, if one uses the address 0x1000000 in an instruction, Jolly will attempt
 * to read the bystes located in range [0x1000000, 0x1000008].
 * While attempting to read an instruction at this address is a bug in the
 * program, Jolly adds 8 trailing bytes (in addition to the 0x1000000 first
 * ones) to its memory.
 * With those 8 trailing bytes set to 0 and that can not be modified, a buggy
 * program can use address 0x1000000 in one of its instruction. The effect will
 * be that it will crash "safely" within the [0, 0x1000008] addresses range and
 * will not be able to read or write outside the memory allocated for Jolly.
 */
#define MAX_MEMORY_SIZE 0x1000008

/**
 * Let us define a null type for the memory of Jolly.
 * This macro just casts NULL pointer into (WORD *).
 */
#define NULL_MEMORY (WORD *) NULL

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

#endif
