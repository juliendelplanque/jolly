#ifndef PRIMITIVES_H

#define PRIMITIVES_H

#include "memory.h"
#include "vm.h"

#define PRIMITIVE_ID_NOPE 0
#define PRIMITIVE_ID_FAIL 1
#define PRIMITIVE_ID_PUT_CHAR 2
#define PRIMITIVE_ID_GET_CHAR 3
#define PRIMITIVE_ID_STOP_VM 4
#define PRIMITIVE_ID_OPEN_FILE 5
#define PRIMITIVE_ID_CLOSE_FILE 6
#define PRIMITIVE_ID_IS_FILE_OPEN 7

#define PRIMITIVE_ID_EXTENDED 255

#define PRIMITIVE_OK_RESULT_CODE 0
#define PRIMITIVE_FAILED_RESULT_CODE 1

#define PRIMITIVE_NOT_READY 0
#define PRIMITIVE_READY 1

#define PRIMITIVE_FILE_STREAM_STDIN 0
#define PRIMITIVE_FILE_STREAM_STDOUT 1
#define PRIMITIVE_FILE_STREAM_STDERR 2

#define PRIMITIVE_FILE_IS_CLOSED 0
#define PRIMITIVE_FILE_IS_OPEN 1

#define PRIMITIVE_FILE_MODE_READ 0
#define PRIMITIVE_FILE_MODE_WRITE 1
#define PRIMITIVE_FILE_MODE_APPEND 2

struct virtual_machine;

int initialize_primitives_data(struct virtual_machine *vm);

int finalize_primitives_data(struct virtual_machine *vm);

/**
 * A primitive that always fails.
 * Writes the PRIMITIVE_FAILED_RESULT_CODE at PRIMITIVE_RESULT_CODE_ADDRESS.
 */
void primitive_fail(struct virtual_machine *vm);

/**
 * A primitive that does nothing.
 */
void primitive_nop(struct virtual_machine *vm);

/**
 * A primitive that get a character from the stream with id provided as
 * argument.
 * 
 * Reads the byte pointed by the result pointer, the char stored there
 * is the id of the stream to read from.
 * Reads the next byte from this stream and stores it in the byte pointed
 * by the result pointer (thus, the id of the input stream is erased!).
 * 
 * The special stream with id PRIMITIVE_FILE_INPUT_STREAM_STDIN is stdin
 * stream.
 */
void primitive_get_char(struct virtual_machine *vm);

/**
 * A primitive that put the character provided as argument on the stream
 * which id is provided as argument.
 * 
 * Reads the byte pointed by the result pointer, the char stored there
 * is the value of the character to put on the stream.
 * Reads the byte pointed by the result pointer incremented of 1, the 
 * char stored there is the id of the stream to write to.
 * 
 * The special stream with id PRIMITIVE_FILE_OUTPUT_STREAM_STDOUT is sdout
 * stream.
 * The special stream with id PRIMITIVE_FILE_OUTPUT_STREAM_STDERR is sderr
 * stream.
 */
void primitive_put_char(struct virtual_machine *vm);

void primitive_open_file(struct virtual_machine *vm);

void primitive_close_file(struct virtual_machine *vm);

/**
 * A primitive that determinate if the file stream with id provided as argument
 * is open or not.
 * 
 * Reads the byte pointed by the result pointer, the char stored there
 * is id of the stream to determinate if open or not.
 * 
 * Stores PRIMITIVE_FILE_IS_CLOSED or PRIMITIVE_FILE_IS_OPEN in the byte pointed
 * by the result pointer (thus, the id of the stream is erased!).
 * 
 */
void primitive_is_file_open(struct virtual_machine *vm);

/**
 * Stops virtual machine execution.
 */
void primitive_stop(struct virtual_machine *vm);

/**
 * Execute an extended primitive. The code of the primitive to execute is stored
 * in the 2 first bytes pointed by PRIMITIVE_RESULT pointer.
 * The code of the extended primitive is encoded with big-endian convention.
 */
void primitive_extended(struct virtual_machine *vm);

#endif
