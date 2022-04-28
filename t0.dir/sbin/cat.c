#include "libc.h"
int main(int argc, char** argv) {
    printf("*** cat implementation\n");
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], 0);
        char *buff = malloc(len(fd));
        read(fd, buff, len(fd));
        write(1, buff, len(fd));
        free(buff);
    }

    return 0;
}
