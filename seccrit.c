#include <sys/sem.h>
#include <stdlib.h>
#define N 20
#define RUTINAS_SHMEM

double factorial(int n){
   if(n<0) return(-1);
   if(n==0) return(1);
   return (n*factorial(n-1));
   }


/*
* shmem.c
*
* Fichero con funciones de creaciÆn de memoria compartida y varias
* de utilidad.
*/




/* Crea memoria compartida.
*    - el manejador de memoria es interno
*    - manda mensajes de error por salida de error estÄndar.
*/
int crearMemoria() {
   char *funcName = "crearMemoria";
   if ((shmid=shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE))<0) {
      fprintf(stderr, "%s: error de shmget()\n", funcName);
   } else if ((memoria=shmat(shmid, 0, 0)) == (void *) -1) {
      fprintf(stderr, "%s: error de shmat()\n", funcName);
   } else {
      return TRUE;
   }
   return FALSE;
}

/* Destruye la memoria compartida creada por crearMemoria()
*/
int eliminarMemoria() {
   char *funcName = "eliminarMemoria";
   if (shmctl(shmid, IPC_RMID, 0) < 0) {
      fprintf(stderr,"%s: error de shmctl()\n", funcName);
      return FALSE ;
   } else
      return TRUE ;
}


/* Coloca una semilla en el temporizador del bucle de
*  tiempoPasa()
*/
void origenTiempo(){
   srand((unsigned int) time(NULL)) ;
}

/* Rutina que hace pasar un poco de tiempo con un bucle
*  sencillo
*/
void tiempoPasa() {
   unsigned int i;
   int a=3;

   /* Los parametros "50" y "2" dependen mucho de la velocidad
   *  de la computadora y de la configuracion del SO. Espero que
   *  funcionen bien
   */
   for (i=rand()/50; i>0; i--)
      a = a%3 + i;
}

double main() {
   key_t claveMutex;
   int Mutex;
   key_t claveLleno;
   int Lleno;
   key_t claveVacio;
   int Vacio; 
   int *finProc =NULL;
   int salida;
   int entrada;
   int *array   =NULL;
   int elemento;
   int total=0;
   int total1=0;
   double media=0;
   double mediaprod=0;


   if ((key_t) -1 == (claveMutex = ftok("ppal", 's')) ||
       (key_t) -1 == (claveLleno = ftok("ppal", 'w')) ||
       (key_t) -1 == (claveVacio = ftok("ppal", 'j'))) {
      fprintf(stderr,"main: Error al crear la clave con ftok(3)\n");
      exit(1);
   }

   if (-1 ==(Mutex = semCreate(claveMutex,1))){
      fprintf(stderr,"main: No puede crear el semaforo\n");
      exit(1);
   }

   if (-1 ==(Lleno = semCreate(claveLleno,0))){
      fprintf(stderr,"main: No puede crear el semaforo\n");
      semClose(Mutex);
      exit(1);
   }

   if (-1 ==(Vacio = semCreate(claveVacio,N))){
      fprintf(stderr,"main: No puede crear el semaforo\n");
      semClose(Mutex);
      semClose(Lleno);
      exit(1);
   }

   if (!crearMemoria()) {
      fprintf(stderr," error de crearMemoria\n");
      semClose(Mutex);
      semClose(Lleno);
      semClose(Vacio);
      exit(1);
   }

   array    = (int*) memoria;
   finProc  = array    + sizeof(int)*N ;

   salida   = 0 ;
   entrada  = 0 ;
   *finProc  = 0 ;

   if (0!=fork()) {
      int i;
      int indprod;
         for (i=0; i< 1000; i++) {
         elemento = rand();/*producir*/
         semWait(Vacio);
         semWait(Mutex);
                 
           array[entrada]=elemento;
        
         semSignal(Mutex);
         semSignal(Lleno);
         entrada=(entrada+1)%N;
         indprod+=1;
         total+=elemento;
      }

      mediaprod=total/indprod;
      printf("La media elementos producidos es:%lf\n",mediaprod);
      
      while (! *finProc) ;
      if(!eliminarMemoria())
         fprintf(stderr,"error de eliminarMemoria\n");
      semClose(Mutex);
      semClose(Lleno);
      semClose(Vacio);
      exit(0);
   } else {  
      int i;
      int indice=0;
     

      if (-1 == (Mutex = semOpen(claveMutex)))
         fprintf(stderr," No tengo el cualificador de Mutex\n");
      if (-1 == (Lleno = semOpen(claveLleno)))
         fprintf(stderr," No tengo el cualificador de Lleno\n");
      if (-1 == (Vacio = semOpen(claveVacio)))
         fprintf(stderr," No tengo el cualificador de Vacio\n");
 

      for (i=0; i< 1000; i++) {
         semWait(Lleno);
         semWait(Mutex);

           elemento=array[salida];
         
         semSignal(Mutex);
         semSignal(Vacio);
         salida=(salida+1)%N;

         
         indice+=1;
         total1+=elemento;
      }
      media=total1/indice;
      printf("La media de los elementos consumidos es:%lf\n",media);

      semClose(Mutex);
      semClose(Lleno);
      semClose(Vacio);
      *finProc = 1;
      exit(0);
   }
  
}