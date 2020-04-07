#ifndef MEMORY_H

#define MEMORY_H

#define WORD_BIT_MASK 0xFF
#define WORD_SIZE 8
#define DOUBLE_WORD_SIZE (WORD_SIZE * 2)

#define MAX_MEMORY_SIZE 0x1000008

// Memory layout for the VM.
#define PC_HIGH_ADDRESS 0
#define PC_MIDDLE_ADDRESS 1
#define PC_LOW_ADDRESS 2

#define PRIMITIVE_IS_READY_ADDRESS 3
#define PRIMITIVE_CALL_ID_ADDRESS 4
#define PRIMITIVE_RESULT_CODE_ADDRESS 5
#define PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS 6
#define PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS 7
#define PRIMITIVE_RESULT_POINTER_LOW_ADDRESS 8

#define NULL_MEMORY (WORD *) NULL

typedef unsigned char WORD;

#endif