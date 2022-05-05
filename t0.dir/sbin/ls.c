#include "libc.h"
// ls bash implementation
int main(int argc, char** argv) {
    // printf("*** ls implementation\n");
    // check if pipelining is passed in as an argument
    if (argc > 1) {
        // printf("*** piping ls\n");
        char* buff = malloc(250);
        ls(buff);
        // printf("input: %s\n", buff);
        char *secondCommand = argv[1];
        // printf("second command: %s\n", secondCommand);
        char *output = malloc(300);
        // move the second command and the input into the output using a while loop
        int i = 0;
        while (secondCommand[i] != '\0') {
            output[i] = secondCommand[i];
            i++;
        }
        output[i] = ' ';
        i++;
        int j = 0;
        while (buff[j] != '\0') {
            output[i] = buff[j];
            i++;
            j++;
        }
        output[i] = '\0';
        // printf("output: %s\n", output);
        // call the shellHandler with the output
        execl("/sbin/shellHandler", "shellHandler", output, 0);
        free(buff);
        free(output);

    } else {
        // printf("*** no piping ls\n");
        char* buff = malloc(250);
        ls(buff);
        printf("%s\n", buff);
        free(buff);
    }
    return 0;
}