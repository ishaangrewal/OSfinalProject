#include "libc.h"

//function to check if a string is a substring of another string
int isSubstring(char *sub, char *str, int len) {
    int x = 0;
    while (sub[x] != '\0') {
        x++;
    }
    int i, j;
    for (i = 0; i < len; i++) {
        for (j = 0; j < len; j++) {
            if (sub[j] != str[i + j]) {
                break;
            }
        }
        if (j == x) {

            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    printf("*** grep implementation\n");
    //get the string to search for from the command line
    char* search_string = argv[1];
    // get the file to search from the command line
    char* file_name = argv[2];
    // open the file
    int fd = open(file_name, 0);
    // get the length of the file
    int file_length = len(fd);
    // allocate memory for the file
    char* file_contents = malloc(file_length);
    // read the file into the buffer
    read(fd, file_contents, file_length);
    // close the file
    close(fd);
    // loop through the file words
    int j = 0;
    for (int i = 0; i < file_length; i+=(j + 1)) {
        // get the length of the word
        j = 0;
        while (i+j < file_length && (file_contents[i+j] != ' ' && file_contents[i+j] != '\n')) {
            j++;
        }
        //check if the search string is anywhere in the word and if so print the word
        if (isSubstring(search_string, file_contents+i, j)) {
            write(1, file_contents+i, j);
            write(1, "\n", 1);
        }


    }
    // free the file contents
    free(file_contents);

    return 0;
}
