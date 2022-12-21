#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include "funcs.h"
#include "users_lib.h"


int main() {
    char string[150], command[7], arguments[150];
    char nomeFifoFront[100];
    int size;
    msg mensagem;

    int res;
    fd_set fds;
    struct timeval timeout;

    setbuf(stdout, NULL);
    
    char *FPROMOTORES = "FPROMOTORES";
    // Verificar se existe
    if( !getenv(FPROMOTORES) ){
        printf("\n\033[31mErro variavel de ambiente nao existe: %s\033[0m\n", FPROMOTORES);
        exit(1);
    }
    char *FUSERS = "FUSERS";
    
    // Verificar se existe
    if( !getenv(FUSERS) ){
        printf("\n\033[31mErro variavel de ambiente nao existe: %s\033[0m\n", FUSERS);
        exit(1);
    }
    char *FITEMS = "FITEMS";
    // Verificar se existe
    if( !getenv(FITEMS) ){
        printf("\n\033[31mErro variavel de ambiente nao existe: %s\033[0m\n", FITEMS);
        exit(1);
    }
    // Carregar Utilizadores
    if (loadUsersFile(FUSERS) == -1) {
        printf("\n\033[31m%s\033[0m", getLastErrorText());
        return 1;
    }
    // Carregar Items
    if (loadItemsFile(FITEMS) == -1) {
        printf("\n\033[31m%s\033[0m", getLastErrorText());
        return 1;
    }
    
    // abertura do fifo do backend para leitura
    if (mkfifo(BACK_FIFO,0666) == -1)  {
       if (errno == EEXIST){
           printf ("Servidor em execução ou fifo já existe\n");
       }
       printf("\n\033[31mERRO nao foi possivel criar o fifo\n");
       return 1;
    }
    int fd_backend = open (BACK_FIFO,O_RDWR);
    if (fd_backend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo\n"); return 1; }

    printf("\n-- Backend\n\n>> ");
    do{
        // limpar a vars
        FD_ZERO(&fds);
        arguments[0] = 0;

        // canais para estar atento
        FD_SET(0, &fds);
        FD_SET(fd_backend, &fds);

        // set do timeout
        timeout.tv_sec = 5; // Timeout de 5 segundos

        res = select(fd_backend+1, &fds, NULL, NULL, &timeout);
        if(res == -1){
            printf("Occoreu um erro no select\n");
            close(fd_backend);
            unlink(BACK_FIFO);
            exit(1);
        }  else if(res >0 && FD_ISSET(0, &fds)){
            // ler os comandos do admin
            scanf("%[^\n]", string);

            // divisão do comando em duas partes: <comando> <argumentos>
            sscanf(string, "%s %[^\n]", command, arguments);
            setbuf(stdin,NULL);

            // verificação do tipo de comando
            if(strcmp(command, "users") == 0){
                if(countWords(arguments, strlen(arguments)) != 0)
                    printf("\n\033[31mERRO na formatação: users \033[0m\n");
                else{
                    printf("\nA carregar lista de users...\n");
                }

            } else if(strcmp(command, "list") == 0){
                if(countWords(arguments, strlen(arguments)) != 0)
                    printf("\n\033[31mERRO na formatação: list \033[0m\n");
                else{
                    printf("\nA carregar lista de items...\n");
                }

            } else if(strcmp(command, "kick") == 0){
                if(countWords(arguments, strlen(arguments)) != 1)
                    printf("\n\033[31mERRO na formatação: kick <username>\033[0m\n");
                else{
                    printf("\nA expulsar %s...\n", arguments);
                }

            } else if(strcmp(command, "prom") == 0){
                if(countWords(arguments, strlen(arguments)) != 0)
                    printf("\n\033[31mERRO na formatação: prom \033[0m\n");
                else{
                    printf("\nA carregar lista de promotores...\n");
                }

            } else if(strcmp(command, "reprom") == 0){
                if(countWords(arguments, strlen(arguments)) != 0)
                    printf("\n\033[31mERRO na formatação: prom \033[0m\n");
                else{
                    printf("\nA ler ficheiro de promotores...\n");
                }

            } else if(strcmp(command, "cancel") == 0){
                if(countWords(arguments, strlen(arguments)) != 1)
                    printf("\n\033[31mERRO na formatação: cancel <nome-do-executável-do-promotor>\033[0m\n");
                else{
                    printf("\nA terminar promotor...\n");
                }

            } else if(strcmp(command, "close") == 0)
                break;
            else
                printf("\nERRO - comando nao existe\n");

            printf("\n>> ");

        } else if(res > 0 && FD_ISSET(fd_backend, &fds)) {
            // ler pedidos do frontend
            size = read(fd_backend, &mensagem, sizeof(msg));

            if (size > 0) {

                if(strcmp(mensagem.command,"login") == 0){

                    if (isUserValid(mensagem.user, mensagem.pass) == -1) {
                        printf("\n\033[31m%s\033[0m", getLastErrorText());
                        return 1;
                    }
                    else if (isUserValid(mensagem.user, mensagem.pass) == 0) {
                        printf("\n\033[31m> User [%d] : Conta nao existe\033[0m", mensagem.pid);
                        mensagem.value = 1;

                        //resposta ao frontend
                        sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
                        int fd_frontend = open(nomeFifoFront, O_WRONLY);
                        if(fd_frontend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid); return 1; }
                        int s2 = write(fd_frontend, &mensagem, sizeof(msg));
                        close(fd_frontend);
                    }
                    else {
                        mensagem.value = 0;
                        printf("\n\033[32m> User [%d] : Fez login\033[0m\n", mensagem.pid);

                        //resposta ao frontend
                        sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
                        int fd_frontend = open(nomeFifoFront, O_WRONLY);
                        if(fd_frontend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid); return 1; }
                        int s2 = write(fd_frontend, &mensagem, sizeof(msg));
                        close(fd_frontend);
                    }

                } else if(strcmp(mensagem.command,"sell") == 0) {
                    if(addItem(mensagem.arguments) == 0){
                        mensagem.value = 0;
                        printf("\n\033[33m> User [%d] : Licitou <%s>\033[0m\n", mensagem.pid, mensagem.arguments);
                    }
                    else
                        mensagem.value = 1;
                    

                    //resposta ao frontend
                    sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
                    int fd_frontend = open(nomeFifoFront, O_WRONLY);
                    if(fd_frontend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid); return 1; }
                    int s2 = write(fd_frontend, &mensagem, sizeof(msg));
                    close(fd_frontend);
                }

            }


            printf("\n>> ");
        }
    
    }while (1);
    

    /*//--------------------------------------------------------------------------------------------------------------
    // Meta 1 testes
    int i;
    char username[50], userpass[50];
    int pid, meu_pid, pid_filho;
    int canal[2];
    int n;
    char buf[200];
    printf("deseja testar que funcionalidade?\n\n1-utilizadores\n2-promotor\n3-items\n4-comandos\n\n>> ");
    scanf("%d", &i);
    switch (i) {

        case 1:
            if( loadUsersFile(FUSERS) == -1 ){ printf("\n\033[31m%s\033[0m\n", getLastErrorText()); return 1;}
            // pede os dados ao utilizador
            printf("login: ");
            scanf("%s %s", username, userpass);
            if(isUserValid(username, userpass) == -1){ printf("\n\033[31m%s\033[0m\n", getLastErrorText()); return 1;}
            else if(isUserValid(username, userpass) == 0){ printf("\n\033[31mErro conta nao existe\033[0m\n"); return 1;}
            return 0;

        case 2:
            meu_pid = getpid();
            printf("\nBackend>> O meu PID: %d\n", meu_pid);

            if( loadPromotoresFile(FPROMOTORES) == -1 )
                return -1;
            
            // verificacao da criacao do pipe
            if(pipe(canal) < 0){
                printf("\n\033[31mErro na criacao do pipe\033[0m\n");
                return 1;
            }

            pid = fork();
            if(pid == -1){
                printf("\n\033[31mErro no fork()\033[0m\n");
                return 1;

            } else if(pid == 0){ // Promotor:

                pid_filho = getpid(); // pid do promotor
                printf("\nPromotor>> O meu PID: %d\n", pid_filho);

                // redirecionar stdout para o canal
                close(STDOUT_FILENO);
                dup(canal[1]);
                close(canal[0]);

                execlp("./promotor_oficial", "./promotor_oficial", NULL);
            } else{ // Backend:

                while(1){
                    // ler o canal
                    n = read(canal[0], buf, 199);
                    buf[n] = 0;
                    fprintf(stdout,"Promotor>> %s", buf);
                    // limpar a memoria
                    fflush(stdout);
                    memset(buf, 0, 199);
                }

            }
            return 0;

        case 3:
            if( loadItemsFile(FITEMS) == -1 )
                return 1;
            printItems(0," ");
            return 0;

        case 4:
        break;
    }
    setbuf(stdin,NULL);

   //--------------------------------------------------------------------------------------------------------------
*/
    // Fechar\Apagar o fifo do backend
    close(fd_backend);
    unlink(BACK_FIFO);

    printf("\nAdeus ;)\n");
    exit(0);


}
