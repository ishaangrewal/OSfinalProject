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
echo
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

/*
    maybe graphics window to type in commands
    maybe graphics window to display output
    multiple modes for graphics: text mode

*/

//function to handle the keyboard input for the shell and run the commands
int mainShellHandler(char* input) {
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
            if (input[3] == '|') {
                printf("ls pipe with command: %s\n", input+5);
                execl("/sbin/ls", "ls", input+5, 0);
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

int main(int argc, char** argv) {
    printf("*** - - - Welcome to our shell\n");
    printf("*** - - - Good Luck\n");
    
    while (1) // replace with shell exit check
    {
        char *input = malloc(100);
        input = "pwd";
        mainShellHandler(input);
        input = "echo /etc/data.txt";
        mainShellHandler(input);
        input = "cat /etc/data.txt";
        mainShellHandler(input);
        input = "ls";
        mainShellHandler(input);
        input = "cd etc";
        mainShellHandler(input);
        input = "pwd";
        mainShellHandler(input);
        input = "echo data.txt";
        mainShellHandler(input);
        input = "cat data.txt";
        mainShellHandler(input);
        input = "ls";
        mainShellHandler(input);
        input = "cd ..";
        mainShellHandler(input);
        input = "pwd";
        mainShellHandler(input);
        input = "grep is /etc/data.txt";
        mainShellHandler(input);
        input = "wc /etc/data.txt";
        mainShellHandler(input);
        input = "exec ls";
        mainShellHandler(input);
        input = "exec cd etc";
        mainShellHandler(input);
        input = "ls | cat";
        mainShellHandler(input);
        input = "ls | echo";
        mainShellHandler(input);



        // int id = fork();
        // if (id < 0) {
        //     printf("fork failed");
        // } else if (id == 0) {
        //     /* ls */
        //     int rc = execl("/sbin/echo","echo","/data/data.txt",0);
        //     exit(0);
        //     printf("*** execl failed, rc = %d\n",rc);
        // } else {
        //     /* parent */
        //     uint32_t status = 42;
        //     wait(id,&status);
        //     printf("*** back to shell\n");
        // }
        // id = fork();
        // if (id < 0) {
        //     printf("fork failed");
        // } else if (id == 0) {
        //     /* ls */
        //     int rc = execl("/sbin/cat","cat","/data/data.txt", "/etc/data.txt", "/data/panic.txt", 0);
        //     printf("*** execl failed, rc = %d\n",rc);
        //     exit(0);
        // } else {
        //     /* parent */
        //     uint32_t status = 42;
        //     wait(id,&status);
        //     printf("*** back to shell\n");
        // }
        // id = fork();
        // if (id < 0) {
        //     printf("fork failed");
        // } else if (id == 0) {
        //     //grep
        //     int rc = execl("/sbin/grep","grep", "i", "/etc/data.txt", 0);
        //     printf("*** execl failed, rc = %d\n",rc);
        //     exit(0);
        // } else {
        //     /* parent */
        //     uint32_t status = 42;
        //     wait(id,&status);
        //     printf("*** back to shell\n");
        // }
        // id = fork();
        // if (id < 0) {
        //     printf("fork failed");
        // } else if (id == 0) {
        //     // wc
        //     int rc = execl("/sbin/wc","wc", "/etc/data.txt", 0);
        //     printf("*** execl failed, rc = %d\n",rc);
        //     exit(0);
        // } else {
        //     /* parent */
        //     uint32_t status = 42;
        //     wait(id,&status);
        //     printf("*** back to shell\n");
        // }
        // id = fork();
        // if (id < 0) {
        //     printf("fork failed");
        // } else if (id == 0) {
        //     // ls
        //     int rc = execl("/sbin/ls","ls", *file, 0);
        //     printf("*** execl failed, rc = %d\n",rc);
        //     exit(0);
        // } else {
        //     /* parent */
        //     uint32_t status = 42;
        //     wait(id,&status);
        //     printf("*** back to shell\n");
        // }
        // id = fork();
        // if (id < 0) {
        //     printf("fork failed");
        // } else if (id == 0) {
        //     // cd
        //     memcpy(file[0], "/data", 5);
        //     int rc = execl("/sbin/cd","cd", file, "etc", 0);
        //     printf("*** execl failed, rc = %d\n",rc);
        //     exit(0);
        // } else {
        //     /* parent */

        //     uint32_t status = 42;
        //     wait(id,&status);
        //     printf("*** back to shell\n");
        //     printf("file: %s\n",*file);
        // }
        shutdown();
    }
}
