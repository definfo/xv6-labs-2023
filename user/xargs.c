#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXARG 14

char* readline()
{
    char* buf = malloc(512);
    char* p = buf;
    while (read(0, p, 1) == 1)
    {
        switch (*p)
        {
        case '\0':
        case '\n':
            *p = '\0'; // End of string
            return buf;
        default:
            p++;
            break;
        }
    }
    if (p != buf) return buf;
    free(buf);
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs [command] ...\n");
        exit(1);
    }

    char* args_buf[MAXARG];

    char** ptr_xargs = args_buf;
    for (int i = 1; i < argc; i++)
    {
        *ptr_xargs++ = argv[i];
    }

    char* l;
    // Read line
    while ((l = readline()) != 0)
    {
        char* ptr_l = l;
        char* buf = malloc(512);
        char* ptr_buf = buf;
        int arg_cnt = argc - 1;

        // Parse arguments from line
        while (*ptr_l != 0)
        {
            if (*ptr_l == ' ' && buf != ptr_buf)
            {
                *ptr_buf = 0;
                args_buf[arg_cnt++] = buf;
                buf = malloc(512);
                ptr_buf = buf;
            }
            else
            {
                *ptr_buf++ = *ptr_l;
            }
            ptr_l++;
        }
        if (buf != ptr_buf)
        {
            args_buf[arg_cnt++] = buf;
        }
        args_buf[arg_cnt] = 0;
        free(l);

        if (fork() == 0)
        {
            exec(args_buf[0], args_buf);
        }
        else
        {
            wait(0);
        }
    }
    exit(0);
}
