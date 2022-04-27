# fsoT2
trabalho 2 de fso 2022/1


Mecanismos de comunicação
Kill(pid, SIGSTOP) //interrompe a execução do processo com pid = pid
Kill(pid, SIGCONT) //resume execução do processo com pid = pid

waitpid(P_ALL, &status, WNOHANG) // verifica se algum filho já terminou a execução, retorna o pid de algum filho que já tenha terminado a execução, a flag WNOHANG faz com que se não houver retorno a execução no pai não é interrompida.


estruturas de dados
NCHILDS = numero de filhos a serem criados

time_t created[NCHILDS]; marcar o tempo de criação de cada filho.
time_t started[NCHILDS]; marcar o tempo que liberamos a execução de cada filho.
time_t ended[NCHILDS]; marca quando filho encerrou sua execução;
// cada processo terá seu pid, o pai terá um array de pids filhos;
pid_t pid; pid do processo atual;
pid_t fpid; variavel para armazenar o pid de algum filho que terminar sua execução;
pid_t pids[NCHILDS]; // pai guarda o pid dos filhos

// o pai terá também um array de tempo para aguardar antes de liberar cada thread;
int stall[NCHILDS]; cada filho terá de esperar um tempo até ser liberado;
int i, status; variaveis de retorno para as funções de wait e contadores de forloop;