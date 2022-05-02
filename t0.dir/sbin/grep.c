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
    char *input = argv[1];
    //find the first word in the input
    int i = 0;
    while (input[i] != ' ' && input[i] != '\0') {
        i++;
    }
    char *first_word = malloc(i + 1);
    int j = 0;
    while (j < i) {
        first_word[j] = input[j];
        j++;
    }
    i++;
    first_word[j] = '\0';
    printf("first_word: %s\n", first_word);
    //find the second word in the input
    int k = 0;
    while (input[i + k] != ' ' && input[i + k] != '\0') {
        // printf("k: %d, input = %c\n", k);
        k++;
    }
    char *second_word = malloc(k + 1);
    int l = 0;
    while (l < k) {
        second_word[l] = input[i + l];
        l++;
    }
    second_word[l] = '\0';
    printf("second_word: %s\n", second_word);
    // open the file
    int fd = open(second_word, 0);
    // get the length of the file
    int file_length = len(fd);
    // allocate memory for the file
    char* file_contents = malloc(file_length);
    // read the file into the buffer
    read(fd, file_contents, file_length);
    // close the file
    close(fd);
    // loop through the file words
    int m = 0;
    for (int n = 0; n < file_length; n+=(m + 1)) {
        // get the length of the word
        m = 0;
        while (n+m < file_length && (file_contents[n+m] != ' ' && file_contents[n+m] != '\n')) {
            m++;
        }
        //check if the search string is anywhere in the word and if so print the word
        if (isSubstring(first_word, file_contents+n, m)) {
            write(1, file_contents+n, m);
            write(1, "\n", 1);
        }


    }
    // free the file contents
    free(file_contents);

    return 0;
}
