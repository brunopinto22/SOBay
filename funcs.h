//
// Created by pinto on 16-11-2022.
//

#ifndef SOBAY_FUNCS_H
#define SOBAY_FUNCS_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_USERS 20
#define MAX_PROMOTORES 10
#define MAX_ITEMS 30


typedef struct items items, *pitems;
struct items{

    int id, preco, preco_ja, durancao;
    char nome[50], categoria[50];

};

typedef struct promotores prom, *pproms;
struct promotores{

    char *nome;

};

int countWords(char* string, int len);
// Le a string e conta as palavras que a mesma contem


int loadItemsFile(char * filename);
// Le o ficheiro de items
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


#endif //SOBAY_FUNCS_H
