#include "libc.h"
/*
    This test case checks open, close, read, write, seek, and shutdown.
    Makes sure you can't open a non existant file or put it into your fd table.
    Checks if the fd is put into the correct spot after a close on one of the earlier entries.
    Lastly, reads a file and resets the cursor to the start to read it again to ensure cursor is updated correctly.
*/
int main(int argc, char** argv) {
    puts("*** hello, user");
    int check = 0;
    char* print_ptr = malloc(100);
    int fd = open("/fake/fake.txt",0);
    if (fd > 0) {
        check = 1;
        puts("*** Shouldn't open non existant file!");
    } 
    int fd2 = open("/etc/data.txt", 0);
    int fd3 = open("/etc/test.txt", 0);
    ssize_t n = read(fd3, print_ptr, 27);
    while (n > 0) {
        ssize_t m = write(1, print_ptr, 27);
        if (m < 0) {
            printf("*** write error\n");
            break;
        }
        n -= m;
        print_ptr += m;
    }
    seek(fd3, 0);
    n = read(fd3, print_ptr, 27);
    while (n > 0) {
        ssize_t m = write(1, print_ptr, 27);
        if (m < 0) {
            printf("*** write error\n");
            break;
        }
        n -= m;
        print_ptr += m;
    }
    close(fd2);
    int fd4 = open("/data/panic.txt", 0);
    if (fd4 != 3) {
        check = 1;
        puts("*** fd4 put into wrong spot!\n");
    }
    if (check == 0) {
        puts("*** passed all checks!");
    }
    shutdown();
    puts("*** Your program should shut down before this runs!");
    return 0;
}
