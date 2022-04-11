#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>


//const int MSGSIZE = 10; // error: redefining array size during execution <<
#define NCHILDS 5
#define MSGSIZE 10

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[MSGSIZE];    /* message data */
};  

void child(time_t created){
    time_t started = time(NULL);
    printf("processo <%d> liberado após %.2f segundos\n", getpid(), difftime(started, created));
    
    long int i;
    // for(i=0; i<10000000000; i++); //5 segundos
    for(i=0; i<50000000000; i++); //25 segundos
    time_t finished = time(NULL);
    // printf("processo pid = <%d> encerrado após %.2f segundos\n", getpid(), difftime(started, created));
    printf("processo <%d> encerrado\n", getpid());
    exit(1);
}

int main() {

    srand(time(NULL)); //inicializa rand seed;
    time_t created[NCHILDS]; // marca quando cada filho foi criado;
    time_t started[NCHILDS]; // marca quando cada filho foi liberado;
    time_t ended[NCHILDS]; //marca quando filho foi encerrado;
    // cada processo terá seu pid, o pai terá um array de pids filhos;
    pid_t pid, fpid;
    pid_t pids[NCHILDS];

    // o pai terá também um array de tempo para aguardar antes de liberar cada thread;
    int stall[NCHILDS];
    int i, status;


    for (i = 0; i < NCHILDS; i++){
        created[i] = time(NULL); //marca o momento em que o filho foi criado.
        stall[i] = rand()%121+60; // cria uma espera de 60-120 segundos;
        pid = fork();
        if (pid == 0) {
            //cada filho tem que aguardar um sinal do pai antes de iniciar a execução do busy wait;
            child(created[i]);
            break;
        }
        kill(pid, SIGSTOP); //manda o filho parar execução
        printf("Processo %d com pid <%d> criado, aguardando %d segundos até liberar execução\n", i, pid, stall[i]);
        pids[i] = pid; //armazena pid do filho criado;
    }
    if (pid == 0) {
        // child
    } else {
        // parent
        int waiting = NCHILDS, running = 0;
        while (waiting > 0 || running > 0) {
            if (waiting > 0){
                for(i = 0; i < NCHILDS; i++){
                    if(stall[i] < (int)difftime(time(NULL),created[i])){
                        stall[i] = 999999999; //marca o stall como infinito pq n sei como tirar isso daqui de um jeito elegante :P
                        started[i] = time(NULL);
                        kill(pids[i],SIGCONT);
                        waiting--;
                        running++;
                    }
                }
                if (waiting == 0)
                    printf("todos os processos liberados");
            }
            if (running > 0){
                fpid = waitpid(P_ALL, &status, WNOHANG); //verifica cada filho se já terminou
                if (fpid){
                    printf("fpid = %d, status = %d\n", fpid, status);
                    for( i = 0; i < NCHILDS; i++){
                        if (pids[i] == fpid){
                            printf("Processo %d <%d> terminou com turnaround de %.2f segundos\n", i, fpid, difftime(time(NULL), started[i]));
                            running--; //marca com -1 filho rodando
                            break;
                        }
                    } 
                     
                }
            }
        }
    }

    return 0;
}