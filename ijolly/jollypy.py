import os
from cffi import FFI

def memory_h_file(jolly_root):
    return os.path.join(jolly_root, "src/lib/includes/memory.h")

def vm_h_file(jolly_root):
    return os.path.join(jolly_root, "src/lib/includes/vm.h")

def lib_file(jolly_root):
    return os.path.join(jolly_root, "build/src/lib/libjolly.1.dylib")

def clean_source_file(file_content, to_remove):
    cleaned = file_content
    for fragment in to_remove:
        cleaned = cleaned.replace(fragment, "")
    return cleaned

def clean_memory_h_content(file_content):
    to_remove = ["""#ifndef MEMORY_H

#define MEMORY_H""",
    "#endif", "#define NULL_MEMORY (WORD *) NULL"]
    return clean_source_file(file_content, to_remove)

def clean_vm_h_content(file_content):
    to_remove = ["""#ifndef VM_H

#define VM_H

#include "memory.h"
#include "primitives.h"
#include <stdio.h>""",
    "#endif", "#define NULL_VM (struct virtual_machine *) NULL"]
    return clean_source_file(file_content, to_remove)

def build_ffi(jolly_root):
    ffi = FFI()

    with open(memory_h_file(jolly_root)) as f:
        ffi.cdef(clean_memory_h_content(f.read()))

    with open(vm_h_file(jolly_root)) as f:
        ffi.cdef(clean_vm_h_content(f.read()))

    lib = ffi.dlopen(lib_file(jolly_root))
    return ffi, lib

class JollyVM(object):
    def __init__(self, ffi, lib, memory=None):
        self.ffi = ffi
        self.lib = lib
        self.__c_vm_pp = ffi.new("struct virtual_machine **")
        if self.lib.new_vm(self.__c_vm_pp) != self.lib.VM_OK:
            raise Error("Error while instantiating VM.")
        self.memory = memory
    
    def __c_vm_pointer(self):
        return self.__c_vm_pp[0]

    def set_memory(self, memory):
        self.lib.set_memory(self.__c_vm_pointer(), memory._JollyMemory__c_memory_p)
        self.memory = memory
    
    def get_pc_address(self):
        return self.lib.get_pc_address(self.__c_vm_pointer())

    def execute_instruction(self):
        self.lib.execute_instruction(self.__c_vm_pointer())
    
    def execute_instructions(self, count=1):
        for _ in range(count):
            self.execute_instruction()

    def load_pc(self):
        self.lib.load_pc(self.__c_vm_pointer())

    def is_primitive_ready(self):
        return self.lib.is_primitive_ready(self.__c_vm_pointer()) == 1
    
    def primitive_call_id(self):
        return self.lib.get_primitive_call_id(self.__c_vm_pointer())
    
    def execute_until_primitive_ready(self, primitive_id=None):
        if primitive_id:
            while not (self.is_primitive_ready() and self.primitive_call_id() == primitive_id):
                self.execute_instruction()
        else: 
            while not self.is_primitive_ready():
                self.execute_instruction()

    def load_from_file(self, filename):
        self.lib.load_image(self.__c_vm_pointer(), filename.encode("ascii"))
        self.load_pc()
        self.memory = JollyMemory(self.ffi, self.lib, self.__c_vm_pointer().memory)

class JollyInstruction(object):
    def __init__(self, address, from_add, to_add, jmp_add):
        self.address = address
        self.from_add = from_add
        self.to_add = to_add
        self.jmp_add = jmp_add

class JollyMemory(object):
    def __init__(self, ffi, lib, cmemory=None):
        self.ffi = ffi
        self.lib = lib
        if cmemory == None:
            self.__c_memory_p = ffi.new("WORD[MAX_MEMORY_SIZE]")
        else:
            self.__c_memory_p = cmemory

    def __getitem__(self, address):
        return self.__c_memory_p[address]

    def __setitem__(self, address, byte):
        self.__c_memory_p[address] = byte

    def get_address(self, address):
        return (self[address] << 16) \
                |  (self[address+1] << 8) \
                | self[address+2]

    def store_address(self, address, address_to_store):
        self[address] = (address_to_store & 0xFF0000 >> 16)
        self[address+1] = (address_to_store & 0x00FF00 >> 8)
        self[address+2] = (address_to_store & 0x0000FF)

    def set_pc_address(self, pc_address):
        self.store_address(self.lib.PC_HIGH_ADDRESS, pc_address)

    def get_instruction(self, address):
        return JollyInstruction(address, self.get_address(address), self.get_address(address+3), self.get_address(address+6))

    def store_instruction(self, instruction):
        self.store_address(instruction.address, instruction.from_add)
        self.store_address(instruction.address+3, instruction.to_add)
        self.store_address(instruction.address+6, instruction.jmp_add)
