#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define CUSTOMERS 35   // Número de clientes
#define TABLE_SIZE 7   // Número de lugares à mesa
#define MIN_EAT 10   // Tempo mínimo de comer (em microsegundos)
#define MAX_EAT 100  // Tempo máximo de comer (em microsegundos)
#define min(a,b) (((a)<(b))?(a):(b))

int eating = 0;
int waiting = 0;
bool must_wait = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *customer(void *arg);

void *customer(void *arg) {
    int customer_id = *(int *)arg;

    sleep(1);
    pthread_mutex_lock(&mutex);

    // Verifica se deve esperar
    while (must_wait) {
        printf("Cliente %d: Sem assentos disponíveis, esperando...\n", customer_id);
        waiting++;
        pthread_cond_wait(&cond, &mutex);
        waiting--;
    }

    // Atribui assento ao cliente
    eating++;
    printf("Cliente %d: Pegou um assento. Assentos restantes: %d\n", customer_id, TABLE_SIZE - eating);
    must_wait = (eating >= TABLE_SIZE);
    pthread_mutex_unlock(&mutex);


    // Almoça
    usleep(rand() % (MAX_EAT - MIN_EAT + 1) + MIN_EAT);

    pthread_mutex_lock(&mutex);

    // Libera assento
    eating--;
    printf("Cliente %d: Liberou um assento. Assentos restantes: %d\n", customer_id, TABLE_SIZE - eating);
    
    // Notifica clientes esperando
    if (eating == 0 && waiting > 0) {
        printf("Todas as cadeiras estão livres, notificando %d clientes esperando.\n", waiting);
        must_wait = false;
        pthread_cond_broadcast(&cond);
                
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t customers[CUSTOMERS];
    int customer_ids[CUSTOMERS];

    srand(time(NULL));

    printf("Sushi abriu! Que venham os clientes!!!\n");

    for (int i = 0; i < CUSTOMERS; i++) {
        customer_ids[i] = i + 1;
        pthread_create(&customers[i], NULL, customer, &customer_ids[i]);
    }

    // Finalizando threads
    for (int i = 0; i < CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    printf("Sushi fechou e atendeu todos os clientes.\n");

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
