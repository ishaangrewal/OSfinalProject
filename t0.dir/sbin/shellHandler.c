#include "libc.h"

int main(int argc, char **argv) {
    printf("*** other shellHandler\n");
    char *input = argv[1];
    int id = fork();
    if (id < 0) {
        printf("fork failed\n");
        return -1;
    } else if (id == 0) {
        /* child */
        // execl each command based on the input
        if (input[0] == 'c' && input[1] == 'd') {
            execl("/sbin/cd", "cd", input+3, 0);
        } else if (input[0] == 'c' && input[1] == 'a' && input[2] == 't') {
            // how to find number of arguments
            execl("/sbin/cat", "cat", input+4, 0);
        } else if (input[0] == 'g' && input[1] == 'r' && input[2] == 'e' && input[3] == 'p') {
            execl("/sbin/grep", "grep", input+5, 0);
        } else if (input[0] == 'w' && input[1] == 'c') {
            execl("/sbin/wc", "wc", input+3, 0);
        } else if (input[0] == 'l' && input[1] == 's') {
            if (input[2] == ' ') {
                execl("/sbin/ls", "ls", input+3, 0);
            } else {
                execl("/sbin/ls", "ls", 0);
            }
        } else if (input[0] == 'p' && input[1] == 'w' && input[2] == 'd') {
            execl("/sbin/pwd", "pwd", input + 4, 0);
        } else if (input[0] == 'e' && input[1] == 'c' && input[2] == 'h' && input[3] == 'o') {
            execl("/sbin/echo", "echo", input + 5, 0);
        } else if (input[0] == 'e' && input[1] == 'x' && input[2] == 'e' && input[3] == 'c') {
            execl("/sbin/exec", "exec", input + 5, 0);
        } else {
            printf("*** invalid command\n");
        }
    }
     else {
        /* parent */
        uint32_t status = 42;
        wait(id,&status);
        printf("*** back from wait %ld\n",status);
    }
    return 0;
}
