#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "sem.c"
#include <sys/types.h>

#define NUM_SHM 10
#define SIZE_SHM 4096

int main() {
    int shm_id = shmget(100, NUM_SHM * SIZE_SHM, IPC_CREAT|0666);
    int sem_id = create_Sem(100, 2);

    int index = 0; // index of bufffer
    char* start = (char*)shmat(shm_id, NULL, SHM_W);
    int cnt = 0;

    while (1) {
        P(sem_id, 1);
        int length = *((int*)(start + index * SIZE_SHM + SIZE_SHM - 4));
        if (length != -1) { // file ends
            for (int i = 0; i < length; i++) {
                putchar(*(start + index * SIZE_SHM + i));
            }
            V(sem_id, 0);
            break;
        }
        for (int i = 0; i < SIZE_SHM - 4; i++) {
            putchar(*(start + index * SIZE_SHM + i));
        }
        index = (index + 1) % NUM_SHM;
        V(sem_id, 0);
        cnt++;
        fprintf(stderr, "read: %d\n", cnt);
    }

    int status = shmdt(start);
    return 0;
}