# Jolly
A ByteByteJump virtual machine inspired by [BytePusher](https://esolangs.org/wiki/BytePusher).

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