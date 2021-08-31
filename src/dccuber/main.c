#include <stdio.h>
#include <unistd.h>

#include "../file_manager/manager.h"

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

  pid_t pid_fabrica = fork();
  if (pid_fabrica == 0){
    printf("Inicio fabrica\n");
    /// crear repartidores en intervalo de tiempo
    int counter = 0;
    while(counter < atoi(data_in->lines[1][1])){
      printf("Creando un repartidor \n");
      counter++;
      printf("%i\n", counter);
      pid_t pid_repartidor = fork();
      if (pid_repartidor == 0){
        char* args[]={"./repartidor", NULL};
        execv(args[0], args);
      } else {
        waitpid(pid_repartidor,0,0);
      }
      sleep(atoi(data_in->lines[1][0]));
    }
    printf("fin fabrica\n");
  } else {
    waitpid(pid_fabrica, 0,0);
  }

  if (pid_fabrica > 0)
  {
    int s_counter = 0;
    while(s_counter < 3){
      printf("Creando un semaforo \n");
      s_counter++;
      pid_t pid_semaforo = fork();
      if (pid_semaforo == 0){
        char id_semaforo[2];
        sprintf(id_semaforo, "%i", s_counter);
        char* tiempo_semaforo = data_in->lines[1][s_counter + 1];
        char* args[] = { "./semaforo", id_semaforo, tiempo_semaforo, NULL };
        execvp(args[0], args);
      } else {
        waitpid(pid_semaforo,0,0);
      }
    }
  }

  if (pid_fabrica > 0)
  {
    printf("Liberando memoria... PROCESO %d\n", getpid());
    input_file_destroy(data_in);
  }
  
}
