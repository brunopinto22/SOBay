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


int main(int argc, char**argv){
    setbuf(stdout, NULL);

    // verificacao da formatacao
    if(argc!=3){ printf("\n\033[31mERRO na formatação: ./FrontEnd <username> <password>\033[0m\n"); return 1;}

    // criar fifo
	char nomeFifoFront[100];
	sprintf (nomeFifoFront,FRONT_FIFO,getpid());
    if (mkfifo(nomeFifoFront,0666) == -1)  { printf("\n\033[31mERRO nao foi possivel criar o fifo\033[0m\n"); return 1; }    

    char string[150], command[7], arguments[150];
    msg mensagem, resposta;
    int valor, id, duracao;
    
    mensagem.pid = getpid();
    strcpy(mensagem.command,"login");
    strcpy(mensagem.user,argv[1]);
    strcpy(mensagem.pass,argv[2]);
    
    
    // abirir fifo do backend
    int fd_backend = open(BACK_FIFO,O_WRONLY);
    if (fd_backend == -1){ printf("\n\033[31mERRO o servidor não está a correr\033[0m\n"); return 1; }
    int s = write(fd_backend,&mensagem,sizeof(msg));
    if(s<=0)
        printf("\n\033[31mERRO no envio\n");  
    close (fd_backend);

    
    //receber a resposta
    int fd_frontend = open(nomeFifoFront,O_RDONLY);
    if(fd_frontend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo\n"); return 1; }
    read(fd_frontend,&resposta,sizeof(msg));
    close(fd_frontend);

    if(resposta.value == 1){ printf("\n\033[31mCredenciais erradas\033[0m\n"); return 1; }
    
    
    printf("\n-- Frontend\n");
    do{

        printf("\n>> ");
        scanf("%[^\n]", string);

        // divisão do comando em duas partes: <comando> <argumentos>
        sscanf(string, "%s %[^\n]", command, arguments);
        setbuf(stdin,NULL);

        strcpy(mensagem.command,command);
        strcpy(mensagem.arguments,arguments);

        // verificação do tipo de comando
        if(strcmp(command,"sell") == 0){
            if(countWords(arguments, strlen(arguments)) != 5)
                printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");
            else{
                item i;
                if(sscanf(arguments,"%s %s %d %d %d", i.nome, i.categoria, &i.preco, &i.preco_ja, &i.duracao) != 0){
                
                    int fd_backend = open(BACK_FIFO,O_WRONLY);
                    if (fd_backend == -1){
                        printf("\n\033[31mERRO o servidor não está a correr");
                        return 1;
                    }    
                
                    int s = write (fd_backend,&mensagem,sizeof(msg));
                    if (s<=0)
                        printf("\n\033[31mERRO no envio\n");  
                    close(fd_backend);

                    //receber a resposta
                    int fd_frontend = open(nomeFifoFront,O_RDONLY);
                    if(fd_frontend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo\n"); return 1; }
                    read(fd_frontend,&resposta,sizeof(msg));
                    close(fd_frontend);

                    if(resposta.value == 0){
                        printf("\n\033[32mProduto Adicionado com sucesso\033[0m\n");
                    }else printf("\nErro a adicionar o produto\n"); 

                }else
                    printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");

            }

        }else if(strcmp(command,"list") == 0){
            if(countWords(arguments, strlen(arguments)) != 0)
                printf("\n\033[31mERRO na formatação: list \033[0m\n");
            else{
                printf("\nA carregar lista de items...\n");
                
            }

        }else if(strcmp(command,"licat") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: licat <categoria>\033[0m\n");
                
                
            else{
                printf("\nA carregar lista de items da categoria %s...\n", arguments);
                
            }

        }else if(strcmp(command,"lisel") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: lisel <username-do-vendedor>\033[0m\n");
            else{
                printf("\nA carregar lista de items do vendedor: %s...\n", arguments);
            }

        }else if(strcmp(command,"lival") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
            else{
                if(sscanf(arguments,"%d", &valor) != 0)
                    printf("\nA carregar lista de items com preços até %d€...\n", valor);
                else
                    printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
            }

        }else if(strcmp(command,"litime") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
            else{
                if(sscanf(arguments,"%d", &duracao) != 0)
                    printf("\nA carregar lista de items com prazo até %d...\n", duracao);
                else
                    printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
            }

        }else if(strcmp(command,"time") == 0){
            if(countWords(arguments, strlen(arguments)) != 0)
                printf("\n\033[31mERRO na formatação: time\033[0m\n");
            else{
                printf("\nHora atual em segundos\n");
                
            }

        }else if(strcmp(command,"buy") == 0){
            if(countWords(arguments, strlen(arguments)) != 2)
                printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
            else{
                if(sscanf(arguments,"%d %d", &id, &valor) != 0)
                    printf("A licitar em %d com %d€...\n", id, valor);
                else
                    printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
            }


        }else if(strcmp(command,"cash")==0){
            if(countWords(arguments, strlen(arguments)) != 0)
                printf("\n\033[31mERRO na formatação: cash\033[0m\n");
            else{
                printf("\nA consultar o saldo...\n");
                
            
        }}else if(strcmp(command,"add")==0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
            else{
                if(sscanf(arguments,"%d", &valor) != 0){
                    printf("A adicionar %d€...\n", valor);
                    
                    
                }
                else
                    printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
            }

        }else if(strcmp(command,"exit")==0){
            break;

        }else
            printf("\nERRO - comando nao existe\n");

        arguments[0] = 0;

    }while(1);

    printf("Adeus ;)\n");


    
}
