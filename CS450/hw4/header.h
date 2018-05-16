/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - OWEN HANDEL */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#define MEMKEY 63740

#define MSGKEY 04736

typedef struct {
  pid_t pid;
  int num_perfect;
  int num_tested;
  int num_skipped;
}
st_process;

typedef struct {
  int bitmap[1048576];
  int perfectnums[20];
  st_process process[20];
  pid_t manage_pid;
  int num_perfect;
}
st_memory;

typedef struct {
  long msgtype;
  int perfect;
}
st_message;
