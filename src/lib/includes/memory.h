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

#define NULL_MEMORY (WORD *) NULL

typedef unsigned char WORD;

#endif
