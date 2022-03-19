#include <stdio.h>
#include "sem.c"
#include <pthread.h>

const int sem_key = 10000;
int sem_id;

int a = 0;

void* cal() {
    for (int i = 1; i <= 100; i++) {
        P(sem_id, 0);
        a += i;
        if (a % 2) { // odd
            V(sem_id, 1);
        }
        else { // even
            V(sem_id, 2);
        }
    }
    return NULL;
}

void* printOdd() {
    for (int i = 1; i <= 50; i++) {
        P(sem_id, 1);
        printf("printOdd  prints: sum is %d\n", a);
        V(sem_id, 0);
    }
    return NULL;
}

void* printEven() {
    for (int i = 1; i<= 50; i++) {
        P(sem_id, 2);
        printf("printEven prints: sum is %d\n", a);
        V(sem_id, 0);
    }
    return NULL;
}

int main() {
    sem_id = create_Sem(sem_key, 3);
    set_N(sem_id, 0, 1); // buffer is empty
    set_N(sem_id, 1, 0); // buffer is full and a is odd
    set_N(sem_id, 2, 0); // buffer is full and a is even

    pthread_t thread[3];
    pthread_create(&thread[0], NULL, &cal, NULL); // creat subprocesses
    pthread_create(&thread[1], NULL, &printOdd, NULL);
    pthread_create(&thread[2], NULL, &printEven, NULL);

    pthread_join(thread[0], NULL); // wait subprocesses
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);

    destroy_Sem(sem_id);
    return 0;
}