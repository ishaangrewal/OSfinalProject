#include "libc.h"

int main(int argc, char **argv)
{
    char* path = malloc(100);
    char* command = malloc(100);
    memcpy(path, "/sbin/", 6);
    // printf("exec implementation\n");
    uint32_t argNum = 0;
    //uint32_t argNumCounter = 0;
    uint32_t input = 6;
    char* arg = (char *) argv[1];
    while ((char) arg[argNum] != ' ' && (char) arg[argNum] != '\0') {
        path[input] = arg[argNum];
        command[argNum] = arg[argNum];
        argNum++;
        input++;

    }
    path[input] = '\0';
    command[argNum] = '\0';
    // printf("Path: %s\n", (char*) path);
    // printf("Arg Num: %ld\n", argNum);
    // printf("Command: %s\n", (char *) command);

    if ((char) arg[argNum] == '\0') {
        return execl(path, command, 0);
    }
    return execl(path, command, argv[1] + argNum + 1, 0);

    //return 0;
}
