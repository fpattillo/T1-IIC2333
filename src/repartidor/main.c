#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include "../file_manager/manager.h"

int indice_repartidor;
int semaforos[3] = {1,1,1};
int stats[4] = {-1,-1,-1,-1};

void file_write()
{
  char filename[20];
  sprintf(filename, "repartidor_%i.txt", indice_repartidor);
  FILE *output = fopen(filename, "w");

  for (int i = 0; i < 4; i++)
  {
    fprintf(output, "%i", stats[i]);
    // No agregamos el separador al último número
    if (i + 1 != 4)
      fprintf(output, ",");
  }

  // Se cierra el archivo (si no hay leak)
  fclose(output);

  // Terminamos el programa con exit code 0
  exit(0);
}

void handle_abort(int signum)
{
  file_write();
}

void rep_handle_sigusr1(int signum, siginfo_t *siginfo, void *context)
{
  int number_received = siginfo->si_value.sival_int;
  if (number_received < 0)
  {
    semaforos[abs(number_received) - 1] = 0;
  }
  else if (number_received > 0)
  {
    semaforos[abs(number_received) - 1] = 1;
  }
}

int main(int argc, char const *argv[])
{
  printf("REPARTIDOR PID %i\n", getpid());
  connect_sigaction(SIGUSR1, rep_handle_sigusr1);
  signal(SIGABRT, handle_abort);
  signal(SIGINT, child_handle_sigint);
  indice_repartidor = getpid();
  int posicion = 0;
  int turno = 0;
  int distancia_semaforo1 = atoi(argv[1]);
  int distancia_semaforo2 = atoi(argv[2]);
  int distancia_semaforo3 = atoi(argv[3]);
  int distancia_bodega = atoi(argv[4]);
  while (posicion < distancia_bodega)
  {
    sleep(1);
    turno++;
    if ( (posicion + 1) == distancia_semaforo1 )
    {
      if (semaforos[0] == 1)
      {
        posicion++; //avanza
        stats[0] = turno; //se registra lo que se demoro
        printf("REPARTIDOR %i AVANZA A %i (S1)\n", getpid(), posicion);
      }
      else
      {
        printf("REPARTIDOR %i ESPERA S1\n", getpid());
      }
    }
    else if ( (posicion + 1) == distancia_semaforo2 )
    {
      if (semaforos[1] == 1)
      {
        posicion++;
        stats[1] = turno;
        printf("REPARTIDOR %i AVANZA A %i (S2)\n", getpid(), posicion);
      }
      else
      {
        printf("REPARTIDOR %i ESPERA S2\n", getpid());
      }
    }
    else if ( (posicion + 1) == distancia_semaforo3 )
    {
      if (semaforos[2] == 1)
      {
        posicion++;
        stats[2] = turno;
        printf("REPARTIDOR %i AVANZA A %i (S3)\n", getpid(), posicion);
      }
      else
      {
        printf("REPARTIDOR %i ESPERA S3\n", getpid());
      }
    }
    else if ( (posicion + 1) == distancia_bodega )
    {
      posicion++;
      stats[3] = turno;
      printf("REPARTIDOR %i AVANZA A %i (BODEGA)\n", getpid(), posicion);
      file_write();
    }
    else
    {
      posicion++;
      printf("REPARTIDOR %i AVANZA A %i\n", getpid(), posicion);
    }
  }
}
