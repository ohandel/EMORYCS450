/* THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS - OWEN HANDEL */
#include "header.h"

st_memory * shared;
int id;
char *terminate;

int main(int argc, char * argv[]) {
    int ret;
    if (argc == 2) {
        terminate = (argv[1]);
        if (strcmp(terminate, "-k") == 0) {
            id = shmget(MEMKEY, sizeof(st_memory), 0);
            if (id < 0) {
                printf("Error\n");
                exit(1);
            }
            shared = (st_memory * ) shmat(id, NULL, 0);
            printf("interrupt sent\n");
            ret = kill(shared -> manage_pid, SIGINT);
            if (ret < 0) {
                printf("Error\n");
                exit(1);
            }
            exit(0);
        }
    } else {
        id = shmget(MEMKEY, sizeof(st_memory), 0);
        if (id < 0) {
            printf("Error\n");
            exit(1);
        }

        shared = (st_memory * ) shmat(id, NULL, 0);
        printf("\n");
        
        for (int i = 0; i < 20; i++) {
            if (shared -> process[i].pid != 0) {
                printf("Compute %d Process ID: %d\n", i + 1, shared -> process[i].pid);
                printf("     Number of perfects found: %d\n", shared -> process[i].num_perfect);
                printf("     Number of tested numbers: %d\n", shared -> process[i].num_tested);
                printf("     Number of skipped numbers: %d\n\n", shared -> process[i].num_skipped);
            }
        }
        
        int total_tested = 0;
        int total_skipped = 0;
        int total_found = 0;

        for (int i = 0; i < 20; i++) {
            if (shared -> process[i].pid != 0) {
                total_tested += shared -> process[i].num_tested;
                total_skipped += shared -> process[i].num_skipped;
                total_found += shared -> process[i].num_perfect;
            }
        }
        
        printf("Total number of perfects found: %d\n", total_found);
        printf("Total number of tested numbers: %d\n", total_tested);
        printf("Total number of skipped numbers: %d\n\n", total_skipped);

        printf("Perfect Numbers found: ");
        
        for (int i = 0; i < 20; i++) {
            if (shared -> perfectnums[i + 1] != 0) {
                printf("%d, ", shared -> perfectnums[i]);
            } else if (shared -> perfectnums[i + 1] == 0 && shared -> perfectnums[i] != 0) {
                printf("%d\n\n", shared -> perfectnums[i]);
                break;
            }
        }
    }
    return 0;
}