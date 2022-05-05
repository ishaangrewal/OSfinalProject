#include "libc.h"
//implement strcmp
int strcmp(const char *s1, const char *s2) {
    while (*s1 == *s2) {
        if (*s1 == '\0') {
            return 0;
        }
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

int main(int argc, char** argv) {
    // printf("*** cat implementation\n");
    // get string from command line
    char* file_names = argv[1];
    // for each filename in file_names open the file and print it to the console
    int i = 0;
    while (file_names[i] != '\0') {
        //get the name of the file to open  terminated by a space
        int j = 0;
        while (file_names[i+j] != ' ' && file_names[i+j] != '\0') {
            j++;
        }
        char * file_name = malloc(j+1);
        for (int k = 0; k < j; k++) {
            file_name[k] = file_names[i+k];
        }
        file_name[j] = '\0';
        // open the file
        //if the file name is ., .., or lost+found then skip it
        if (strcmp(file_name, ".") == 0 || strcmp(file_name, "..") == 0 || strcmp(file_name, "lost+found") == 0) {
            if (file_names[i+j] == ' ') {
                i += j + 1;
            } else {
                i += j;
            }
            continue;
        }
        int fd = open(file_name, 0);
        // get the length of the file
        int file_length = len(fd);
        // allocate memory for the file
        char* file_contents = malloc(file_length);
        // read the file into the buffer
        read(fd, file_contents, file_length);
        // close the file
        close(fd);
        // print the file contents
        write(1, file_contents, file_length);
        // free the file contents
        free(file_contents);
        // move to the next filename
        if (file_names[i+j] == ' ') {
            i += j + 1;
        } else {
            i += j;
        }

    }
    // for (int i = 1; i < argc; i++) {
    //     int fd = open(argv[i], 0);
    //     char *buff = malloc(len(fd));
    //     read(fd, buff, len(fd));
    //     write(1, buff, len(fd));
    //     free(buff);
    // }

    return 0;
}
