#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sem.c"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_SHM 10
#define SIZE_SHM 4096

int main() {
    int shm_id = shmget(100, NUM_SHM * SIZE_SHM, IPC_CREAT|0666);
    int sem_id = create_Sem(100, 2);
    set_N(sem_id, 0, NUM_SHM); // #buffer that is empty
    set_N(sem_id, 1, 0); // #buffer that is full

    int pid1, pid2;
    if ((pid1 = fork()) == 0) {
        execv("writeBuf", NULL);
        exit(0);
    }
    if ((pid2 = fork()) == 0) {
        execv("readBuf", NULL);
        exit(0);
    }

    int status;
    wait(&status);
    wait(&status);
    destroy_Sem(sem_id);
    status = shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}