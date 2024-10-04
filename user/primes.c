#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const int MAX_NUM = 35;
const int INT_SIZE = 4;

// Redirect stdin/stdout to dst pipe read/write end
void redir(int src, int dst[])
//@requires src == 0 || src == 1;
{
    close(src);
    dup(dst[src]);
    close(dst[0]);
    close(dst[1]);
}

// Send integers to process
void send()
{
    int i;
    for (i = 2; i <= MAX_NUM; i++)
    {
        write(1, &i, INT_SIZE);
    }
}

// Filter integers to child process
void filter(int p)
{
    int n;
    while (read(0, &n, INT_SIZE))
    {
        if (n % p != 0)
        {
            write(1, &n, INT_SIZE);
        }
    }
}

// Sink into child process
void sink()
{
    // Create buf
    int n;
    if (read(0, &n, INT_SIZE))
    {
        // Save and print the first received integer
        printf("prime %d\n", n);

        // Create pipe
        int p[2];
        pipe(p);

        // Child process
        if (fork() == 0)
        {
            redir(1, p);
            filter(n);
        }
        // Parent process
        else
        {
            redir(0, p);
            sink();
        }
    }
}

int main(int argc, char* argv[])
{
    // Create pipe
    int p[2];
    pipe(p);

    // Child process
    if (fork() == 0)
    {
        redir(1, p);
        send();
    }
    // Parent process
    else
    {
        redir(0, p);
        sink();
    }
}
