#include "funcs.h"


#define PID getpid()

int fd_frontend;
char nomeFifoFront[100];


void closeFront(int s,siginfo_t *i, void *v){
    int n=i->si_value.sival_int;

    if(n == 0)
        printf("\n\033[31m> O Backend expulsou-o\033[0m\n");
    else
        printf("\n\033[35m> O Backend fechou\033[0m\n");

    close(fd_frontend);
    unlink(nomeFifoFront);

    
    exit(0);

}


int main(int argc, char**argv){
    setbuf(stdout, NULL);

    // verificacao da formatacao
    if(argc!=3){ printf("\n\033[31mERRO na formatação: ./FrontEnd <username> <password>\033[0m\n"); return 1;}

    // criar fifo
	sprintf (nomeFifoFront,FRONT_FIFO,PID);
    if (mkfifo(nomeFifoFront,0666) == -1)  { printf("\n\033[31mERRO nao foi possivel criar o fifo\033[0m\n"); return 1; }    

    char string[150], command[7], arguments[150];
    msg mensagem, resposta;
    int valor, id, duracao;
    
    mensagem.pid = getpid();
    strcpy(mensagem.command,"login");
    strcpy(mensagem.user,argv[1]);
    strcpy(mensagem.pass,argv[2]);
    
    
    
    // abrir fifo do backend
    int fd_backend = open(BACK_FIFO,O_WRONLY);
    if (fd_backend == -1){ printf("\n\033[31mERRO o servidor não está a correr\033[0m\n"); return 1; }
    int s = write(fd_backend,&mensagem,sizeof(msg));
    if(s<=0)
        printf("\n\033[31mERRO no envio\n");  
    close (fd_backend);

    
    //receber a resposta
    fd_frontend = open(nomeFifoFront,O_RDONLY);
    if(fd_frontend == -1){ printf("\n\033[31mERRO nao foi possivel abrir o fifo\n"); return 1; }
    read(fd_frontend,&resposta,sizeof(msg));
    close(fd_frontend);

    if(resposta.value == 1){ printf("%s", resposta.message); return 1; }

    // handlers de sinais
    signal(SIGINT, (void (*)(int))closeFront);
    struct sigaction sa;
    sa.sa_sigaction=closeFront;
    sa.sa_flags = SA_RESTART|SA_SIGINFO;
    sigaction(SIGUSR1,&sa,NULL);
    
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
                strcat(arguments," ");
                strcat(arguments,mensagem.user);
                strcat(arguments," -");
                strcpy(mensagem.arguments,arguments);
                printf(">%s",arguments);
                if(sscanf(arguments,"%s %s %d %d %d %s %s", i.nome, i.categoria, &i.preco, &i.preco_ja, &i.duracao, i.vendedor, i.licitador) != 0){
                  
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
                loadItemsFile("FITEMS");
                printItems(0," ",0);
                
            }

        }else if(strcmp(command,"licat") == 0){
            if(countWords(arguments, strlen(arguments)) != 1){
                printf("\n\033[31mERRO na formatação: licat <categoria>\033[0m\n");

                
            }
            else{
                printf("\nA carregar lista de items da categoria %s...\n", arguments);

                loadItemsFile("FITEMS");
                printItems(1,arguments,0);
                
            }

        }else if(strcmp(command,"lisel") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: lisel <username-do-vendedor>\033[0m\n");
            else{
                printf("\nA carregar lista de items do vendedor: %s...\n", arguments);

                loadItemsFile("FITEMS");
                printItems(2,arguments,0);

            }

        }else if(strcmp(command,"lival") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
            else{
                if(sscanf(arguments,"%d", &valor) != 0){
                    printf("\nA carregar lista de items com preços até %d€...\n", valor);
                    
                    loadItemsFile("FITEMS");
                    printItems(3,arguments,valor);
                }else
                    printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
            }

        }else if(strcmp(command,"litime") == 0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
            else{
                if(sscanf(arguments,"%d", &duracao) != 0){
                    printf("\nA carregar lista de items com prazo até %d...\n", duracao);
                    loadItemsFile("FITEMS");
                    printItems(4,arguments,duracao);
                }
                else
                    printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
            }

        }else if(strcmp(command,"time") == 0){
            if(countWords(arguments, strlen(arguments)) != 0)
                printf("\n\033[31mERRO na formatação: time\033[0m\n");
            else{
                printf("\nHora atual em segundos\n");
                printf("%d",printTime());
                
            }

        }else if(strcmp(command,"buy") == 0){
            if(countWords(arguments, strlen(arguments)) != 2)
                printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
            else{
                if(sscanf(arguments,"%d %d", &mensagem.id_offer, &mensagem.offer) != 0){

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
                        printf("\n\033[32mComprou o item %d\033[0m\n",resposta.id_offer);
                    }else if(resposta.value == 1){
                        printf("\n\033[32mLicitou o item %d com %d €\033[0m\n",resposta.id_offer,resposta.offer);
                    }else{printf("\n\033[32mLicitação inválida\033[0m\n");}
                }

                else
                    printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
            }


        }else if(strcmp(command,"cash")==0){
            if(countWords(arguments, strlen(arguments)) != 0)
                printf("\n\033[31mERRO na formatação: cash\033[0m\n");
            else{
                printf("\nA consultar o saldo...\n");
                
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
                        printf("\n\033[32mSaldo: %d\033[0m\n",resposta.cash);
                    }else printf("\nErro a adicionar o produto\n");
            
        }}else if(strcmp(command,"add")==0){
            if(countWords(arguments, strlen(arguments)) != 1)
                printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
            else{
                if(sscanf(arguments,"%d", &mensagem.valor_add) != 0){
                    printf("A adicionar %d€...\n", mensagem.valor_add);
                    
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
                        printf("Saldo adicionado com sucesso");
                    }else printf("\nErro a adicionar o saldo\n");
                }
                else
                    printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
            }

        }else if(strcmp(command,"exit")==0){

            msg mensagem;
            mensagem.pid = PID;
            strcpy(mensagem.command,"exit");
            int fd_backend = open(BACK_FIFO,O_WRONLY);
            if (fd_backend == -1){
                printf("\n\033[31mERRO o servidor não está a correr");
                exit(1);
            }    
        
            int s = write (fd_backend,&mensagem,sizeof(msg));
            if (s<=0)
                printf("\n\033[31mERRO no envio\n");  
            close(fd_backend);
            break;

        }else
            printf("\nERRO - comando nao existe\n");

        arguments[0] = 0;

    }while(1);

    close(fd_frontend);
    unlink(nomeFifoFront);

    printf("\n\033[32mAdeus\033[0m\n");
    exit(0);
    
}
