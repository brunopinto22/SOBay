//
// Created by pinto on 16-11-2022.
//

#ifndef SOBAY_FUNCS_H
#define SOBAY_FUNCS_H

#include <stdio.h>
#include <stdlib.h>

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
    char command[7], arguments[150];
    char user[50], pass[50];

};

typedef struct onlineusers onlineusers, *ponlineusers;
struct onlineusers{

    int pid;
    char nome[100];

};

typedef struct items item, *pitems;
struct items{
    pid_t pid;
    int id, duracao;
    char nome[50], categoria[50];
    int preco, preco_ja,envio;
    pitems proximo;
   
};

typedef struct promotores prom, *pproms;
struct promotores{

    char *nome;

};


// funcoes

int getId();
// retorna o id para o novo item

int countWords(char* string, int len);
// Le a string e conta as palavras que a mesma contem


int loadItemsFile(char * filename);
// Le o ficheiro de items
// Guarda os items na estrututra leilao
// Retorna    o número de items lidos
//           -1 em caso de erro


void printItems(int type, char* filter);
// Le o ficheiro de items
// Imprime    <id> <nome> <categoria> <preco-base> <preco-compre-ja> <duracao>
// Conforme o valor do type sera filtrada a listagem conforme o filter
// Caso type seja       0 - nao tem filtros, logo o filter é ignorado
//                      1 - filtra por categoria
//                      2 - filtra por vendedor
//                      3 - mostra todos os items ate a um valor
//                      4 - mostra todos os items ate a uma determinada hora

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


void printTime(int filter); // implementar
// Imprime a hora atual


int loadPromotoresFile(char * filename);
// Le o ficheiro de promotores
// Retorna    o número de items lidos
//           -1 em caso de erro

//void preenche_itens(items p);

#endif //SOBAY_FUNCS_H
