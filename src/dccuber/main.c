#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../file_manager/manager.h"

int semaforos[3] = {0,0,0};

//void handle_sigalrm(int sig)
//{
  //printf("entro a handler\n");
//}

void handle_sigusr1(int sig, siginfo_t *siginfo, void *context){
  //recibe señal de la fabrica y actualiza status de los semaforos
  int number_received = siginfo->si_value.sival_int;
  printf("Fabrica: Recibi semaforo %i\n", number_received);
  // preguntar como lo hacemos
  semaforos[number_received/3] = number_received - (number_received/3 * 3);
}

int main(int argc, char const *argv[])
{
  printf("I'm the DCCUBER process and my PID is: %i\n", getpid());

  char *filename = "input.txt";
  InputFile *data_in = read_file(filename);

  printf("Leyendo el archivo %s...\n", filename);
  printf("- Lineas en archivo: %i\n", data_in->len);
  printf("- Contenido del archivo:\n");

  printf("\t- ");
  for (int i = 0; i < 4; i++)
  {
    printf("%s, ", data_in->lines[0][i]);
  }
  printf("\n");

  printf("\t- ");
  for (int i = 0; i < 5; i++)
  {
    printf("%s, ", data_in->lines[1][i]);
  }
  printf("\n");

//////// start codigo tarea /////

  pid_t pid_fabrica = fork();
  if (pid_fabrica == 0){
    printf("Inicio fabrica\n");
    pid_t* array_repartidores = malloc(sizeof(pid_t)* atoi(data_in -> lines[1][1]));
    /// crear repartidores en intervalo de tiempo
    int counter = 0;
    int status = 0;
    pid_t wpid;
    while(counter < atoi(data_in -> lines[1][1])){
      printf("Creando un repartidor \n");
      pid_t pid_repartidor = fork();
      if (pid_repartidor == 0){
        char* args[]={"./repartidor", data_in -> lines[0][0], data_in -> lines[0][1], data_in -> lines[0][2], data_in -> lines[0][3], NULL};
        execv(args[0], args);
      } else {
        array_repartidores[counter] = pid_repartidor;
        setpgid(pid_repartidor, array_repartidores[0]);
        counter++;
        sleep(atoi(data_in->lines[1][0]));
      }
      send_signal_with_int(array_repartidores[0], 8);
    }
    while ((wpid = wait(&status)) > 0);  
    printf("fin fabrica\n");
  } else {
    waitpid(pid_fabrica, 0,0);
    printf("Liberando memoria...\n");
    input_file_destroy(data_in);
  }
}