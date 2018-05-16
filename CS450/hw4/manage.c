/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - OWEN HANDEL */
#include "header.h"

st_memory * shared;
int id;
int msgid;


void terminate(int signum) {
    int check;
    int i = 0;
    id = shmget(MEMKEY, sizeof(st_memory), 0);
    if (id < 0) {
        printf("Error\n");
        exit(1);
    }
    shared = (st_memory * ) shmat(id, NULL, 0);
    printf("Shutdown signal recieved\n");
    for (i = 0; i < 20; i++) {
        if (shared -> process[i].pid != 0) {
            printf("Process ID: %d\n", shared -> process[i].pid);
            kill(shared -> process[i].pid, SIGKILL);
            shared -> process[i].pid = 0;
        } else {
            break;
        }
    }
    sleep(5);
    printf("\nstarting shutdown\n");
    shmctl(id, IPC_RMID, NULL);
    msgctl(msgid, IPC_RMID, NULL);
    shmdt(shared);
    printf("Shutdown complete\n");
    printf("Program finished press enter");
    exit(0);
}

void init() {
    shared -> manage_pid = getpid();
    printf("\nShared memory init\n");
}

int main() {
    int index = 0;
    int alive = 1;
    st_message message;
    struct sigaction sa;

    sa.sa_handler = terminate;
    sigemptyset( &sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);

    id = shmget(MEMKEY, sizeof(st_memory), IPC_CREAT | IPC_EXCL | 0666);

    if (id < 0) {
        printf("error getting the segment id %s\n", strerror(errno));
    }

    shared = (st_memory * ) shmat(id, NULL, 0);

    init();

    printf("segment ID:%d\n", id);

    while (alive != 0) {
        msgid = msgget(MSGKEY, 0);
        if (msgrcv(msgid, &message, sizeof(message.perfect), 1, 0) < 0) {
            continue;
        }
        shared -> perfectnums[index] = message.perfect;
        index++;
        continue;
    }
    return 0;
}