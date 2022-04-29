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
*/

int main(int argc, char** argv) {
    printf("*** cd implementation\n");
    printf("Usage: cd %s\n", ((char**)argv[1])[0]);
    //get the new directory name from the command line
    char *new_dir_name = argv[2];
    printf("new_dir_name: %s\n", new_dir_name);
    uint32_t index = 0;
    //get to end of the current file name
    while ((((char**)argv[1])[0])[index] != '\0') {
        index++;
    }
    //add the new directory name to the end of the current file name
    if ((((char**)argv[1])[0])[index - 1] != '/') {
        memcpy((((char**)argv[1])[0]) + index, "/", 1);
        index++;
    }
    uint32_t i = 0;
    while (new_dir_name[i] != '\0') {
        memcpy((((char**)argv[1])[0]) + index, &new_dir_name[i], 1);
        // (current_file_name_array[0])[index] = new_dir_name[i];
        index++;
        i++;
    }
    memcpy((((char**)argv[1])[0]) + index, "\0", 1);
    // (current_file_name_array[0])[index] = '\0';
    printf("current_file_name: %s\n", (((char**)argv[1])[0]));
    // memcpy(argv[1], ((char**)argv[1])[0],index + 1);
    // ((char**)argv[1])[0] = current_file_name_array[0];
    return 0;
}