#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  int i, x;

  if(argc != 2){
    exit();
  }

  for(i=0;i<atoi(argv[1]); i++){
    x += i;
  }
  exit();
  return 0;
}
