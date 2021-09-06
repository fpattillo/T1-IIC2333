// Tells the compiler to compile this file once
#pragma once

// Include signal for the declaration of the helper functions
#include <signal.h>

// Define compile-time constants
#define MAX_SPLIT 255
#define BUFFER_SIZE 4096
#define FILE_SIZE 2

// Define the struct
typedef struct inputfile
{
  int len;
  char ***lines; // This is an array of arrays of strings
} InputFile;

// Declare functions
InputFile *read_file(char *filename);
void input_file_destroy(InputFile *input_file);
void connect_sigaction(int sig, void (*handler)(int, siginfo_t *, void *));
void send_signal_with_int(int pid, int payload);
void alarm_handler(int signum);
void handle_sigusr1(int signum, siginfo_t *siginfo, void *context);
void child_handle_sigint(int signum);
void parent_handle_sigint(int signum);

pid_t* array_repartidores;
int rep_count;
int total_reps;
int pid_fabrica;
int pid_s1;
int pid_s2;
int pid_s3;