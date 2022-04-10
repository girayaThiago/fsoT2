#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>


//const int MSGSIZE = 10; // error: redefining array size during execution <<
#define NCHILDS 5

void child(double t){
    printf("processo pid = <%d> liberado após %.2f segundos\n",getpid(), t);
    long int i;
    for(i=0; i<50000; i++){};
}

int main() {

    srand(time(NULL)); //inicializa rand seed;
    time_t start[NCHILDS]; // cada filho terá um marco inicial;

    // cada processo terá seu pid, o pai terá um array de pids filhos;
    pid_t pid;
    pid_t pids[NCHILDS];

    // o pai terá também um array de tempo para aguardar antes de liberar cada thread;
    int stall[NCHILDS];
    int i;
    

    for (i = 0; i < NCHILDS; i++){
        start[i] = time(NULL); //marca o momento em que o filho foi criado.
        stall[i] = rand()%120+60; // cria uma espera de 60-120 segundos;
        pid = fork();
        if (pid == 0) {
            //cada filho tem que aguardar um sinal do pai antes de iniciar a execução do busy wait;
            child(difftime(time(NULL), start[i]));
            break;
        }
        printf("Processo de numero %d e pid <%d> criado, aguardando %d segundos até liberar execução\n", i, pid, stall[i]);
        kill(pid, SIGSTOP); //manda o filho parar execução
        pids[i] = pid; //armazena pid do filho criado;
    }
    if (pid == 0) {
        // child
    } else {
        // parent
        int cont = NCHILDS;
        while (cont > 0) {
            for(i = 0; i < NCHILDS; i++){
                if(stall[i] < (int)difftime(time(NULL),start[i])){
                    stall[i] = 999999999; //marca o stall como infinito pq n sei como tirar isso daqui de um jeito elegante :P
                    kill(pids[i],SIGCONT);
                    cont--;
                }
            }
        }
        int status;
        for( i = 0; i < NCHILDS; i++){
            waitpid(pids[i], &status, 0);
            if (WIFSIGNALED(status)){
                printf("Error\n");
            }
            else if (WEXITSTATUS(status)){
                printf("Exited Normally\n");
            }
        }
    }
    // pid_t pids[10];
    // pid_t pid;
    // int i;
    // for (int i = 0; i < 5; i++){
    //     pid = fork();
    //     if (pid == 0){
    //         break;
    //     } 
    // }

    return 0;
}