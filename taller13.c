#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX 1000000
pthread_mutex_t mutex_c = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_p = PTHREAD_MUTEX_INITIALIZER;
int tam_cola = 0;
int cont = 0;
//Declaramos una estructura
typedef struct mi_estructuraTDA{
	int conteo;
	char *str;
} estructura;

void * productor(void *arg){
	
	//Intervalos *intervalo = (Intervalos *)arg;


	pthread_mutex_lock(&mutex);
	while(!condicion)
		pthread_cond_wait(&cv, &mutex);
	
	pthread_mutex_unlock(&mutex);

	return (void *)0;
}

void * consumidor(void *arg){
	
	Intervalos *intervalo = (Intervalos *)arg;

	pthread_mutex_lock(&mutex);
	/*código que modifica la condicion*/
	pthread_mutex_unlock(&mutex);
	pthread_cond_broadcast(&cv);

	return (void *)0;
}

int main(int argc, char **argv){

	int cont;
	int status, status2;
	int num_h_c = atoi(argv[2]);
	int num_h_p = atoi(argv[2]);
	tam_cola = atoi(argv[2]);
	int total_items = atoi(argv[2]);
	char *ruta = argv[1];
	pthread_t *th_id_c = (pthread_t *) malloc(num_hilos * sizeof(*thread_id));
	pthread_t *th_id_p = (pthread_t *) malloc(num_hilos * sizeof(*thread_id));
	//printf("%d\n", argc);
	int *cola = (int *) malloc(tam_cola * sizeof(*cola));


	//Productor
	for (int i = 0; i < num_h_p; i++){
		status = pthread_create(&th_id_p[i], NULL, productor, (void *)intervalo);
		if(status < 0){
			fprintf(stderr, "Error al crear el productor %d\n", i);
			exit(-1);	
		}
	}

	//Consumidor
	for (int i = 0; i < num_h_c; i++){
		status2 = pthread_create(&th_id_c[i], NULL, consumidor, (void *)intervalo);
		if(status2 < 0){
			fprintf(stderr, "Error al crear el consumidor %d\n", i);
			exit(-1);	
		}
	}

	
	sleep(1);
	printf("Hilos terminaron normalmente\n");
	exit(0);
	

}
