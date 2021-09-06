#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "../file_manager/manager.h"

// semaforos parten en verde (rojo = 0, verde = 1)
int estado_semaforo1 = 1;
int estado_semaforo2 = 1;
int estado_semaforo3 = 1;
pid_t* array_semaforos;
int term_repartidores = 0;

void handle_sigusr1(int signum, siginfo_t *siginfo, void *context)
{
  int number_received = siginfo->si_value.sival_int;
  int response = number_received;
  if (number_received == 1 || number_received == 2 || number_received == 3)
  {
    if (number_received == 1)
    {
      estado_semaforo1 = - (estado_semaforo1 - 1); // si es 0 da 1, si es 1 da 0
      if (estado_semaforo1 == 0)
      {
        response *= -1;
      }
    }
    else if (number_received == 2)
    {
      estado_semaforo2 = - (estado_semaforo2 - 1);
      if (estado_semaforo2 == 0)
      {
        response *= -1;
      }
    }
    else if (number_received == 3)
    {
      estado_semaforo3 = - (estado_semaforo3 - 1);
      if (estado_semaforo3 == 0)
      {
        response *= -1;
      }
    }
    for (int i = 0; i < rep_count; i++)
    {
      send_signal_with_int(array_repartidores[i], response); //puede ser 1, 2, 3, -1, -2 o -3. indica semaforo y estado
    }
  }
  else if (number_received != -1)
  {
    printf("AAAAAA PID %i\n", number_received);
    waitpid(number_received, 0, 0);
    term_repartidores++;
    printf("TERMINADOS %i\n", term_repartidores);
  }
}

void main_abort_handler(int signum)
{
  for (int i = 0; i < 3; i++)
  {
    kill(array_semaforos[i], SIGABRT);
  }
}

void factory_handle_abort(int signum)
{
  for (int i = 0; i < rep_count; i++)
  {
    kill(array_repartidores[i], SIGABRT);
  }
  exit(0);
}

int main(int argc, char const *argv[])
{
  printf("I'm the DCCUBER process and my PID is: %i\n", getpid());
  signal(SIGINT, parent_handle_sigint);
  signal(SIGABRT, main_abort_handler);
  char *filename = argv[1];
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

  pid_t pid_fabrica = fork();
  if (pid_fabrica == 0){
    printf("Inicio fabrica\n");
    signal(SIGALRM, alarm_handler);
    signal(SIGABRT, factory_handle_abort);
    signal(SIGINT, child_handle_sigint);
    connect_sigaction(SIGUSR1, handle_sigusr1);
    /// crear repartidores en intervalo de tiempo
    rep_count = 0;
    int counter = 0;
    array_repartidores = malloc(sizeof(pid_t)* atoi(data_in -> lines[1][1]));
    while(counter < atoi(data_in->lines[1][1])){
      counter++;
      printf("COUNTER %i\n", counter);
      alarm(atoi(data_in->lines[1][0]));
      sleep(atoi(data_in->lines[1][0]));
    }
    while (term_repartidores < atoi(data_in->lines[1][1]));
    printf("fin fabrica\n");
    free(array_repartidores);
  } else {
    //wait(NULL);
  }

  if (pid_fabrica > 0)
  {
    array_semaforos = malloc(sizeof(pid_t)* 3);
    int s_counter = 0;
    while(s_counter < 3){
      s_counter++;
      pid_t pid_semaforo = fork();
      if (pid_semaforo == 0){
        char id_semaforo[2];
        sprintf(id_semaforo, "%i", s_counter);
        char str_pid_fabrica[2];
        sprintf(str_pid_fabrica, "%i", pid_fabrica);
        char* tiempo_semaforo = data_in->lines[1][s_counter + 1];
        char* args[] = { "./semaforo", id_semaforo, tiempo_semaforo, str_pid_fabrica, NULL };
        execvp(args[0], args);
      } else {
        array_semaforos[s_counter - 1] = pid_semaforo;
      }
    }
    waitpid(pid_fabrica, 0, 0);
    kill(array_semaforos[0], SIGABRT);
    kill(array_semaforos[1], SIGABRT);
    kill(array_semaforos[2], SIGABRT);
    waitpid(array_semaforos[0], 0, 0);
    waitpid(array_semaforos[1], 0, 0);
    waitpid(array_semaforos[2], 0, 0);
    free(array_semaforos);
    printf("Liberando memoria... PROCESO %d\n", getpid());
    input_file_destroy(data_in);
  }
  
}
