#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
  printf("I'm the SEMAFORO %s process, my time is %s and my PID is: %i\n", argv[1], argv[2], getpid());
}
