#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // $ sleep --> error: not enough arguments
  if(argc <= 1){
    fprintf(2, "Usage: sleep time...\n");
    exit(1);
  }

  // $ sleep `t` ... --> error: too many arguments
  if(argc > 2){
    fprintf(2, "sleep: failed to sleep\n");
    exit(1);
  }
  
  // Convert char* read from command line to int
  int t = atoi(argv[1]);
  // System call to sleep function
  sleep(t);
  // Stop the calling process and release resources
  exit(0);
}