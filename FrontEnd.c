#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "funcs.h"
#include "users_lib.h"


int main(int argc, char**argv){

    char string[150], command[7], argumments[150];
    char username[20],password[20], nome[50], categoria[50];
    int id, duracao, valor, compra_ja;

    if(argc!=3){ printf("\n\033[31mERRO na formatação: ./FrontEnd <username> <password>\033[0m\n"); return 1;}

    printf("\n-- Frontend\n");
    do{

        printf("\n>> ");
        scanf("%[^\n]", string);

        // divisão do comando em duas partes: <comando> <argumentos>
        sscanf(string, "%s %[^\n]", command, argumments);
        setbuf(stdin,NULL);


        // verificação do tipo de comando
        if(strcmp(command,"sell") == 0){
            if(countWords(argumments, strlen(argumments)) != 5)
                printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");
            else{
                if(sscanf(argumments,"%s %s %d %d %d", nome, categoria, &valor, &compra_ja, &duracao) != 0)
                    printf("\nA adicionar item\nNome: %s\nCategoria: %s\nValor: %d€\nValor de compra já: %d€\nDuração: %ds\n", nome, categoria, valor, compra_ja, duracao);
                else
                    printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");
            }

        }else if(strcmp(command,"list") == 0){
            if(countWords(argumments, strlen(argumments)) != 0)
                printf("\n\033[31mERRO na formatação: list \033[0m\n");
            else{
                printf("\nA carregar lista de items...\n");
            }

        }else if(strcmp(command,"licat") == 0){
            if(countWords(argumments, strlen(argumments)) != 1)
                printf("\n\033[31mERRO na formatação: licat <categoria>\033[0m\n");
            else{
                printf("\nA carregar lista de items da categoria %s...\n", argumments);
            }

        }else if(strcmp(command,"lisel") == 0){
            if(countWords(argumments, strlen(argumments)) != 1)
                printf("\n\033[31mERRO na formatação: lisel <username-do-vendedor>\033[0m\n");
            else{
                printf("\nA carregar lista de items do vendedor: %s...\n", argumments);
            }

        }else if(strcmp(command,"lival") == 0){
            if(countWords(argumments, strlen(argumments)) != 1)
                printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
            else{
                if(sscanf(argumments,"%d", &valor) != 0)
                    printf("\nA carregar lista de items com preços até %d€...\n", valor);
                else
                    printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
            }

        }else if(strcmp(command,"litime") == 0){
            if(countWords(argumments, strlen(argumments)) != 1)
                printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
            else{
                if(sscanf(argumments,"%d", &duracao) != 0)
                    printf("\nA carregar lista de items com prazo até %d...\n", duracao);
                else
                    printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
            }

        }else if(strcmp(command,"time") == 0){
            if(countWords(argumments, strlen(argumments)) != 0)
                printf("\n\033[31mERRO na formatação: time\033[0m\n");
            else{
                printf("\nHora atual em segundos\n");
            }

        }else if(strcmp(command,"buy") == 0){
            if(countWords(argumments, strlen(argumments)) != 2)
                printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
            else{
                if(sscanf(argumments,"%d %d", &id, &valor) != 0)
                    printf("A licitar em %d com %d€...\n", id, valor);
                else
                    printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
            }


        }else if(strcmp(command,"cash")==0){
            if(countWords(argumments, strlen(argumments)) != 0)
                printf("\n\033[31mERRO na formatação: cash\033[0m\n");
            else{
                printf("\nA consultar o saldo...\n");
            }

        }else if(strcmp(command,"add")==0){
            if(countWords(argumments, strlen(argumments)) != 1)
                printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
            else{
                if(sscanf(argumments,"%d", &valor) != 0)
                    printf("A adicionar %d€...\n", valor);
                else
                    printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
            }

        }else if(strcmp(command,"exit")==0){
            break;

        }else
            printf("\nERRO - comando nao existe\n");

        argumments[0] = 0;

    }while(1);

    printf("Adeus ;)\n");

}
