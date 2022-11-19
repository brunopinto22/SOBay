//
// Created by pinto on 16-11-2022.
//

#include "funcs.h"
#include <string.h>


char* itemsfilepath;        // ficheiro de items
char* promsfilepath;        // ficheiro de promotores
int count_proms = 0;        // nº de promotores
int count_items = 0;        // nº de items
pitems leilao = NULL;       // estrutura de items
pproms promotores = NULL;   // estrutura de promotores


int countWords(char* string, int len){
    if(len == 0)
        return 0;

    int count=0;
    for(int i=0; i <= strlen(string); i++)
        if(*(string + i) == ' ' || *(string + i) == '\0')
            count++;
    return count;
}

int loadItemsFile(char* filename){
    FILE *f = fopen(filename, "rw");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",filename);
        return -1;
    }

    // guarda o nome do ficheiro dos items
    itemsfilepath = filename;

    // leitura do ficheiro com a estrutura de dados
    items aux;
    while(fread(&aux, sizeof(items), 1, f))
        count_items++;

    fclose(f);
    f = fopen(filename, "rw");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",filename);
        return -1;
    }

    // alocacao da estrutura de dados
    leilao = realloc(leilao,count_items*sizeof(items));
    if(leilao == NULL){
        printf("\n\033[31mErro nao foi possivel alocar o leilao em memoria\033[0m\n");
        return -1;
    }
    // guardar a estrutura de dados
    for(int i = 0; i < count_items; i++)
        fread((leilao+i), sizeof(items), 1, f);

    fclose(f);
    return count_items;

}

void printItems(int type, char* filter){
    FILE *f = fopen(itemsfilepath, "r");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",itemsfilepath);
        return;
    }

    printf("\n>> Lista de items <<\n");
    for(int i = 0; i < count_items; i++)
        printf("%d: %s %s %d€ %d€ %ds\n", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->categoria, (leilao+i)->preco, (leilao+i)->preco_ja, (leilao+i)->durancao);

    fclose(f);

}

int loadPromotoresFile(char * filename){
    FILE *f = fopen(filename, "rw");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",filename);
        return -1;
    }

    // guarda o nome do ficheiro dos promotores
    promsfilepath = filename;

    // leitura do ficheiro com a estrutura de dados
    prom aux;
    while(fread(&aux, sizeof(prom), 1, f))
        count_proms++;

    fclose(f);
    f = fopen(filename, "rw");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",filename);
        return -1;
    }

    // alocacao da estrutura de dados
    promotores = realloc(promotores,count_proms*sizeof(prom));
    if(promotores == NULL){
        printf("\n\033[31mErro nao foi possivel alocar o leilao em memoria\033[0m\n");
        return -1;
    }
    // guardar a estrutura de dados
    for(int i = 0; i < count_proms; i++)
        fread((promotores+i), sizeof(prom), 1, f);

    fclose(f);
    return count_proms;

}