#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MINEIROS 10
#define CHANCE 33

int picaretas = 3;
int total_picaretas = 3;

pthread_mutex_t mutex_picaretas = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_total_picaretas = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mineiros = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_ferreiro = PTHREAD_COND_INITIALIZER;

void * mineiro(void* id);
void * ferreiro();

void main(int argc, char* argv[]) {
    int erro;
    int *id;

    pthread_t tm[MINEIROS];
   
    for (int i = 0; i < MINEIROS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&tm[i], NULL, mineiro, (void *) (id));

        if(erro) {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_t tf;
    erro = pthread_create(&tf, NULL, ferreiro, 0);

    if(erro) {
        printf("erro na criacao do thread do ferreiro.\n");
        exit(1);
    }
  
    pthread_join(tf, NULL);
}


void * mineiro(void* id) {
    int m_id =  *(int *)(id);
    int picareta_quebrou;
    srand(m_id);
    while (1) {
        picareta_quebrou = 0;
        pthread_mutex_lock(&mutex_picaretas);
            while (picaretas == 0) {
                pthread_cond_wait(&cond_mineiros, &mutex_picaretas);
            }
            picaretas--;
            printf("Mineiro %d: pegou uma picareta. Picaretas = %d.\n", m_id, picaretas);
        pthread_mutex_unlock(&mutex_picaretas);

        printf("Mineiro %d: trabalhando.\n", m_id);
        sleep(5);
        int r = rand() % 100;
        if (r <= CHANCE) {
            picareta_quebrou = 1;
        }
        pthread_mutex_lock(&mutex_picaretas);
            if (picareta_quebrou == 0){
                picaretas++;
                printf("Mineiro %d: devolveu a picareta. Picaretas = %d. Total = %d.\n", m_id, picaretas, total_picaretas);
                pthread_cond_broadcast(&cond_mineiros);
            } else {
                pthread_mutex_lock(&mutex_total_picaretas);
                    total_picaretas--;
                    printf("Mineiro %d: a picareta quebrou. Picaretas = %d. Total = %d.\n", m_id, picaretas, total_picaretas);
                pthread_mutex_unlock(&mutex_total_picaretas);
                pthread_cond_signal(&cond_ferreiro);
            }
        pthread_mutex_unlock(&mutex_picaretas);
        printf("Mineiro %d: dormindo.\n", m_id);
        sleep(5);
    }
}

void * ferreiro() {
    while (1){
        pthread_mutex_lock(&mutex_total_picaretas);
            while (total_picaretas == MINEIROS) {
                printf("Ferreiro: dormindo.\n");
                pthread_cond_wait(&cond_ferreiro, &mutex_total_picaretas);
            }
        pthread_mutex_unlock(&mutex_total_picaretas);
        
        printf("Ferreio: trabalhando.\n");
        sleep(10);

        pthread_mutex_lock(&mutex_total_picaretas);
            total_picaretas++;
            pthread_mutex_lock(&mutex_picaretas);
                picaretas++;
                printf("Ferreiro: produziu uma picareta. Picaretas = %d. Total = %d.\n", picaretas, total_picaretas);
            pthread_mutex_unlock(&mutex_picaretas);
            pthread_cond_broadcast(&cond_mineiros);
        pthread_mutex_unlock(&mutex_total_picaretas);
    }
}