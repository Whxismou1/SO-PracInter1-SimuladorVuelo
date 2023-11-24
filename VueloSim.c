#include <stdio.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

void tecnicSig();
void assistSig();
void respSig();
void errorPid(pid_t pid);
void assistantMessage(int numAsistantPasseng, pid_t *numAsistentesArr, int j);
bool exitsOverbook(int overBook);
void printFinalFree(pid_t *numAsistentesArr, int numPassengers, int exitFinalCode);
int generarRandom(int num1, int num2);

int main(int argc, char const *argv[])
{

    if(argc != 2){
        printf("ERROR: Num args innvalid!\n");
        exit(1);
    }

    // Creacion tecnico del avion
    pid_t pidTecnico;

    pidTecnico = fork();
    errorPid(pidTecnico);

    if (pidTecnico == 0)
    {
        // Entramos en el hijo tecnico
        if (signal(SIGUSR1, tecnicSig) == SIG_ERR)
        {
            perror("Señal del técnico.");
        }
        pause();
        
        srand(getpid());
        sleep(generarRandom(3, 6));
        exit(generarRandom(0, 1));
    }
    else
    {
        // Creamos el encargado
        pid_t pidEncargado;
        pidEncargado = fork();
        errorPid(pidEncargado);
        if (pidEncargado == 0)
        {
            if (signal(SIGUSR1, respSig) == SIG_ERR)
            {
                perror("Señal del encargado.");
            }
            pause();
            
            srand(getpid());
            int numOver = generarRandom(0, 1);
            sleep(3);
            exit(numOver);
        }
        else
        {
            int numAsistentes = atoi(argv[1]);
            pid_t *numAsistentesArr;
            numAsistentesArr = (int *)malloc(numAsistentes * sizeof(pid_t));

            for (int i = 0; i < numAsistentes; i++)
            {
                pid_t pidAsist = fork();
                errorPid(pidAsist);

                if (pidAsist == 0)
                {
                    if (signal(SIGUSR2, assistSig) == SIG_ERR)
                    {
                        perror("Señal del asistente.");
                    }
                    pause();
                    srand(getpid());
                    
                    sleep(generarRandom(3, 6));
                    int numPasseng = generarRandom(20, 30);
                    exit(numPasseng);
                }
                else
                {
                    numAsistentesArr[i] = pidAsist;
                }
            }

            // Estamos en coordinador

            sleep(2);
            kill(pidTecnico, SIGUSR1);
            int exitStatus;
            wait(&exitStatus);
            int numPassengers = 0;
            int exitCodeStatus = WEXITSTATUS(exitStatus);
            if (exitCodeStatus != 0)
            {
                // Vuelo viable
                printf("Vuelo viable");
                kill(pidEncargado, SIGUSR1);
                wait(&exitStatus);
                int overB = WEXITSTATUS(exitStatus);

                if (overB != 1)
                {
                    printf("No overbooking");
                }
                else
                {
                    printf("Overbooking");
                }

                for (int i = 0; i < numAsistentes; i++)
                {
                    kill(numAsistentesArr[i], SIGUSR2);
                }

                for (int j = 0; j < numAsistentes; j++)
                {
                    wait(&exitStatus);
                    int numAsistantPasseng = WEXITSTATUS(exitStatus);
                    numPassengers += numAsistantPasseng;

                    assistantMessage(numAsistantPasseng, numAsistentesArr, j);
                }

                if (exitsOverbook(overB))
                {
                    numPassengers -= 10;
                }

                printFinalFree(numAsistentesArr, numPassengers, 0);
            }
            else
            {
                for (int i = 0; i < numAsistentes; i++)
                {
                    kill(numAsistentesArr[i], SIGKILL);
                }
                kill(pidEncargado, SIGKILL);
                kill(pidTecnico, SIGKILL);
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
    printf("Assistant %d has boarded %d passengers.\n", numAsistentesArr[j], numAsistantPasseng);
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

int generarRandom(int num1, int num2)
{
    return rand() % (num2 - num1 + 1) + num1;
}