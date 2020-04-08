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

## Future

- Multi-threading
- FFI
