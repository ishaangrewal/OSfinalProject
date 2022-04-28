#include "libc.h"
// ls bash implementation
int main(int argc, char** argv) {
    printf("*** ls implementation\n");
    // open the directory passed by the console
    int fd = open(argv[1], 0);
    // get the length of the directory
    int dir_length = dirlen(fd);
    // allocate memory for the directory
    char* dir_contents = malloc(dir_length);
    
    // read the directory into the buffer
    readdir(fd, dir_contents);

    // close the directory
    close(fd);
    //print out the contents of the directory
    write(1, dir_contents, dir_length);
    // free the directory contents
    free(dir_contents);

    return 0;
}