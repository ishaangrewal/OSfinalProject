#include "libc.h"

/*
    * cd: change directory
    *
    * Usage: cd [dir]
    *
    * Changes the current directory to dir.
    * If dir is not specified, changes to the home directory.
    *
    * If the directory does not exist, prints an error message.

    need to store current working directory in a global variable in ext2 or syshandler somewhere
    need to change directory in the global variable
    */

int main(int argc, char** argv) {
    printf("*** cd implementation\n");
    printf("Usage: cd %s\n", ((char**)argv[1])[0]);
    //get the new directory name from the command line
    char *new_dir_name = argv[2];
    printf("new_dir_name: %s\n", new_dir_name);
    cd(new_dir_name);
    return 0;
}