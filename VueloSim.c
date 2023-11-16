#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    /* code */

    int numAsistentes = atoi(argv[1]);

    //Creacion tecnico del avion
    __pid_t pidTecnico, pidEncargado;

    pidTecnico = fork();

    if (pidTecnico < 0)
    {
        //Entramos en el caso de error 
        printf("Error en el proceso tecnico\n");
        exit(EXIT_FAILURE);

    }else if(pidTecnico == 0){
        //Codigo del proceso tecnico
        printf("Proceso tecnico\n");
        exit(EXIT_SUCCESS);
    }
    

    //Creacion del encargado
    pidEncargado = fork();

    if(pidEncargado < 0){
        printf("Error en el proceso encargado\n");
        //Si falla el proceso encargado matamos el proceso tecnico
        kill(pidTecnico, SIGKILL);
        //Salimos con codigo de error
        exit(EXIT_FAILURE);
    }else{
        //Codigo del proceso encargado
        printf("El proceso encargado es %d\n", pidEncargado);
        exit(EXIT_SUCCESS);
    }









    printf("Hola mundo mi parametro es %d\n", numAsistentes);
    return 0;
}
