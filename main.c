#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MINEIROS_OURO 15
#define MINEIROS_DIAMANTE 15

int picaretas = 10;
int m_ouro_quer = 0;
int m_diamante_quer = 0;

pthread_mutex_t mutex_picaretas = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_mineiro_ouro = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_mineiro_diamante = PTHREAD_COND_INITIALIZER;

void * mineiroOuro(void* id);
void * mineiroDiamante(void* id);

void main(int argc, char* argv[]) {
    int erro;
    int *id;

    pthread_t tmo[MINEIROS_OURO];
    
    // Criar threads dos mineiradores de ouro
    for (int i = 0; i < MINEIROS_OURO; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&tmo[i], NULL, mineiroOuro, (void *) (id));

        if(erro) {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }

    pthread_t tmd[MINEIROS_DIAMANTE];

    // Criar threads dos mineiradores de diamante
    for (int i = 0; i < MINEIROS_DIAMANTE; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        erro = pthread_create(&tmd[i], NULL, mineiroDiamante, (void *) (id));

        if(erro) {
            printf("erro na criacao do thread %d\n", i);
            exit(1);
        }
    }
    
    pthread_join(tmo[0], NULL);
}


void * mineiroOuro(void* id) {
    int m_id =  *(int *)(id);

    while (1) {
        pthread_mutex_lock(&mutex_picaretas);
            m_ouro_quer++;
            while (picaretas == 0 || m_diamante_quer >= MINEIROS_DIAMANTE / 2) {
                pthread_cond_wait(&cond_mineiro_ouro, &mutex_picaretas);
            }
            m_ouro_quer--;
            picaretas--;
            printf("Mineiro de Ouro %d: pegou uma picareta. Picaretas = %d.\n", m_id, picaretas);
        pthread_mutex_unlock(&mutex_picaretas);

        printf("Mineiro de Ouro %d: trabalhando.\n", m_id);
        sleep(5);

        pthread_mutex_lock(&mutex_picaretas);
            picaretas++;
            printf("Mineiro de Ouro %d: devolveu a picareta. Picaretas = %d.\n", m_id, picaretas);
            pthread_cond_broadcast(&cond_mineiro_ouro);
            pthread_cond_broadcast(&cond_mineiro_diamante);
        pthread_mutex_unlock(&mutex_picaretas);
        printf("Mineiro de Ouro %d: dormindo.\n", m_id);
        sleep(10);
    }
}

void * mineiroDiamante(void* id) {
    int m_id =  *(int *)(id);

    while (1){
        pthread_mutex_lock(&mutex_picaretas);
            m_diamante_quer++;
            while (picaretas == 0) {
                pthread_cond_wait(&cond_mineiro_diamante, &mutex_picaretas);
            }
            m_diamante_quer--;
            picaretas--;
            printf("Mineiro de Diamante %d: pegou uma picareta. Picaretas = %d.\n", m_id, picaretas);
        pthread_mutex_unlock(&mutex_picaretas);
        
        printf("Mineiro de Diamante %d: trabalhando.\n", m_id);
        sleep(5);

        pthread_mutex_lock(&mutex_picaretas);
            picaretas++;
            printf("Mineiro de Diamante %d: devolveu a picareta. Picaretas = %d.\n", m_id, picaretas);
            pthread_cond_broadcast(&cond_mineiro_ouro);
            pthread_cond_broadcast(&cond_mineiro_diamante);
        pthread_mutex_unlock(&mutex_picaretas);
        printf("Mineiro de Diamante %d: dormindo.\n", m_id);
        sleep(10);
    }
}

