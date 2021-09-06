#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../file_manager/manager.h"

int cambios = 0;
int semaforo;

void handle_abort(int signum)
{
  char filename[20];
  sprintf(filename, "semaforo_%i.txt", semaforo);
  FILE *output = fopen(filename, "w");

  fprintf(output, "%i", cambios);

  // Se cierra el archivo (si no hay leak)
  fclose(output);

  // Terminamos el programa con exit code 0
  exit(0);
}

int main(int argc, char const *argv[])
{
  printf("SEMAFORO %s DELAY %s FACTORY %s PID %i\n", argv[1], argv[2], argv[3], getpid());
  signal(SIGINT, child_handle_sigint);
  signal(SIGABRT, handle_abort);
  semaforo = atoi(argv[1]);
  int tiempo_cambio_luz = atoi(argv[2]);
  int pid_fabrica = atoi(argv[3]);
  cambios = 0;
  while (1)
  {
    sleep(tiempo_cambio_luz);
    cambios++;
    printf("CAMBIO %i LUZ %i\n", cambios, semaforo);
    send_signal_with_int(pid_fabrica, atoi(argv[1])); // manda senal con identificador del semaforo (1, 2, 3)
  }
}
