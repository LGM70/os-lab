#include <stdio.h>
#include "sem.c"
#include <pthread.h>

#define TICKETS 100
#define PROCS 3

const int sem_key = 10000;
int sem_id;
int ticketsRemain = TICKETS; // initialize

void* subp(void* index) {
    int cnt = 0; // total number of tickets this process sells
    while (ticketsRemain) {
        P(sem_id, 0);
        if (ticketsRemain) { // if there's no tickets remaining, do not sell
            ticketsRemain--;
            printf("thread %d sells a ticket.\n", *(int*)index);
            cnt++;
        }
        V(sem_id, 0);
    }
    printf("thread %d sells %d tickets in total.\n", *(int*)index, cnt);
    return NULL;
}

int main() {
    sem_id = create_Sem(sem_key, 1); // creat semaphore
    set_N(sem_id, 0, 1);

    pthread_t thread[PROCS];
    int index[PROCS];
    for (int i = 0; i < PROCS; i++) {
        index[i] = i;
        pthread_create(&thread[i], NULL, &subp, &index[i]); // creat subprocesses
    }

    for (int i = 0; i < PROCS; i++) {
        pthread_join(thread[i], NULL); // wait subprocesses
    }

    destroy_Sem(sem_id); // destroy semaphore
    return 0;
}