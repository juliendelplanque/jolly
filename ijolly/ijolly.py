#!/usr/bin/env python3
import os
import sys
import cmd
import readline

from jollypy import *

class JollyShell(cmd.Cmd):
    intro = 'Welcome to interactive jolly. Type help or ? to list commands.\n'
    prompt = '(ijolly) '

    def __init__(self, completekey='tab', stdin=None, stdout=None):
        super().__init__(completekey, stdin, stdout)
        self.vm = None
        self.int_print_strategy = lambda x, padding : print(str(x))

    def parse_int(self, string):
        if string == "pc":
            return self.vm.get_pc_address()
        elif string.startswith('0x'):
            return int(string[2:], 16)
        else:
            return int(string)

    def parse_args(self, arg):
        """ Convert a series of zero or more numbers to an argument tuple.
        """
        args_list = arg.split()
        return tuple(map(lambda x: self.parse_int(x), args_list))

    def exit(self):
        print("Bye!")
        sys.exit(0)

    def do_EOF(self, arg):
        self.exit()

    def do_dec(self, arg):
        """Sets integer printing to decimal mode.
        """
        self.int_print_strategy = lambda x, padding : print(str(x))
        print("Switched integer printing to decimal.")
    
    def do_hex(self, arg):
        """Sets integer printing to hexadecimal mode.
        """
        self.int_print_strategy = lambda x, padding : print("{0:#0{1}x}".format(x, padding))
        print("Switched integer printing to hexadecimal.")

    def do_next(self, arg):
        """Execute a number of instructions specified by the argument: NEXT 10 (by default execute a single instruction).
        """
        self.jolly_next(*self.parse_args(arg))

    def do_primready(self, arg):
        print(self.vm.is_primitive_ready())
    
    def do_prim(self, arg):
        print(self.vm.primitive_call_id())

    def do_nextprim(self, arg):
        self.jolly_nextprim(*self.parse_args(arg))

    def do_pc(self, arg):
        self.jolly_print_pc()

    def do_load(self, arg):
        self.jolly_load(arg)

    def do_readb(self, arg):
        self.jolly_read_byte(*self.parse_args(arg))

    def help_readb(self):
        print("Reads the byte at address provided as argument.\n\n" \
            + "Synopsis: readb address\n\n" \
            + "Example: readb 0x09002F")

    def do_reada(self, arg):
        self.jolly_read_address(*self.parse_args(arg))
    
    def help_reada(self):
        print("Reads the address (3 bytes) at address provided as argument.\n\n" \
            + "Synopsis: reada address\n\n" \
            + "Example: reada 0x09002F")

    def do_readi(self, arg):
        self.jolly_read_instruction(*self.parse_args(arg))
    
    def help_readi(self):
        print("Reads the instruction (9 bytes) at address provided as argument.\n\n" \
            + "Synopsis: readi address\n\n" \
            + "Example: readi 0x09002F")

    def do_writeb(self, arg):
        self.jolly_write_byte(*self.parse_args(arg))

    def do_writea(self, arg):
        self.jolly_write_address(*self.parse_args(arg))

    def do_writei(self, arg):
        self.jolly_write_instruction(*self.parse_args(arg))

    def do_hexdump(self, arg):
        self.jolly_hexdump(*self.parse_args(arg))

    def help_hexdump(self):
        print("Prints an hexdump of VM memory starting at the closest inferior multiple of 16.\n" \
            + "If number of lines is not specified, prints 16 lines (16 * 16 bytes).\n\n" \
            + "Synopsis: hexdump address [lines]\n\n" \
            + "Example: hexdump 0x09002F 32")

    def jolly_next(self, count=1):
        for _ in range(count):
            self.vm.execute_instruction()

    def jolly_print_pc(self):
        self.int_print_strategy(self.vm.get_pc_address(), 8)

    def jolly_load(self, filename):
        self.vm.load_from_file(filename)

    def jolly_read_byte(self, address):
        self.int_print_strategy(self.vm.memory[address], 4)
    
    def jolly_read_address(self, address):
        self.int_print_strategy(self.vm.memory.get_address(address), 8)
    
    def jolly_read_instruction(self, address):
        instruction = self.vm.memory.get_instruction(address)
        self.int_print_strategy(instruction.from_add, 8)
        self.int_print_strategy(instruction.to_add, 8)
        self.int_print_strategy(instruction.jmp_add, 8)

    def jolly_write_byte(self, address, byte):
        self.vm.memory[address] = byte
    
    def jolly_write_address(self, address, value):
        self.vm.memory.set_address(address, value)
    
    def jolly_write_instruction(self, address, from_add, to_add, jmp_add):
        self.vm.memory.set_address(address, from_add)
        self.vm.memory.set_address(address+3, to_add)
        self.vm.memory.set_address(address+6, jmp_add)
    
    def jolly_hexdump(self, start_address_wanted, lines_count=16):
        first_address = start_address_wanted - (start_address_wanted % 16)
        for line in range(lines_count):
            first_line_address = first_address + line * 16
            print("{0:0{1}x}".format(first_line_address, 8), end='')
            print(" ", end='')
            for byte_address in range(first_line_address, first_line_address + 16):
                if byte_address == first_line_address + 8:
                    print(" ", end='')
                print("{0:0{1}x}".format(self.vm.memory[byte_address], 2), end=' ')
            print("|", end='')
            for byte_address in range(first_line_address, first_line_address + 16):
                if self.vm.memory[byte_address] >= 0x21 and self.vm.memory[byte_address] <= 0x7e:
                    b = bytearray(1)
                    b[0] = self.vm.memory[byte_address]
                    print(b.decode("ascii"), end='')
                else:
                    print(".", end='')
            print("|", end='')
            print("")

    def jolly_nextprim(self, primitive_id=None):
        self.vm.execute_until_primitive_ready(primitive_id)

if __name__ == '__main__':
    ffi, lib = build_ffi(os.path.join(os.path.dirname(__file__), '..'))

    vm = JollyVM(ffi, lib)

    vm.load_from_file(os.path.join(os.path.dirname(__file__), '..', 'images' , 'echo.jolly'))

    # memory = JollyMemory(ffi, lib)
    # memory.set_pc_address(0x424242)
    # memory.store_instruction(JollyInstruction(0x424242, 0x434242, 0x434243, 0x434244))
    # vm.set_memory(memory)

    # print(hex(vm.get_pc_address()))
    # vm.execute_instruction()
    # print(hex(vm.get_pc_address()))

    shell = JollyShell()
    shell.vm = vm
    shell.cmdloop()