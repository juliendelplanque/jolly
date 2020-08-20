#include "vm.h"
#include "memory.h"
#include "primitives.h"
#include "log.h"

#define ENABLE_LOGGING

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv){
    struct virtual_machine *jolly;
    char *image_file_name;

    log_set_level(LOG_ERROR);

    if(argc != 2){
        fprintf(stderr, "Incorrect number of arguments. Need to specify image file to run, aborting.");
        exit(-1);
    }

    image_file_name = argv[1];

    if(new_vm(&jolly) != VM_OK){
        fprintf(stderr, "Failed to create VM, aborting.\n");
        exit(-1);
    }
    if(load_image(jolly, image_file_name) != VM_OK){
        fprintf(stderr, "Failed to load VM memory from file, aborting.\n");
        exit(-1);
    }
    load_pc(jolly);
    log_debug("Loaded PC=0x%06X", get_pc_address(jolly));
    
    run(jolly);

    free_vm(jolly);
    return 0;
}
