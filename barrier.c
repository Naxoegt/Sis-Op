#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 
#include <time.h>   

// implementacion del monitor

typedef struct {
    int N;             
    int count;         
    int etapa;         
    pthread_mutex_t mutex; 
    pthread_cond_t cond;   
} reusable_barrier_t;

reusable_barrier_t barrier;


//  Inicializa la barrera para sincronizar n hebras.
 
void barrier_init(reusable_barrier_t *b, int num_threads) {
    b->N = num_threads;
    b->count = 0;
    b->etapa = 0;
    // para inicializar el mutex y la variable de condición
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cond, NULL);
}

// para liberar los recursos de la barrera
void barrier_destroy(reusable_barrier_t *b) {
    pthread_mutex_destroy(&b->mutex);
    pthread_cond_destroy(&b->cond);
}


 // bloquea la hebra hasta que todas las n hebras hayan llegado.
  // implementa la lógica de reseteo para la reutilización.
 
void barrier_wait(reusable_barrier_t *b) {
    pthread_mutex_lock(&b->mutex);

    // aqui captura la etapa actual en una variable local 
    int etapa_local = b->etapa;

    b->count++;

    if (b->count == b->N) {
        
        b->etapa++; 
        
        b->count = 0;
        
        pthread_cond_broadcast(&b->cond);
    } else {
        // La hebra no es la última por ende debe esperar.
        // hay que esperar mientras la etapa no cambie. 
        // si el valor de la etapa global es diferente de la etapa local, significa que la barrera ha sido liberada.
        while (b->etapa == etapa_local) {
            pthread_cond_wait(&b->cond, &b->mutex);
        }
    }

    pthread_mutex_unlock(&b->mutex);
}

// ACTIVIDAD 2: APLICACIÓN DE VERIFICACIÓN

#define DEFAULT_N 5 // N hebras por defecto 
#define DEFAULT_E 4 // E etapas por defecto 

// función que ejecuta cada hebra. Simula el trabajo y usa la barrera.
 
void *thread_function(void *arg) {
    long tid = (long)arg; 
    int N_etapas = DEFAULT_E; 

    for (int e = 0; e < N_etapas; e++) {
        // esto simula que las hebras llegan en tiempos distintos.
        usleep(rand() % 100000); 

        printf("[tid %02ld] esperando en etapa %d\n", tid, e);

        barrier_wait(&barrier);

        printf("[tid %02ld] paso barrera en etapa %d\n", tid, e);

        // trabajo simulado entre etapas
        usleep(rand() % 50000); 
    }

    pthread_exit(NULL);
}

// función principal para crear hebras y verificar la barrera.
 
int main(int argc, char *argv[]) {
    // para inicializar generador de números aleatorios una sola vez
    srand(time(NULL)); 
    
    int N = DEFAULT_N;
    int E = DEFAULT_E;

    // crear N hebras 
    if (argc > 1) { N = atoi(argv[1]); }
    if (argc > 2) { E = atoi(argv[2]); }
    
    if (N < 2) { 
        fprintf(stderr, "N debe ser al menos 2 para que la barrera tenga efecto.\n");
        return 1;
    }

    // inicializar la barrera para N hebras
    barrier_init(&barrier, N);

    pthread_t threads[N];
    printf("--- Verificación de Barrera Reutilizable ---\n");
    printf("Iniciando %d hebras para %d etapas...\n", N, E);

    // crear las hebras
    for (long i = 0; i < N; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, (void *)i) != 0) {
            perror("Error al crear hebra");
            return 1;
        }
    }

    // esperar a que todas las hebras terminen
    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("--- Fin de la Verificación ---\n");
    barrier_destroy(&barrier);
    return 0;
}