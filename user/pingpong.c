#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // Pipe read / write file descriptors
  int p[2];

  // Create pipe
  pipe(p);

  // Child process (r -> w)
  if(fork() == 0){
    // Buffer
    char buf[1];
    // PID
    int cpid = getpid();
    // Reassign p read end to stdin
    // p[0] -- stdout
    close(0);
    dup(p[0]);
    // 2. Read from p
    // Status: 11
    // Check if successfully read from p
    if(read(p[0], buf, 1) != 1){
      fprintf(2, "child read error\n");
      exit(1);
    }
    // Close read end of p
    // (For later usage of write end)
    // Status: 01
    // x -- stdout
    // (No need to redirect stdout)
    // (Simply write into p)
    close(p[0]);
    fprintf(1, "%d: received ping\n", cpid);
    // 3. Write into p
    // Block until 4
    // Status: 01
    // Check if successfully write into p
    if(write(p[1], buf, 1) != 1){
      fprintf(2, "child write error\n");
      exit(1);
    }
    // Close write end of p
    // (For later usage of read end in parent process)
    // Status: 00
    close(p[1]);
    // Remember to exit child process
    exit(0);
  }
  // Parent process (w -> r)
  else {
    // Buffer
    char buf[1];
    // PID
    int ppid = getpid();
    // Byte to send
    char *b = "z";
    // 1. Write into p
    // Status: 11
    // Block until 2
    // Check if successfully write into p
    // Should not use `sizeof b` instead of `1` !!!
    if(write(p[1], b, 1) != 1){
      fprintf(2, "parent write error\n");
      exit(1);
    }
    // Close write end of p
    // (For later usage of read end in parent process)
    // Status: 10
    close(p[1]);
    // Wait for child to return
    // Ensure release of p after read & write in child process
    wait(0);
    // 4. Read from p
    // Check if successfully read from p
    if(read(p[0], buf, 1) != 1){
      fprintf(2, "parent read error\n");
      exit(1);
    }
    // Close read end of p
    // Status: 00
    close(p[0]);
    // Remember to exit parent process
    fprintf(1, "%d: received pong\n", ppid);
    exit(0);
  }
}
