#ifndef SOBAY_FUNCS_H
#define SOBAY_FUNCS_H

// includes
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

typedef struct string strings, *pstring;
struct string{

    char text[250];
   
};

typedef struct dataMSG msg, *pmsg;
struct dataMSG {

    pid_t pid;
    int value;
    int cash,valor_add; //comando cash e add
    int offer,id_offer; //comando buy
    char command[7], arguments[150];
    char user[50], pass[50];
    char message[500];
    strings output[MAX_ITEMS];

};

typedef struct onlineuser onlineuser, *ponlineusers;
struct onlineuser{

    int pid;
    char nome[50];

};

typedef struct items item, *pitems;
struct items{

    int id, duracao;
    char nome[50], categoria[50];
    int preco, preco_ja;
    char vendedor[50],licitador[50];
    int ofertamax;
   
};

typedef struct proms prom, *pproms;
struct proms{
    pid_t pid;
    char nome[150];
    char categoria[150];
    int promocao;
   
};


// funcoes

int getId();
// Retorna o id para o novo item

void printHelp_Back();
// Imprime toda a informacao sobre os comandos do BACKEND

void printHelp_Front();
// Imprime toda a informacao sobre os comandos do FRONTEND		

void closeAllFronts(ponlineusers users, int count);
// Fecha todos os frontends

void notificaAllFronts(ponlineusers users, int count, char nome[], int id, int pid);
// Envia uma notificacao para todos os Frontends

int countWords(char* string, int len);
// Le a string e conta as palavras que a mesma contem


void printOnlineUsers(ponlineusers users, int count);
// Imprime todos os utilizadores online


int addOnlineUser(ponlineusers users, onlineuser new, int* count);
// Adiciona a array o utilizador que tentou dar login
// Retorna  0 caso nao estajea logado
//          1 caso ja esteja logado
//         -1 caso tenha atingido o max de utilizadores

int deleteOnlineUser(ponlineusers users, char* remove, int rpid, int* count);
// Remove o ultilizador da lista
// Retorna  pid dele
//          0 caso nao tenho encontrado ninguem
//         -1 caso nao esteja ninguem logado


int loadItemsFile(pitems leilao, char * filename, int* count);
// Le o ficheiro de items
// Guarda os items na estrututra leilao
// Retorna    o numero de items lidos
//           -1 em caso de erro


int updateItemsFile(pitems leilao, int count);
// Atualiza o ficheiro de items


int countItems(pitems leilao, int count, int type, char filter[], int valor);
// Conta os items na estrutura
// Retorna      o numero de items encontrados
// Conforme o valor do type sera filtrada a listagem conforme o filter
// Caso type seja       0 - nao tem filtros, logo o filter é ignorado
//                      1 - filtra por categoria
//                      2 - filtra por vendedor
//                      3 - mostra todos os items ate a um valor
//                      4 - mostra todos os items ate uma determinada hora


void printItems(pitems leilao, int count, int type, char filter[], int valor, pstring output);
// Imprime    <id> <nome> <categoria> <preco-base> <preco-compre-ja> <duracao>
// Conforme o valor do type sera filtrada a listagem conforme o filter
// Caso type seja       0 - nao tem filtros, logo o filter é ignorado
//                      1 - filtra por categoria
//                      2 - filtra por vendedor
//                      3 - mostra todos os items ate a um valor
//                      4 - mostra todos os items ate uma determinada hora


int addItem(pitems leilao, char *aux, int* count);
// Adiciona um item ao ficheiro de items e a estrutura leilao
// Retorna      0 caso corra tudo bem
//              1 caso tenha chegado ao max de items
//             -1 em caso de erro


int deleteItem(pitems leilao, int remove, int* count);
// Elemina um item
// Retorna  0 caso corra tudo bem
//         -1 caso não encontre o item


int buyItem(pitems leilao, int* count, int id, int oferta, char* licitador);
// Retorna      0 caso seja comprado pelo preço compre já
//              1 caso tenha sido feita uma licitação
//              2 caso da licitação feita ser abaixo do preço do item 
//             -1 caso nao encontre item


int printTime(); 
// Imprime a hora atual em segundos


int loadPromotoresFile(pproms promotores, char * filename, int* count);
// Le o ficheiro de promotores
// Retorna    o número de items lidos
//           -1 em caso de erro


void printProms(pproms promotores, int count);
// Imprime todos os promotores

#endif //SOBAY_FUNCS_H
