#include <kernel/param.h>
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void exec_cmd(const char* cmd, char* args[])
{
    if (fork() == 0)
    {
        exec(cmd, args); // 1
        exit(0);
    }
    else
    {
        wait(0);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs [command] ...");
        exit(1);
    }

    char* args_buf[MAXARG];

    char** ptr_xargs = args_buf;
    for (int i = 2; i < argc; i++)
    {
        printf("%s\n", argv[i]);
        *ptr_xargs++ = argv[i];
    }
    char** ptr_args = ptr_xargs;

    char buf[512];
    char* buf_start = buf;
    char* buf_end = buf;
    while (read(0, buf_start, 1) == 1)
    {
        switch (*buf_start)
        {
        case ' ':
            *buf_start = '\0'; // End of argument
            *ptr_args++ = buf_end; // Add arg into args_buf
            buf_end = buf_start + 1;
        case '\n':
            *ptr_args = 0; // Change line
            exec_cmd(argv[1], args_buf);
            ptr_args = ptr_xargs; // Reset ptr_args
            break;
        default:
            buf_start++;
            break;
        }
    }
    // Last line
    if (ptr_args != ptr_xargs)
    {
        *buf_start = '\0'; // End of argument
        *ptr_args++ = buf_end; // Add arg into args_buf
        *ptr_args = 0; // Change line
        exec_cmd(argv[1], args_buf);
    }
    exit(0);
}
