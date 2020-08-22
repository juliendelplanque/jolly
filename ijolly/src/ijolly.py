#!/usr/bin/env python3
"""ijolly

Usage:
  ijolly.py <file>
  ijolly.py (-h | --help)
  ijolly.py (-v | --version)
"""

import os
import sys
import cmd
import readline
import re
from functools import reduce
from abc import ABCMeta, abstractmethod
from docopt import docopt
from termcolor import colored

from jollypy import *


class IntegerToStringStrategy(object, metaclass=ABCMeta):
    @abstractmethod
    def to_string(self, integer, padding=0, prefix=""):
        pass


class IntegerToHexString(IntegerToStringStrategy):
    def to_string(self, integer, padding=0, prefix="0x"):
        # need to add 2 to padding because '0x' is counted as padding
        return prefix+("{0:0{1}x}".format(integer, padding))


class IntegerToDecimalString(IntegerToStringStrategy):
    def to_string(self, integer, padding=0, prefix=""):
        return "{0:d}".format(integer)


class MemoryWatcher(object):
    def __init__(self, name, address, bytes_count=1):
        self.name = name
        self.address = address
        self.bytes_count = bytes_count

    @property    
    def end_address(self):
        return self.address + self.bytes_count - 1

    def extract(self, memory):
        """ Extracts the list of bytes corresponding to the memory zone watched
            from the VM's memory.
        """
        return memory[self.address:self.end_address + 1]
    
    def extract_memory_to_string(self, interactive_jolly):
        i2s = interactive_jolly.integer_to_string
        return " ".join(i2s(b, prefix="", padding=2) for b in self.extract(interactive_jolly.memory))
       

    def to_user_string(self, interactive_jolly):
        address = interactive_jolly.integer_to_string(self.address, padding=8)
        memory_dump = self.extract_memory_to_string(interactive_jolly)
        return f'"{self.name}" {address}: {memory_dump}'


class Macro(object):
    def __init__(self, name, command):
        self.name = name
        self.command = command
    
    def run(self, shell):
        shell.onecmd(" ".join(map(str, self.command)))


class InteractiveJolly(object):
    def __init__(self, vm):
        self.vm = vm
        self.integer_print_strategy = IntegerToHexString()
        self.watchers = []
        self.macros = []

    def int_print_strategy(self, integer, padding):
        print(self.integer_to_string(integer, padding))
    
    def switch_to_decimal(self):
        self.integer_print_strategy = IntegerToDecimalString()
    
    def switch_to_hexadecimal(self):
        self.integer_print_strategy = IntegerToHexString()

    def integer_to_string(self, *args, **kwargs):
        return self.integer_print_strategy.to_string(*args, **kwargs)
    
    @property
    def memory(self):
        return self.vm.memory

    def next(self, count=1):
        for _ in range(count):
            self.vm.execute_instruction()
    
    @property
    def pc_address(self):
        return self.vm.get_pc_address()

    def print_pc(self):
        self.int_print_strategy(self.pc_address, 8)

    def load(self, filename):
        self.vm.load_from_file(filename)

    def read_byte(self, address):
        self.int_print_strategy(self.vm.memory[address], 4)
    
    def read_address(self, address):
        self.int_print_strategy(self.vm.memory.get_address(address), 8)
    
    def read_instruction(self, address):
        instruction = self.vm.memory.get_instruction(address)
        self.int_print_strategy(instruction.from_add, 8)
        self.int_print_strategy(instruction.to_add, 8)
        self.int_print_strategy(instruction.jmp_add, 8)

    def write_byte(self, address, byte):
        self.vm.memory[address] = byte
    
    def write_address(self, address, value):
        self.vm.memory.set_address(address, value)
    
    def write_instruction(self, address, from_add, to_add, jmp_add):
        self.vm.memory.set_address(address, from_add)
        self.vm.memory.set_address(address+3, to_add)
        self.vm.memory.set_address(address+6, jmp_add)
    
    def hexdump(self, start_address_wanted, lines_count=16):
        first_address = start_address_wanted - (start_address_wanted % 16)
        for line in range(lines_count):
            first_line_address = first_address + line * 16
            print("{0:0{1}x}".format(first_line_address, 8), end='')
            print(" ", end='')
            for byte_address in range(first_line_address, first_line_address + 16):
                if byte_address == first_line_address + 8:
                    print(" ", end='')
                to_print = "{0:0{1}x}".format(self.vm.memory[byte_address], 2)
                if byte_address == start_address_wanted:
                    to_print = colored(to_print, attrs=['underline'])
                print(to_print, end=' ')
            print("|", end='')
            for byte_address in range(first_line_address, first_line_address + 16):
                to_print = None
                if self.vm.memory[byte_address] >= 0x21 and self.vm.memory[byte_address] <= 0x7e:
                    b = bytearray(1)
                    b[0] = self.vm.memory[byte_address]
                    to_print = b.decode("ascii")
                else:
                    to_print = '.'
                if byte_address == start_address_wanted:
                    to_print = colored(to_print, attrs=['underline'])
                print(to_print, end='')
            print("|", end='')
            print("")

    def nextprim(self, primitive_id=None):
        self.vm.execute_until_primitive_ready(primitive_id)

    def next_up_to(self, address):
        while not self.pc_address == address:
            self.next()

    def add_watcher(self, name, address, bytes_count=1):
        self.watchers.append(MemoryWatcher(name, address, bytes_count))

    def has_watchers(self):
        return len(self.watchers) >= 1
    
    def add_macro(self, name, command):
        self.macros.append(Macro(name, command))
    
    def get_macro(self, macro_name):
        for m in self.macros:
            if m.name == macro_name:
                return m
        raise Error("No macro with such name defined.")
    
    def call_macro(self, macro_name, shell):
        self.get_macro(macro_name).run(shell)


welcome_shell_message = \
""",~~_
|/\\ =_ _ ~
 _( )_( )\\~~
 \\,\\  _|\\ \\~~~
    \\`   \\
    `    `
Welcome to interactive jolly.
Type help or ? to list commands.
"""


class JollyShell(cmd.Cmd):
    intro = welcome_shell_message
    prompt = '(ijolly) '

    def __init__(self, ijolly, completekey='tab', stdin=None, stdout=None):
        super().__init__(completekey, stdin, stdout)
        self.ijolly = ijolly

    def parse_input(self, string):
        if string == "pc":
            return self.ijolly.pc_address
        elif re.match("0x[0-9A-Fa-f]+", string):
            return int(string[2:], 16)
        elif re.match("[0-9]+", string):
            return int(string, 10)
        else:
            return string
        

    def parse_args(self, arg):
        """ Convert a series of zero or more inputs to an argument tuple.
        """
        args_list = arg.split()
        return tuple(self.parse_input(x) for x in args_list)

    def exit(self):
        print("Bye!")
        sys.exit(0)

    def do_EOF(self, arg):
        self.exit()

    def do_dec(self, arg):
        """Sets integer printing to decimal mode.
        """
        self.ijolly.switch_to_decimal()
        print("Switched integer printing to decimal.")
    
    def do_hex(self, arg):
        """Sets integer printing to hexadecimal mode.
        """
        self.ijolly.switch_to_hexadecimal()
        print("Switched integer printing to hexadecimal.")

    def do_next(self, arg):
        """Execute a number of instructions specified by the argument: NEXT 10 (by default execute a single instruction).
        """
        self.ijolly.next(*self.parse_args(arg))
        if self.ijolly.has_watchers():
            self.do_watchers()

    def do_primready(self, arg):
        print(self.vm.is_primitive_ready())
    
    def do_prim(self, arg):
        print(self.vm.primitive_call_id())

    def do_nextprim(self, arg):
        self.ijolly.nextprim(*self.parse_args(arg))

    def do_nextupto(self, arg):
        self.ijolly.next_up_to(*self.parse_args(arg))

    def do_pc(self, arg):
        self.ijolly.print_pc()

    def do_load(self, arg):
        self.ijolly.load(arg)

    def do_readb(self, arg):
        self.ijolly.read_byte(*self.parse_args(arg))

    def help_readb(self):
        print("Reads the byte at address provided as argument.\n\n"
              "Synopsis: readb address\n\n"
              "Example: readb 0x09002F")

    def do_reada(self, arg):
        self.ijolly.read_address(*self.parse_args(arg))
    
    def help_reada(self):
        print("Reads the address (3 bytes) at address provided as argument.\n\n"
              "Synopsis: reada address\n\n"
              "Example: reada 0x09002F")
    
    def do_readw(self, arg):
        self.do_reada(arg)
    
    def help_reada(self):
        print("Reads the word (3 bytes) at address provided as argument.\n\n"
              "Synopsis: readw address\n\n"
              "Example: readw 0x09002F")

    def do_readi(self, arg):
        self.ijolly.read_instruction(*self.parse_args(arg))
    
    def help_readi(self):
        print("Reads the instruction (9 bytes) at address provided as argument.\n\n"
              "Synopsis: readi address\n\n"
              "Example: readi 0x09002F")

    def do_writeb(self, arg):
        self.ijolly.write_byte(*self.parse_args(arg))

    def do_writea(self, arg):
        self.ijolly.write_address(*self.parse_args(arg))

    def do_writei(self, arg):
        self.ijolly.write_instruction(*self.parse_args(arg))

    def do_hexdump(self, arg):
        self.ijolly.hexdump(*self.parse_args(arg))

    def help_hexdump(self):
        print("Prints an hexdump of VM memory starting at the closest inferior multiple of 16.\n"
              "If number of lines is not specified, prints 16 lines (16 * 16 bytes).\n\n"
              "Synopsis: hexdump address [lines default: 16]\n\n"
              "Example: hexdump 0x09002F 32")

    def do_watch(self, arg):
        self.ijolly.add_watcher(*self.parse_args(arg))

    def help_watch(self):
        print("Watch the bytes at the address provided as arguments.\n"
              "Once this command is called, the values in memory will be "
              "printed\nafter each user interaction with the execution of the "
              "VM (for example using next command).\n\n"
              "Synopsis: watch watcher_name address [bytes_count default: 1]\n\n"
              "Example: watch varX 0x09002F 3")
    
    def do_watch_vm_vars(self, arg):
        self.do_watch("serialized_pc 0x0 3")
        self.do_watch("primitive_is_ready 0x3 1")
        self.do_watch("primitive_call_id 0x4 1")
        self.do_watch("primitive_result_code 0x5 1")
        self.do_watch("primitive_result_pointer 0x6 3")

    def help_watch_vm_vars(self):
        print("Add watchers for memory zones required by the VM.\n"
              "This command is a shortcut equivalent to the following "
              "sequence of commands:\n"
              "watch serialized_pc 0x0 3\n"
              "watch primitive_is_ready 0x3 1\n"
              "watch primitive_call_id 0x4 1\n"
              "watch primitive_result_code 0x5 1\n"
              "watch primitive_result_pointer 0x6 3")

    def do_watchers(self, arg=""):
        print("Watchers:")
        if self.ijolly.has_watchers():
            for watcher in self.ijolly.watchers:
                print(watcher.to_user_string(self.ijolly))
        else:
            print("No watcher.")
    
    def do_defm(self, arg):
        parsed = self.parse_args(arg)
        self.ijolly.add_macro(parsed[0], parsed[1:])
    
    def do_callm(self, arg):
        self.ijolly.call_macro(*self.parse_args(arg), self)


if __name__ == '__main__':
    arguments = docopt(__doc__, version='ijolly 0.1.0')

    ffi, lib = build_ffi(os.path.join(os.path.dirname(__file__), '..', '..'))
    
    ijolly = InteractiveJolly(JollyVM(ffi, lib))
    
    if arguments["<file>"]:
        ijolly.load(os.path.join(os.path.dirname(__file__), arguments["<file>"]))

    shell = JollyShell(ijolly)
    shell.cmdloop()