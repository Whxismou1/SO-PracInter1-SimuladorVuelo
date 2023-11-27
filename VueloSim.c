#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

/*Funcion encargada de manejar la señal SIGUSR1 que recibe el tecnico*/
void tecnicSig();

/*Funcion encargada de manejar la señal SIGUSR2 que reciben los asistentes*/
void assistSig();

/*Funcion encargada de manejar la señal SIGUSR1 que recibe el encargado*/
void respSig();

/*Funcion encargada de comrpobar el error en los fork*/
void errorPid(pid_t pid);

/*Funcion encargada de imprimir el numero de asistente y sus pasajeros*/
void assistantMessage(int numAsistantPasseng, pid_t *numAsistentesArr, int j);

/*Funcion encargada de comprobar si hay overbooking*/
bool exitsOverbook(int overBook);

/*Funcion encargada de imprimir un mensaje final en funcion del exitCode recibido por parametro
y de liberar el array de asistentes*/
void printFinalFree(pid_t *numAsistentesArr, int numPassengers, int exitFinalCode);

/*Funcion encargada de generar aleatorios entre los numeros pasados por parametro*/
int generateRandom(int num1, int num2);

int main(int argc, char const *argv[])
{

    /*Se comprueba que los parametros pasados sean correctos*/
    if (argc != 2)
    {
        printf("ERROR: Num args invalid!\n");
        exit(1);
    }

    // Creacion tecnico del avion
    pid_t pidTecnico;
    pidTecnico = fork();
    // Se comrpueba si hay error en la creacion del tecnico
    errorPid(pidTecnico);

    if (pidTecnico == 0)
    {
        // Entramos en el hijo tecnico
        // Se comprueba si hay error al recibir la señal SIGUSR1 del padre
        if (signal(SIGUSR1, &tecnicSig) == SIG_ERR)
        {
            perror("ERROR: technician signal\n");
        }
        // Esperamos
        pause();
        srand(getpid());
        printf("Technician ready!\n");
        // Duerme entre 3 y 6 segundos
        sleep(generateRandom(3, 6));
        // Finaliza el hijo tecnico con un 1 si el vuelo es viable o un 0 si el vuelo es inviable
        exit(generateRandom(0, 1));
    }
    else
    {
        // Creamos el encargado
        pid_t pidEncargado;
        pidEncargado = fork();
        // Comprobamos si hay error en la creacion del encargado
        errorPid(pidEncargado);
        if (pidEncargado == 0)
        {
            // Estamos en el hijo encargado
            if (signal(SIGUSR1, &respSig) == SIG_ERR)
            {
                perror("ERROR: responsible signal\n");
            }
            pause();

            srand(getpid());
            // calcula un aleatrio [1 overbooking | 0 no overbooking]
            int numOver = generateRandom(0, 1);
            // duerme 2 segundos
            sleep(2);
            // sale con el aleatorio [1 overbooking | 0 no overbooking]
            exit(numOver);
        }
        else
        {
            // Se obtienenmos el numero de asistentes pasados por parametro
            int numAsistentes = atoi(argv[1]);
            // Se crea un vector para almacenar el numero de asistentes(id)
            pid_t *numAsistentesArr;
            numAsistentesArr = (int *)malloc(numAsistentes * sizeof(pid_t));

            for (int i = 0; i < numAsistentes; i++)
            {
                // Se crea el asistente
                pid_t pidAsist = fork();
                // Comprobamos si hay error en la creacion del asistente
                errorPid(pidAsist);

                if (pidAsist == 0)
                {
                    // Estamos en el hijo (asistente)
                    // Se comrpueba si hay error al recibir la señal SIGUSR2 del padre
                    if (signal(SIGUSR2, &assistSig) == SIG_ERR)
                    {
                        perror("ERROR: assistant signal\n");
                    }
                    pause();
                    srand(getpid());
                    // Duerme entre 3 y 6 segundos
                    sleep(generateRandom(3, 6));
                    // Genera un numero aleatorio de pasajeros entre 20 y 30
                    int numPasseng = generateRandom(20, 30);
                    // sale con el numero de pasajeros
                    exit(numPasseng);
                }
                else
                {
                    // Estamos en el padre(coordinador) y almacena el pid de los asistentes
                    numAsistentesArr[i] = pidAsist;
                }
            }

            // Estamos en coordinador
            // Espera 2 segundos a que se cree todo
            sleep(2);
            // Envia la señal al tecnico
            kill(pidTecnico, SIGUSR1);

            // Se obtiene el estado del exit del tecnico [0 inviable | 1 viable]
            int exitStatus;
            wait(&exitStatus);
            int numPassengers = 0;
            int exitCodeStatus = WEXITSTATUS(exitStatus);
            if (exitCodeStatus != 0)
            {
                // Si entra aqui significa que el estado de exit es [1] por tanto el vuelo es viable
                printf("Vuelo viable\n");
                // Duerme un segundo
                sleep(1);
                // Manda la señal al encargado
                kill(pidEncargado, SIGUSR1);
                wait(&exitStatus);
                // Obtiene el estado del exit del encargado [1 overbooking | 0 no overbooking]
                int overB = WEXITSTATUS(exitStatus);
                if (overB != 1)
                {
                    printf("No overbooking\n");
                }
                else
                {
                    printf("Overbooking\n");
                }
                // Manda la señal a los asistentes
                for (int i = 0; i < numAsistentes; i++)
                {
                    kill(numAsistentesArr[i], SIGUSR2);
                }
                /*Obtiene el estado del exit de los asistentes, en este caso es el numero de pasajeros
                 de cada asistente [entre 20 y 30]*/
                for (int j = 0; j < numAsistentes; j++)
                {
                    wait(&exitStatus);
                    int numAsistantPasseng = WEXITSTATUS(exitStatus);
                    numPassengers += numAsistantPasseng;
                    // Imprime el numero de pasajeros de cada asistente
                    assistantMessage(numAsistantPasseng, numAsistentesArr, j);
                }

                // Si hay overbooking decrementa el numero de pasajeros en 10
                if (exitsOverbook(overB))
                {
                    numPassengers -= 10;
                }
                /*Imprime un mensaje de finalizacion en funcion del estado de finalizacion pasado por parametro
                 *En este caso como ha ido bien mandamos un 0 por parametro*/
                printFinalFree(numAsistentesArr, numPassengers, 0);
            }
            else
            {
                // Si entra aqui significa que el estado de exit es [0] por tanto el vuelo es inviable
                for (int i = 0; i < numAsistentes; i++)
                {
                    // Mata el proceso de los asistentes
                    kill(numAsistentesArr[i], SIGKILL);
                }
                // Mata el proceso del encargado
                kill(pidEncargado, SIGKILL);
                // Mata el proceso del tecnico
                kill(pidTecnico, SIGKILL);
                /*Imprime un mensaje de finalizacion en funcion del estado de finalizacion pasado por parametro
                 *En este caso como ha fallado mandamos un 1 por parametro*/
                printFinalFree(numAsistentesArr, numPassengers, 1);
            }
        }
    }

    return 0;
}

void tecnicSig()
{
    printf("Technician checking flight\n");
}
void assistSig()
{
    printf("Assistant %d boarding passengers\n", getpid());
}

void respSig()
{
    printf("Manager checking overbooking\n");
}

void errorPid(pid_t pid)
{
    if (pid < 0)
    {
        printf("ERROR: fork()");
        exit(1);
    }
}

void assistantMessage(int numAsistantPasseng, pid_t *numAsistentesArr, int j)
{
    printf("Assistant %ld has boarded %d passengers.\n", (long)numAsistentesArr[j], numAsistantPasseng);
}

bool exitsOverbook(int overBook)
{
    if (overBook == 1)
    {
        return true;
    }
    return false;
}

void printFinalFree(pid_t *numAsistentesArr, int numPassengers, int exitFinalCode)
{
    if (exitFinalCode == 0)
    {
        printf("There are a total of %d passengers\n", numPassengers);
    }
    else
    {
        printf("The flight is not feasible.\n");
    }
    free(numAsistentesArr);
}

int generateRandom(int num1, int num2)
{
    return rand() % (num2 - num1 + 1) + num1;
}