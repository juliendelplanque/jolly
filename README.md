# Jolly

A [ByteByteJump](https://esolangs.org/wiki/ByteByteJump) virtual machine inspired by [BytePusher](https://esolangs.org/wiki/BytePusher).

## Building and running

Install the [`check`](https://libcheck.github.io/check/) library, e.g. `brew install check`.

```shell
make test
./jolly images/echo.jolly
``` 

## Demo images
The `demo` folder contains image files that can be executed by Jolly VM.

## Features (early draft)

**/!\ This is a draft of what I want, not everything is supported yet.**

- 3*8-bit [WordWordJump machine](https://esolangs.org/wiki/ByteByteJump) (3 words per address, and 8 bits per word)
- 16 [MiB](https://en.wikipedia.org/wiki/Mebibyte) RAM
- Support for primitives
- Can save memory snapshot from program
- Can stop the VM from program
- Interface with operating system via primitives (e.g. read from stdin / write to stdout/stderr)

## Jolly's memory
The memory of Jolly has a size of `0x1000000` bytes.
It's address range is `[0x000000, 0xFFFFFF]`.

Jolly virtual machine only has a few expectation on how its memory is organized.
For Jolly, only the first `9` bytes of the memory are important.

The following table is a shortcut allowing you to not read [memory.h](src/lib/includes/memory.h).

| Byte address | Name                                    | Description |
|-------------:|:----------------------------------------|:------------|
| 0x000000     | PC_HIGH_ADDRESS                         | Address of the byte storing the most significant bits of the program counter. |
| 0x000001     | PC_MIDDLE_ADDRESS                       | Address of the byte storing the middle bits of the program counter. |
| 0x000002     | PC_LOW_ADDRESS                          | Address of the byte storing the less significant bits of the program counter. |
| 0x000003     | PRIMITIVE_IS_READY_ADDRESS              | Address of the byte storing a value that triggers primitive execution. |
| 0x000004     | PRIMITIVE_CALL_ID_ADDRESS               | Address of the byte storing a value that is the id of the primitive to execute. |
| 0x000005     | PRIMITIVE_RESULT_CODE_ADDRESS           | Address of the byte storing a value that is the result code of the last primitive executed by the virtual machine. |
| 0x000006     | PRIMITIVE_RESULT_POINTER_HIGH_ADDRESS   | Address of the byte storing the most significant bits of the result pointer. |
| 0x000007     | PRIMITIVE_RESULT_POINTER_MIDDLE_ADDRESS | Address of the byte storing the middle bits of the result pointer. |
| 0x000008     | PRIMITIVE_RESULT_POINTER_LOW_ADDRESS    | Address of the byte storing the less significant bits of the result pointer. |

## Future

- Multi-threading
- FFI
