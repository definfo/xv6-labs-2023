#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// Extract filename from path
char* extract_filename(char* path)
{
    char* p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
    {
    }
    p++;
    return p;
}

char* concat_path(const char* path, const char* filename)
{
    int path_len = strlen(path);
    int filename_len = strlen(filename);
    int result_len = path_len + filename_len + 2; // +1 for possible '/', +1 for null terminator

    char* result = malloc(result_len);
    char* p = result;

    // Copy path
    memcpy(p, path, path_len);
    p += path_len;

    // Add a path separator if it's not already there
    if (path_len > 0 && path[path_len - 1] != '/')
    {
        *p++ = '/';
    }

    // Copy filename
    memcpy(p, filename, filename_len);
    p += filename_len;

    // Null terminate
    *p = '\0';

    return result;
}

void find(char* path, char* filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        if (strcmp(extract_filename(path), filename) == 0)
        {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            if (stat(buf, &st) < 0)
            {
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            char* pt = concat_path(path, de.name);
            find(pt, filename);
            free(pt);
        }
        break;
    default:
        fprintf(2, "find: stat type error\n");
        exit(1);
    }
    close(fd);
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find [dir] [filename]\n");
        exit(1);
    }
    for (int i = 2; i < argc; i++)
    {
        find(argv[1], argv[i]);
    }
    exit(0);
}
