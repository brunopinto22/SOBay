//
// Created by pinto on 16-11-2022.
//

#include "funcs.h"

int id = 0;
char* itemsfilepath;        // ficheiro de items
char* promsfilepath;        // ficheiro de promotores
int count_proms = 0;        // nº de promotores
int count_items = 0;      // nº de items
pitems leilao;              // estrutura de items
pproms promotores;          // estrutura de promotores



int getId(){
    id++;
    return id;
}


void closeAllFronts(ponlineusers users, int count){
    union sigval val;
    val.sival_int = 1;
    for(int i=0; i < count; i++)
        sigqueue((users+i)->pid,SIGUSR1, val);   
}


int countWords(char* string, int len){
    if(len == 0)
        return 0;

    int count=0;
    for(int i=0; i <= strlen(string); i++)
        if(*(string + i) == ' ' || *(string + i) == '\0')
            count++;
    return count;
}


void printOnlineUsers(ponlineusers users, int count){
    if(count == 0){
        printf("\033[31m> Não há niguém logado\033[0m\n");
        return;
    }
    printf("\n");
    for(int i=0; i < count; i++)
        printf("> %s [%d]\n", (users+i)->nome, (users+i)->pid);
}


int addOnlineUser(ponlineusers users, onlineuser new, int* count){

    if(*count == MAX_USERS)
        return -1;
    
    int i;
    for(i=0; i < *count; i++)
        if(strcmp(new.nome, (users+i)->nome) == 0)
            return 1;

    strcpy((users+*(count))->nome, new.nome);
    (users+*(count))->pid = new.pid;
    *count += 1;
    return 0;
}


int deleteOnlineUser(ponlineusers users, char* remove, int* count){
    if(*count == 0){
        printf("\033[31m> Não há niguém logado\033[0m\n");
        return -1;
    }

    int pid = 0;
    for(int i=0, y=0; i < *count; i++, y++){
        if(strcmp(remove, (users+i)->nome) == 0){
            y++;
            pid = (users+i)->pid;
        }
        strcpy((users+i)->nome, (users+y)->nome);
        (users+i)->pid = (users+y)->pid;
    }
    *count -= 1;
    return pid;
        
}


int loadItemsFile(char* filename){
    int count_itemss = 0;
    FILE *f = fopen(filename, "rw");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",filename);
        return -1;
    }

    // guarda o nome do ficheiro dos items
    itemsfilepath = filename;

    // contagem dos items do ficheiro
    char aux[500];
    while(fgets(aux, sizeof(aux), f))
        count_itemss++;

    // reabertura do ficheiro
    fclose(f);
    f = fopen(itemsfilepath, "rw");
    if(f == NULL){
        printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n",filename);
        return -1;
    }

    // alocacao da estrutura de dados
    leilao = realloc(leilao,count_itemss*sizeof(item));
    if(leilao == NULL){
        printf("\n\033[31mErro nao foi possivel alocar o leilao em memoria\033[0m\n");
        return -1;
    }
    // guardar a estrutura de dados
    for(int i = 0; i < count_itemss; i++) {
        fgets(aux, sizeof(aux), f);
        sscanf(aux,"%d %s %s %d %d %d %s %s", &(leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, &(leilao + i)->preco, &(leilao + i)->preco_ja, &(leilao + i)->duracao,(leilao + i)->vendedor,(leilao + i)->licitador);
        getId();
    }

    fclose(f);
    return count_itemss;

}


int addItem(char* aux){
    // guarda na estrutura
    int count_items;
    count_items=loadItemsFile("FITEMS");
    
    //count_items++;
    leilao = realloc(leilao,count_items*sizeof(item));
    if(leilao == NULL){
        printf("\n\033[31mErro nao foi possivel alocar o leilao em memoria\033[0m\n");
        //count_items--;
        return -1;
    }
    int i = count_items-1;
    (leilao + i)->id = getId();
    sscanf(aux,"%s %s %d %d %d %s %s", (leilao + i)->nome, (leilao + i)->categoria, &(leilao + i)->preco, &(leilao + i)->preco_ja, &(leilao + i)->duracao,(leilao + i)->vendedor,(leilao + i)->licitador);
   
    // guarda no ficheiro
    FILE *file = fopen(itemsfilepath,"a");
    if(file==NULL){
        printf("\nErro no acesso ao ficheiro\n");
        return -1;
    }
    
    fprintf(file,"\n%d %s %s %d %d %d %s %s", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao,(leilao + i)->vendedor,(leilao + i)->licitador);
    fclose(file);
    
    return 0;
}


void printItems(int type, char* filter,int valor){

    int count_items;
    count_items=loadItemsFile("FITEMS");
    
    if(type==0){
        printf("\n>> Lista de items <<\n");
        printf("\n-->%d",count_items);
        for(int i = 0; i < count_items; i++)
        printf("\n%d: %s %s %d€ %d€ %ds\n", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->categoria, (leilao+i)->preco, (leilao+i)->preco_ja, (leilao+i)->duracao);
    }
    else if(type==1){ //filtra por categoria
        
        for(int i = 0; i < count_items; i++){
            if(strcmp((leilao+i)->categoria,filter)==0){
              printf("\n%d: %s %s %d€ %d€ %ds\n", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->categoria, (leilao+i)->preco, (leilao+i)->preco_ja, (leilao+i)->duracao); 
              
            }
        }
    }
    else if(type==2){ //filtra por vendedor
    
        for(int i = 0; i < count_items; i++){
           
            if(strcmp((leilao+i)->vendedor,filter)==0){
              printf("\n%d: %s %s %d€ %d€ %ds\n", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->categoria, (leilao+i)->preco, (leilao+i)->preco_ja, (leilao+i)->duracao);  
            }
        }
    }
    else if(type==3){ //mostra todos os items ate um valor
    
        for(int i = 0; i < count_items; i++){
            if((leilao+i)->preco <= valor ){
              printf("\n%d: %s %s %d€ %d€ %ds\n", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->categoria, (leilao+i)->preco, (leilao+i)->preco_ja, (leilao+i)->duracao);  
            }
        }    
    }
    else if(type==4){ //mostra todos os items ate uma determinada hora
        for(int i = 0; i < count_items; i++){
            if((leilao+i)->duracao <= valor ){
              printf("\n%d: %s %s %d€ %d€ %ds\n", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->categoria, (leilao+i)->preco, (leilao+i)->preco_ja, (leilao+i)->duracao);  
            }
        }
    }


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

int printTime(){

    time_t mytime;
    mytime = time(NULL);
    struct tm tm = *localtime(&mytime);
   
    return (tm.tm_hour*3600)+tm.tm_sec+(tm.tm_min*60);
}

int buy_item(char * filename,int id, int oferta,char * licitador,int pid){

    int count_items;
    count_items=loadItemsFile("FITEMS");

    for(int i = 0; i < count_items; i++){
        
        if((leilao+i)->id==id){
            if(oferta>=(leilao+i)->preco_ja){
                
                //update saldo do comprador
                //Remover item do ficheiro
                return 0;

            }
            else if(oferta >= (leilao+i)->preco && oferta < (leilao+i)->preco_ja){   
                //guardar oferta na struct
                //atualizar ficheiro de items com nome do licitador
                return 1;
            }
            else if(oferta<(leilao+i)->preco){
                return -1;
            }
        }
    }

} 


