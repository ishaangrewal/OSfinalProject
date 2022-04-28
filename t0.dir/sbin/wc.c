#include "libc.h"


// function to display the number of lines, words, characters, and the name of a file 
int main(int argc, char** argv) {
    printf("*** wc implementation\n");
    // get the file to search from the command line
    char* file_name = argv[1];
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
    int line_count = 0;
    int word_count = 0;
    int char_count = 0;
    for (int i = 0; i < file_length; i+=(j + 1)) {
        // get the length of the word
        j = 0;
        while (i+j < file_length && (file_contents[i+j] != ' ' && file_contents[i+j] != '\n')) {
            j++;
        }
        //check if the search string is anywhere in the word and if so print the word
        if (file_contents[i + j] == '\n') {
            line_count++;
            word_count++;
        }
        else if (file_contents[i + j] == ' ') {
            word_count++;
        }
        char_count += j;
    }
    // print the results
    printf("%d %d %d %s\n", line_count, word_count, char_count, file_name);
    // free the file contents
    free(file_contents);

    return 0;
}
    