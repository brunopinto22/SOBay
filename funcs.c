#include "funcs.h"

int id = 0;
char *itemsfilepath; // ficheiro de items
char *promsfilepath; // ficheiro de promotores

int getId(){
	id++;
	return id;
}

int verifyCmdBack(char cmd[], char args[]){

	if(strcmp(cmd,"users") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -1;
		return 1;

	} else if(strcmp(cmd,"list") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -2;
		return 2;

	} else if(strcmp(cmd,"kick") == 0){
		if (countWords(args, strlen(args)) != 1)
			return -3;
		return 3;

	} else if(strcmp(cmd,"prom") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -4;
		return 4;

	} else if(strcmp(cmd,"reprom") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -5;
		return 5;

	} else if(strcmp(cmd,"cancel") == 0){
		if (countWords(args, strlen(args)) != 1)
			return -6;
		return 6;

	} else if(strcmp(cmd,"time") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -7;
		return 7;

	} else if(strcmp(cmd,"help") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -8;
		return 8;

	} else if(strcmp(cmd,"close") == 0){
		if (countWords(args, strlen(args)) != 0)
			return -9;
		return 9;

	} else
		return 0;

}

int verifyCmdFront(char cmd[], char args[]){
	item i;
	msg mensagem;

	if(strcmp(cmd,"sell") == 0){
		if((countWords(args, strlen(args)) != 5) || (sscanf(args, "%s %s %d %d %d", i.nome, i.categoria, &i.preco, &i.preco_ja, &i.duracao) == 0))
			return -1;
		return 1;

	} else if(strcmp(cmd,"list") == 0){
		if(countWords(args, strlen(args)) != 0)
			return -2;
		return 2;

	} else if(strcmp(cmd,"licat") == 0){
		if((countWords(args, strlen(args)) != 1) || (sscanf(args, "%s", i.categoria) == 0))
			return -3;
		return 3;

	} else if(strcmp(cmd,"lisel") == 0){
		if((countWords(args, strlen(args)) != 1) || (sscanf(args, "%s", i.vendedor) == 0))
			return -4;
		return 4;

	} else if(strcmp(cmd,"lival") == 0){
		if((countWords(args, strlen(args)) != 1) || (sscanf(args, "%d", &i.preco) == 0))
			return -5;
		return 5;

	} else if(strcmp(cmd,"litime") == 0){
		if((countWords(args, strlen(args)) != 1) || (sscanf(args, "%d", &i.duracao) == 0))
			return -6;
		return 6;

	} else if(strcmp(cmd,"buy") == 0){
		if((countWords(args, strlen(args)) != 2) || (sscanf(args, "%d %d", &mensagem.id_offer, &mensagem.offer) == 0))
			return -7;
		return 7;

	} else if(strcmp(cmd,"cash") == 0){
		if(countWords(args, strlen(args)) != 0)
			return -8;
		return 8;

	} else if(strcmp(cmd,"add") == 0){
		if((countWords(args, strlen(args)) != 1) || (sscanf(args, "%d", &i.preco) == 0))
			return -9;
		return 9;

	} else if(strcmp(cmd,"time") == 0){
		if(countWords(args, strlen(args)) != 0)
			return -10;
		return 10;

	} else if(strcmp(cmd,"help") == 0){
		if(countWords(args, strlen(args)) != 0)
			return -11;
		return 11;

	} else if(strcmp(cmd,"exit") == 0){
		if(countWords(args, strlen(args)) != 0)
			return -12;
		return 12;

	} else
		return 0;

}

int sendto(msg mensagem, char tofifo[]){

	int fd = open(tofifo, O_WRONLY);
	if(fd == -1)
		return -1;
	int s = write(fd, &mensagem, sizeof(msg));
	close(fd);
	if (s <= 0)
		return -1;
	return 0;

}


msg recivefrom(char fromfifo[]){
	msg resposta;
	resposta.value = -5;

	int fd = open(fromfifo, O_RDONLY);
	if(fd == -1)
		return resposta;
	read(fd, &resposta, sizeof(msg));

	return resposta;

}

int loadTime(){
	FILE *f = fopen("CONFIG", "r");
	if(f == NULL){ // caso o ficheiro nao exista cria-o
		f = fopen("CONFIG", "w");
		fprintf(f, "0");
		fclose(f);
		return 0;
	}
	int time;
	fscanf(f,"%d",&time);

	return time;
}

void updateTime(int new){
	FILE *f = fopen("CONFIG", "w");
	if(f == NULL){ // caso o ficheiro nao exista
		printf("\n\033[31mERRO não foi possível abrir o ficheiro CONFIG\n");
		return;
	}

	fprintf(f,"%d",new);
	fclose(f);
	return;

}

void printHelp_Back(){
	printf("\n>> Lista de Comandos <<\n");
	printf("\t▹ users\t\t: mostra todos os utilizadores online\n");
	printf("\t▹ list\t\t: lista todos os itens à venda\n");
	printf("\t▹ prom\t\t: lista todos os promotores ativos\n");
	printf("\t▹ reprom\t: atualiza os promotores\n");
	printf("\t▹ cancel\t: cancela a execução de um promotor\n");
	printf("\t▹ kick\t\t: bane um utilizador que está online\n");
	printf("\t▹ time\t\t: obter a hora atual em segundos\n");
	printf("\t▹ close\t\t: fecha o backend\n");
}

void printHelp_Front(){
	printf("\n>> Lista de Comandos <<\n");
	printf("\t▹ sell\t: coloca um item à venda\n");
	printf("\t▹ buy\t: licitar um item\n");
	printf("\t▹ cash\t: consultar o saldo\n");
	printf("\t▹ time\t: obter a hora atual em segundos\n");
	printf("\t▹ exit\t: fecha o frontend\n");
}

void closeAllFronts(ponlineusers users, int count){
	printf("\n\033[35m> A Fechar Frontends : [%d]\033[0m\n", count);

	union sigval val;
	val.sival_int = 2;

	for (int i = 0; i < count; i++)
		sigqueue((users + i)->pid, SIGUSR1, val);
}

void notificaAllFronts(ponlineusers users, int count, char text[], int pid){
	char nomeFifoFront[100];
	msg mensagem;
	mensagem.value = -2;
	int fd_frontend;

	union sigval val;
	val.sival_int = 3;
			
	strcpy(mensagem.message, text);
	
	for (int i = 0; i < count; i++){
		if (pid != (users + i)->pid){
			sigqueue((users + i)->pid, SIGUSR1, val);
			// resposta ao frontend
			sprintf(nomeFifoFront, FRONT_FIFO, (users + i)->pid);
			fd_frontend = open(nomeFifoFront, O_WRONLY);
			if (fd_frontend == -1)
			{
				printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", (users + i)->pid);
				exit(1);
			}
			int s2 = write(fd_frontend, &mensagem, sizeof(msg));
			close(fd_frontend);
		}
	}
}

int countWords(char *string, int len){
	if (len == 0)
		return 0;

	int count = 0;
	for (int i = 0; i <= strlen(string); i++)
		if (*(string + i) == ' ' || *(string + i) == '\0')
			count++;
	return count;
}

void printOnlineUsers(ponlineusers users, int count){
	if (count == 0){
		printf("\033[31m> Não há niguém logado\033[0m\n");
		return;
	}
	printf("\n>> Lista de Utilizadores <<\n");
	for (int i = 0; i < count; i++)
		printf("\033[33m\t▹ User [%d] %s\n\033[0m", (users + i)->pid, (users + i)->nome);
}

int addOnlineUser(ponlineusers users, onlineuser new, int *count){

	if (*count == MAX_USERS)
		return -1;

	int i;
	for (i = 0; i < *count; i++)
		if (strcmp(new.nome, (users + i)->nome) == 0)
			return 1;

	strcpy((users + *(count))->nome, new.nome);
	(users + *(count))->pid = new.pid;
	*count += 1;
	return 0;
}

int deleteOnlineUser(ponlineusers users, char *remove, int rpid, int *count){
	if (*count == 0){
		printf("\033[31m> Não há niguém logado\033[0m\n");
		return -1;
	}

	int pid = 0;
	for (int i = 0, y = 0; i < *count; i++, y++){
		if (strcmp(remove, (users + i)->nome) == 0 || (users + i)->pid == rpid){
			y++;
			pid = (users + i)->pid;
		}
		strcpy((users + i)->nome, (users + y)->nome);
		(users + i)->pid = (users + y)->pid;
	}
	*count -= 1;
	return pid;
}

int getUserPid(ponlineusers users, int count, char name[]){
	if (count == 0){
		printf("\033[31m> Não há niguém logado\033[0m\n");
		return -1;
	}
	for (int i = 0; i < count; i++)
		if (strcmp(name, (users + i)->nome) == 0)
			return (users + i)->pid;
	return 0;
}

int loadItemsFile(pitems leilao, char *filename, int *count)
{
	// guarda o nome do ficheiro dos items
	itemsfilepath = filename;

	FILE *f = fopen(itemsfilepath, "r+");
	if (f == NULL){
		return 0;
	}

	// contagem dos items do ficheiro
	char aux[500];
	while (fgets(aux, sizeof(aux), f))
		*count += 1;

	// reabertura do ficheiro
	fclose(f);
	f = fopen(itemsfilepath, "rw");
	if (f == NULL){
		printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n", filename);
		return -1;
	}

	// guardar a estrutura de dados
	for (int i = 0; i < *count && i < MAX_ITEMS; i++){
		fgets(aux, sizeof(aux), f);
		sscanf(aux, "%d %s %s %d %d %d %s %s %d", &(leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, &(leilao + i)->preco, &(leilao + i)->preco_ja, &(leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador, &(leilao + i)->ofertamax);
		getId();
	}

	fclose(f);
	return *count;
}

int updateItemsFile(pitems leilao, int count){

	FILE *f = fopen(itemsfilepath, "w");
	if (f == NULL){
		printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n", itemsfilepath);
		return -1;
	}

	for (int i = 0; i < count; i++)
		fprintf(f, "%d %s %s %d %d %d %s %s\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador);

	fclose(f);
}

int verifyItems(pitems leilao, int* count, pstring output){
	int y=0;

	for(int i=0; i < *count; i++){
		if((leilao+i)->duracao == 0){
			sprintf((output+y)->text, "%d %s %s %s %d", (leilao+i)->id, (leilao+i)->nome, (leilao+i)->vendedor, (leilao+i)->licitador, (leilao+i)->ofertamax);
			y++;
			deleteItem(leilao, (leilao+i)->id, count);
		}
	}
		
	return y;
			
}

int countItems(pitems leilao, int count, int type, char filter[], int valor){
	int counter = 0;

	if (type == 0){
		for (int i = 0; i < count; i++)
			counter++;
	}
	else if (type == 1){ // filtra por categoria
		for (int i = 0; i < count; i++)
			if (strcmp((leilao + i)->categoria, filter) == 0)
				counter++;
	}
	else if (type == 2){ // filtra por vendedor
		for (int i = 0; i < count; i++)
			if (strcmp((leilao + i)->vendedor, filter) == 0)
				counter++;
	}
	else if (type == 3){ // mostra todos os items ate um valor
		for (int i = 0; i < count; i++)
			if ((leilao + i)->preco <= valor)
				counter++;
	}
	else if (type == 4){ // mostra todos os items ate uma determinada hora
		for (int i = 0; i < count; i++)
			if ((leilao + i)->duracao <= valor)
				counter++;
	}

	return counter;
}

void printItems(pitems leilao, int count, int type, char filter[], int valor, pstring output){

	if (type == 0)
	{
		for (int i = 0; i < count; i++)
			sprintf((output + i)->text, "%d: %s %s %d€ %d€ %ds %s \033[33m%s:%d€\033[0m\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador, (leilao + i)->ofertamax);
	}
	else if (type == 1)
	{ // filtra por categoria

		for (int i = 0, y = 0; i < count; i++)
		{
			if (strcmp((leilao + i)->categoria, filter) == 0)
			{
				sprintf((output + y)->text, "%d: %s %s %d€ %d€ %ds %s \033[33m%s:%d€\033[0m\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador, (leilao + i)->ofertamax);
				y++;
			}
		}
	}
	else if (type == 2)
	{ // filtra por vendedor

		for (int i = 0, y = 0; i < count; i++)
		{

			if (strcmp((leilao + i)->vendedor, filter) == 0)
			{
				sprintf((output + y)->text, "%d: %s %s %d€ %d€ %ds %s \033[33m%s:%d€\033[0m\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador, (leilao + i)->ofertamax);
				y++;
			}
		}
	}
	else if (type == 3)
	{ // mostra todos os items ate um valor

		for (int i = 0, y = 0; i < count; i++)
		{
			if ((leilao + i)->preco <= valor)
			{
				sprintf((output + y)->text, "%d: %s %s %d€ %d€ %ds %s \033[33m%s:%d€\033[0m\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador, (leilao + i)->ofertamax);
				y++;
			}
		}
	}
	else if (type == 4)
	{ // mostra todos os items ate uma determinada hora
		for (int i = 0, y = 0; i < count; i++)
		{
			if ((leilao + i)->duracao <= valor)
			{
				sprintf((output + y)->text, "%d: %s %s %d€ %d€ %ds %s \033[33m%s:%d€\033[0m\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador, (leilao + i)->ofertamax);
				y++;
			}
		}
	}
}

int addItem(pitems leilao, char *aux, int *count)
{

	if (*(count) + 1 >= MAX_ITEMS)
		return 1;

	int i = *count;
	(leilao + i)->id = getId();
	sscanf(aux, "%s %s %d %d %d %s", (leilao + i)->nome, (leilao + i)->categoria, &(leilao + i)->preco, &(leilao + i)->preco_ja, &(leilao + i)->duracao, (leilao + i)->vendedor);
	strcpy((leilao + i)->licitador, "-");
	// guarda no ficheiro
	FILE *file = fopen(itemsfilepath, "a");
	if (file == NULL)
	{
		printf("\n\033[31mERRO no acesso ao ficheiro %s\033[0m\n", itemsfilepath);
		return -1;
	}

	if (*count == 1)
		fprintf(file, "%d %s %s %d %d %d %s %s 0\n", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador);
	else
		fprintf(file, "%d %s %s %d %d %d %s %s 0", (leilao + i)->id, (leilao + i)->nome, (leilao + i)->categoria, (leilao + i)->preco, (leilao + i)->preco_ja, (leilao + i)->duracao, (leilao + i)->vendedor, (leilao + i)->licitador);
	fclose(file);

	*count += 1;
	return 0;
}

int deleteItem(pitems leilao, int remove, int *count)
{
	int p = 0;
	for (int i = 0, y = 0; i < *count; i++, y++)
	{
		if ((leilao + i)->id == remove)
		{
			y++;
			p++;
		}
		(leilao + i)->id = (leilao + i)->id;
		strcpy((leilao + i)->nome, (leilao + y)->nome);
		strcpy((leilao + i)->categoria, (leilao + y)->categoria);
		(leilao + i)->preco = (leilao + y)->preco;
		(leilao + i)->preco_ja = (leilao + y)->preco_ja;
		(leilao + i)->duracao = (leilao + y)->duracao;
		strcpy((leilao + i)->vendedor, (leilao + y)->vendedor);
		strcpy((leilao + i)->licitador, (leilao + y)->licitador);
		(leilao + i)->ofertamax = (leilao + y)->ofertamax;
	}

	if (p = 1)
		*count -= 1;

	return p - 1;
}

int buyItem(pitems leilao, int *count, int id, int oferta, char *licitador)
{
	for (int i = 0; i < *count; i++)
	{
		if ((leilao + i)->id == id)
		{
			if ((leilao + i)->preco_ja <= oferta)
				return deleteItem(leilao, id, count);
			else if ((leilao + i)->ofertamax < oferta)
			{
				(leilao + i)->ofertamax = oferta;
				strcpy((leilao + i)->licitador, licitador);
				return 1;
			}
			return 2;
		}
	}
	return -1;
}

int loadPromotoresFile(pproms promotores, char *filename, int *count)
{
	FILE *f = fopen(filename, "rw");
	if (f == NULL)
	{
		printf("\n\033[31mERRO nao foi possivel abrir o ficheiro: %s\033[0m\n", filename);
		return -1;
	}

	// guarda o nome do ficheiro dos promotores
	promsfilepath = filename;

	// leitura do ficheiro com a estrutura de dados
	char aux[150];
	while (fgets(aux, sizeof(aux), f))
		*count += 1;

	fclose(f);
	f = fopen(filename, "rw");
	if (f == NULL)
	{
		printf("\n\033[31mErro nao foi possivel abrir o ficheiro: %s\033[0m\n", filename);
		return -1;
	}

	// guardar a estrutura de dados
	for (int i = 0; i < *count; i++)
	{
		fgets(aux, sizeof(aux), f);
		strcpy((promotores + i)->nome, aux);
	}

	fclose(f);
	return *count;
}

void printProms(pproms promotores, int count)
{
	printf("\n");
	for (int i = 0; i < count; i++)
		printf("> Promotor [%d] : %s", i + 1, (promotores + i)->nome);
	printf("\n");
}