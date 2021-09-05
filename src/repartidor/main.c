#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "../file_manager/manager.h"

int semaforos[3] = {0,0,0};
int stats[4] = {0,0,0,0};

int check_in_array(int value, int* array, int len_array){
  for (int x=1; x<= len_array; x++){
    if (value == array[x-1]){
      return x;
    }
  }
  return 0;
}

void handle_sigusr1(int sig, siginfo_t *siginfo, void *context){
  //recibe señal de la fabrica y actualiza status de los semaforos
  int number_received = siginfo->si_value.sival_int;
  printf("Hijo: Recibi %i\n", number_received);
  semaforos[number_received/3] = number_received - (number_received/3 * 3);
}

int main(int argc, char const *argv[])
{
  int array_dis[4] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
  int pos=0;
  int turno=0;
  connect_sigaction(SIGUSR1, handle_sigusr1);
  printf("I'm the REPARTIDOR process and my PID is: %i\n", getpid());
  while(pos < array_dis[3]){ /// poner condicion de meta
  ///recibir señal de estados de semaforos (hacer un if) ///
    if (check_in_array(pos + 1, array_dis, 4)){
      printf("NEXT IS A STOP\n");
      // checkear si el semaforo esta en rojo
    } else if (check_in_array(pos, array_dis, 4)) {
      printf("IN STOP\n");
      stats[check_in_array(pos, array_dis, 4)] = turno;
    }
    pos++;
    turno++;
    sleep(1);
  }
  printf("%i TERMINO \n", getpid());
  ///enviar señal de termino de ruta
}
