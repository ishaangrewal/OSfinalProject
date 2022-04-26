#include "libc.h"

/*
input:
    -keyboard input
    -file inputs
    -

foreground and background processes:
    -shell could remain foreground process instead of waiting on child process
    -if we go this route we can have multiple processes called


Device detection:
    -in flags for runquemu
    -keyboard input:
        -remove nographic add vgastd to rumqemu/debugqemu
        -(might work on physical lab machines)

    -keyboard events triggered keyboard handler
    -utilize handlere (sys) to compare
    -need to get inputs from system
    -check to see each millisecond if there was a key pressed (add to event queue)


pipelining options:
    - write out to stdout is fine
    - collab with other team with write
    - if we have time implement write
*/
int main(int argc, char** argv) {
    printf("*** - - - Welcome to our shell\n");
    printf("*** - - - Good Luck\n");
    while (1) // replace with shell exit check
    {
        int id = fork();
        if (id < 0) {
            printf("fork failed");
        } else if (id == 0) {
            /* ls */
            printf("*** ls implementation\n");
            exit(0);
            //int rc = execl("/sbin/shell","shell","a","b","c",0);
            //printf("*** execl failed, rc = %d\n",rc);
        } else {
            /* parent */
            uint32_t status = 42;
            wait(id,&status);
            printf("*** back to shell\n");
        }
        id = fork();
        if (id < 0) {
            printf("fork failed");
        } else if (id == 0) {
            /* ls */
            printf("*** cd implementation\n");
            exit(0);
            //int rc = execl("/sbin/shell","shell","a","b","c",0);
            //printf("*** execl failed, rc = %d\n",rc);
        } else {
            /* parent */
            uint32_t status = 42;
            wait(id,&status);
            printf("*** back to shell\n");
        }
        id = fork();
        if (id < 0) {
            printf("fork failed");
        } else if (id == 0) {
            /* ls */
            printf("*** pwd implementation\n");
            exit(0);
            //int rc = execl("/sbin/shell","shell","a","b","c",0);
            //printf("*** execl failed, rc = %d\n",rc);
        } else {
            /* parent */
            uint32_t status = 42;
            wait(id,&status);
            printf("*** back to shell\n");
        }
        shutdown();
    }
}
