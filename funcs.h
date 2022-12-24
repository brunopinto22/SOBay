//
// Created by pinto on 16-11-2022.
//

#ifndef SOBAY_FUNCS_H
#define SOBAY_FUNCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "users_lib.h"

// vars globais

#define BACK_FIFO "BACKFIFO"
#define FRONT_FIFO "FRONTFIFO%d"

#define RD 0
#define WR 1
#define MAX_USERS 20
#define MAX_PROMOTORES 10
#define MAX_ITEMS 30

// estruturas
typedef struct dataID dataID;
struct dataID{
    pid_t pid;
    int id;
};

typedef struct dataMSG msg;
struct dataMSG {

    pid_t pid;
    int value;
    int cash,valor_add; //comando cash e add
    int offer,id_offer; //comando buy
    char command[7], arguments[150];
    char user[50], pass[50];
    char message[150];

};

typedef struct onlineuser onlineuser, *ponlineusers;
struct onlineuser{

    int pid;
    char nome[50];

};

typedef struct items item, *pitems;
struct items{
    pid_t pid;
    int id, duracao;
    char nome[50], categoria[50];
    int preco, preco_ja,envio;
    char vendedor[50],licitador[50];
    int licit_offer;
    pitems proximo;
   
};

typedef struct promotores prom, *pproms;
struct promotores{

    char *nome;

};


// funcoes

int getId();
// Retorna o id para o novo item


void closeAllFronts(ponlineusers users, int count);
// Fecha todos os frontends


int countWords(char* string, int len);
// Le a string e conta as palavras que a mesma contem


void printOnlineUsers(ponlineusers users, int count);
// Imprime todos os utilizadores online


int addOnlineUser(ponlineusers users, onlineuser new, int* count);
// Adiciona a array o utilizador que tentou dar login
// Retorna  0 caso nao estajea logado
//          1 caso ja esteja logado
//         -1 caso tenha atingido o max de utilizadores

int deleteOnlineUser(ponlineusers users, char* remove, int* count);
// Remove o ultilizador da lista
// Retorna  pid dele
//          0 caso nao tenho encontrado ninguem
//         -1 caso nao esteja ninguem logado


int loadItemsFile(char * filename);
// Le o ficheiro de items
// Guarda os items na estrututra leilao
// Retorna    o número de items lidos
//           -1 em caso de erro


void printItems(int type, char* filter,int valor);
// Le o ficheiro de items
// Imprime    <id> <nome> <categoria> <preco-base> <preco-compre-ja> <duracao>
// Conforme o valor do type sera filtrada a listagem conforme o filter
// Caso type seja       0 - nao tem filtros, logo o filter é ignorado
//                      1 - filtra por categoria
//                      2 - filtra por vendedor
//                      3 - mostra todos os items ate a um valor
//                      4 - mostra todos os items ate uma determinada hora


int addItem(char* aux);
// Adiciona um item ao ficheiro de items e a estrutura leilao
// Retorna      0 caso corra tudo bem
//              -1 em caso de erro


void printCatg(char* filter); // implementar
// Le o ficheiro de items
// Imprime    <id> <nome> <categoria> <preco-base> <preco-compre-ja> <duracao>
// Filtrando os items que nao sejam da categoria = filter


void printVend(char* filter); // implementar
// Le o ficheiro de items
// Imprime    <id> <nome> <categoria> <preco-base> <preco-compre-ja> <duracao>
// Filtrando os items que nao sejam do vendedor = filter


void printVal(int filter); // implementar
// Le o ficheiro de items
// Imprime    <id> <nome> <categoria> <preco-base> <preco-compre-ja> <duracao>
// Filtrando os items que tenham um valor maior do que filter


int printTime(); 
// Imprime a hora atual em segundos


int loadPromotoresFile(char * filename);
// Le o ficheiro de promotores
// Retorna    o número de items lidos
//           -1 em caso de erro

//void preenche_itens(items p);

int buy_item(char * filename,int id, int oferta, char * licitador, int pid);
// Retorna      0 caso seja comprado pelo preço compre já
//              1 caso tenha sido feita uma licitação
//             -1 em caso da licitação feita ser abaixo do preço do item 

#endif //SOBAY_FUNCS_H
