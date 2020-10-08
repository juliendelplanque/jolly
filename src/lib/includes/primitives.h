#ifndef PRIMITIVES_H

#define PRIMITIVES_H

#include "memory.h"
#include "vm.h"

/**
 * Error codes.
 */
#define PRIMITIVE_OK 0 // Code returned when everything went fine.

/**
 * Constants to encode/decode primitive information in memory.
 */
#define PRIMITIVE_ID_NOPE 0
#define PRIMITIVE_ID_FAIL 1
#define PRIMITIVE_ID_PUT_CHAR 2
#define PRIMITIVE_ID_GET_CHAR 3
#define PRIMITIVE_ID_STOP_VM 4
#define PRIMITIVE_ID_OPEN_FILE 5
#define PRIMITIVE_ID_CLOSE_FILE 6
#define PRIMITIVE_ID_IS_FILE_OPEN 7
#define PRIMITIVE_ID_ARGC 8
#define PRIMITIVE_ID_ARGV_SIZE_AT_INDEX 9
#define PRIMITIVE_ID_ARGV 10
#define PRIMITIVE_ID_ADD_ADDRESSES 11
#define PRIMITIVE_ID_SUBSTRACT_ADDRESSES 12
#define PRIMITIVE_ID_DECREMENT_ADDRESS 13
#define PRIMITIVE_ID_INCREMENT_ADDRESS 14

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
 * 
 * If no character can be read from the stream, the primitive fails.
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

/**
 * A primitive that open a new filestream for the file path provided as argument
 * and with the open mode provided as argument.
 * 
 * Reads the byte pointed by the result pointer, the value stored there is
 * the open mode of the file.
 * Currently, this value can be one of the following:
 * - PRIMITIVE_FILE_MODE_READ
 * - PRIMITIVE_FILE_MODE_WRITE
 * - PRIMITIVE_FILE_MODE_APPEND
 * 
 * Then, reads the null-terminated ASCII string directly consecutive to the open
 * mode byte in memory.
 * This string encodes the path of the file to open.
 * 
 * If the primitive succeed, the id of the stream open is written in the byte
 * pointed by the result pointer.
 * This stream id will be needed by primitives that read from or write bytes to
 * file streams.
 */
void primitive_open_file(struct virtual_machine *vm);

/**
 * A primitive that close the filestream for id provided as argument.
 * 
 * Reads the byte pointed by the result pointer, the value stored there is
 * the id of the filestream.
 */
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
 * A primitive that retrieve the number of arguments passed to the VM when it
 * was invoked from the command line.
 * 
 * Stores the arguments count (argc) in the byte pointed by the result pointer.
 */
void primitive_argc(struct virtual_machine *vm);

/**
 * A primitive that retrieve the size of the arguments string at a specific
 * index that was passed to the VM when it was invoked from the command line.
 * 
 * Stores the argument null-terminated string size (argc) in the 3 consecutive
 * bytes pointed by the result pointer.
 */
void primitive_argv_size_at_index(struct virtual_machine *vm);

/**
 * A primitive that retrieve the arguments passed to the VM when it was invoked
 * from the command line one by one according to the index provided.
 * 
 * Reads the byte pointed by the result pointer, the char stored there
 * is the index of the argument string to retrieve.
 * Reads the 3 bytes address located at [result pointer + 1, result pointer+3],
 * the address stored there points to the location where the argument string
 * should be stored.
 * 
 * Stores the retrieved argument as a null-terminated string in consecutive
 * memory starting at the byte pointed by the address read previously.
 */
void primitive_argv(struct virtual_machine *vm);

/**
 * A primitive that adds 2 addresses (3 bytes each) together and stores the
 * result and the carry.
 * 
 * Reads the 6 consecutive bytes in memory pointed by the result pointer and
 * consider the 3 first bytes as the first address and the 3 next as the second
 * address.
 * 
 * Stores the result of the addition in the 3 first bytes pointed by the
 * result pointer (thus overwriting the first address!) and stores either there
 * is a carry (1) or not (0) in the following byte (thus overwriting part of the
 * second address!).
 */
void primitive_add_addresses(struct virtual_machine *vm);

void primitive_substract_addresses(struct virtual_machine *vm);

void primitive_decrement_address(struct virtual_machine *vm);

void primitive_increment_address(struct virtual_machine *vm);

/**
 * Execute an extended primitive. The code of the primitive to execute is stored
 * in the 2 first bytes pointed by PRIMITIVE_RESULT pointer.
 * The code of the extended primitive is encoded with big-endian convention.
 */
void primitive_extended(struct virtual_machine *vm);

#endif
