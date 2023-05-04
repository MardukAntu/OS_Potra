#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define MSGSIZE 16

int main()
{
    char *msg1 = "hello, world #1";
    char *msg2 = "hello, world #2";
    char *msg3 = "hello, world #3";
    char inbuf[MSGSIZE];
    int p[2], pid2, nbytes;

    if (pipe(p) < 0)
    {
        fprintf(stderr, "pipe failed\n");
        exit(1);
    }

    if ((pid2 = fork()) < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (pid2 > 0)
    {                // parent process
        close(p[0]); // close read end
        write(p[1], msg1, MSGSIZE);
        write(p[1], msg2, MSGSIZE);
        write(p[1], msg3, MSGSIZE);
        close(p[1]); // close write end
        printf("parent %d sent messages\n", getpid());
    }
    else
    {                // child process
        close(p[1]); // close write end
        while ((nbytes = read(p[0], inbuf, MSGSIZE)) > 0)
        {
            printf("child %d received %d bytes: %s\n", getpid(), nbytes, inbuf);
        }
        if (nbytes != 0)
        {
            fprintf(stderr, "read error\n");
            exit(1);
        }
        close(p[0]); // close read end
    }

    return 0;
}
