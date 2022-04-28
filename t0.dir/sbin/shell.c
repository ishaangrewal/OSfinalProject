#include "libc.h"
/*
command ideas:
cat
grep
sort
wc
ls
cd
pwd
locate?

*/


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
            int rc = execl("/sbin/echo","echo","/data/data.txt",0);
            exit(0);
            printf("*** execl failed, rc = %d\n",rc);
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
            int rc = execl("/sbin/cat","cat","/data/data.txt", "/etc/data.txt", "/data/panic.txt", 0);
            printf("*** execl failed, rc = %d\n",rc);
            exit(0);
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
            //grep
            int rc = execl("/sbin/grep","grep", "i", "/etc/data.txt", 0);
            printf("*** execl failed, rc = %d\n",rc);
            exit(0);
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
            // wc
            int rc = execl("/sbin/wc","wc", "/etc/data.txt", 0);
            printf("*** execl failed, rc = %d\n",rc);
            exit(0);
        } else {
            /* parent */
            uint32_t status = 42;
            wait(id,&status);
            printf("*** back to shell\n");
        }
        shutdown();
    }
}
