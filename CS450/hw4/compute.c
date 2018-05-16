/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - OWEN HANDEL */
#include "header.h"

#define SetBit(A, k)(A[(k / 32)] |= (1 << (k % 32)))
#define TestBit(A, k)((A[k / 32] & (1 << (k % 32))) != 0)

st_memory * shared;
pid_t pid;
st_message message;
int msgid;

int main(int argc, char * argv[]) {

    int N;

    N = atoi(argv[1]);
   
    int id;

    id = shmget(MEMKEY, sizeof(st_memory), 0);

    printf("Attached to segment: %d\n", id);

    if (id < 0) {
        printf("Error\n");
    }

    shared = (st_memory * ) shmat(id, NULL, 0);

    pid = getpid();

    int index = 0;
    for (int i = 0; i < 20; i++) {
        if (shared -> process[i].pid == 0) {
            shared -> process[i].pid = pid;
            index = i;
            break;
        }
    }

    int i;
    int j;
    int sum = 0;
    for (i = N; i <= (sizeof(shared -> bitmap) * 8); i++) {
        if(i == (sizeof(shared -> bitmap) * 8)){
        break;
        }
        if (TestBit(shared -> bitmap, i) == 1) {
            shared -> process[index].num_skipped++;
            continue;
        }
        SetBit(shared -> bitmap, i);
        shared -> process[index].num_tested++;
        for (j = 1; j < i; j++) {
            if (i % j == 0) {
                sum += j;
            }
        }
        if (sum == i && i != 0) {
            msgid = msgget(MSGKEY, IPC_CREAT | 0666);
            message.msgtype = 1;
            message.perfect = i;
            if (msgsnd(msgid, (void * ) &message, sizeof(message.perfect), 0) < 0) {
                perror("msgsnd");
                exit(1);
            }

            shared -> process[index].num_perfect++;
        }
        sum = 0;
    }
    return 0;
}