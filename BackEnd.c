#include "funcs.h"

#define PID getpid()

int fd_backend;
onlineuser online[MAX_USERS];			// estrutura de utilizadores online
int online_count = 0;			 				// nº de utilizadores online
item leilao[MAX_ITEMS];			 			// estrutura de items
int count_items = 0;			 				// nº de items
prom promotores[MAX_PROMOTORES]; 	// estrutura de promotores
int count_proms = 0;			 				// nº de promotores
int systemtime;

void closeBack()
{
	if (online_count != 0)
		closeAllFronts(online, online_count);

	updateItemsFile(leilao, count_items);
	close(fd_backend);
	unlink(BACK_FIFO);
	
	updateTime(systemtime);	

	printf("\n\033[32mAdeus\033[0m\n");
	exit(0);
}

int main()
{
	// vars
	char string[150], command[7], arguments[150];
	char nomeFifoFront[100];
	int value, size, v, rm;
	msg mensagem;
	strings output[MAX_ITEMS];
	int count_output;
	int balance;
	onlineuser aux;
	item auxit;
	int res;
	fd_set fds;
	struct timeval timeout;
	systemtime = loadTime();

	setbuf(stdout, NULL);
	setbuf(stdin, NULL);

	char *FPROMOTORES = "FPROMOTORES";
	// Verificar se existe
	if (!getenv(FPROMOTORES))
	{
		printf("\n\033[31mERRO variavel de ambiente nao existe: %s\033[0m\n", FPROMOTORES);
		exit(1);
	}
	char *FUSERS = "FUSERS";

	// Verificar se existe
	if (!getenv(FUSERS))
	{
		printf("\n\033[31mERRO variavel de ambiente nao existe: %s\033[0m\n", FUSERS);
		exit(1);
	}
	char *FITEMS = "FITEMS";
	// Verificar se existe
	if (!getenv(FITEMS))
	{
		printf("\n\033[31mERRO variavel de ambiente nao existe: %s\033[0m\n", FITEMS);
		exit(1);
	}
	// Carregar Promotores
	if (loadPromotoresFile(promotores, FPROMOTORES, &count_proms) == -1)
	{
		printf("\n\033[31mERRO Não foi possível abrir o ficheiro de Promotores\033[0m\n");
		exit(1);
	}
	// Carregar Utilizadores
	if (loadUsersFile(FUSERS) == -1)
	{
		printf("\n\033[31m%s\033[0m", getLastErrorText());
		exit(1);
	}
	// Carregar Items
	if (loadItemsFile(leilao, FITEMS, &count_items) == -1)
	{
		printf("\n\033[31mERRO Não foi possível abrir o ficheiro de Items\033[0m\n");
		exit(1);
	}

	// abertura do fifo do backend para leitura
	if (mkfifo(BACK_FIFO, 0666) == -1)
	{
		if (errno == EEXIST)
		{
			printf("\n\033[31mERRO Servidor em execução\n");
			exit(1);
		}
		printf("\n\033[31mERRO nao foi possivel criar o fifo\n");
		exit(1);
	}
	fd_backend = open(BACK_FIFO, O_RDWR);
	if (fd_backend == -1)
	{
		printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
		exit(1);
	}

	// handlers de sinais
	signal(SIGINT, closeBack);

	printf("\n\033[1;35m⋉  Backend [%d] ⋊\033[0m\n\n>> ",PID);
	do
	{
		// limpar a vars
		FD_ZERO(&fds);
		arguments[0] = 0;

		// canais para estar atento
		FD_SET(0, &fds);
		FD_SET(fd_backend, &fds);

		// set do timeout
		timeout.tv_sec = 1; // Timeout de 1 segundo

		res = select(fd_backend + 1, &fds, NULL, NULL, &timeout);
		if (res == -1)
		{
			printf("Occoreu um erro no select\n");
			close(fd_backend);
			unlink(BACK_FIFO);
			exit(1);
		}
		else if (res > 0 && FD_ISSET(0, &fds))
		{
			// ler os comandos do admin
			scanf("%[^\n]", string);

			// divisão do comando em duas partes: <comando> <argumentos>
			sscanf(string, "%s %[^\n]", command, arguments);
			setbuf(stdin, NULL);

			value = verifyCmdBack(command, arguments); // verifica comando

			// verificação do tipo de comando
			if(value == 1){
				printOnlineUsers(online, online_count);

			} else if(value == -1){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: users \033[0m\n");

			} else if(value == 2){
				count_output = countItems(leilao, count_items, 0, " ", 0);
				if (count_output == 0)
					printf("\n\033[31m> Não há items\033[0m\n");
				else
				{
					printItems(leilao, count_items, 0, " ", 0, output);
					for (int x = 0; x < count_output; x++)
						printf("%s", (output + x)->text);
				}
				
			} else if(value == -2){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: list \033[0m\n");

			} else if(value == 3){
				int i = deleteOnlineUser(online, arguments, 0, &online_count);
				if (i > 0)
					printf("\n\033[35m> User [%d] : foi expluso\033[0m\n", i);
				union sigval val;
				val.sival_int = 1;
				sigqueue(i, SIGUSR1, val);

			} else if(value == -3){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: kick <username>\033[0m\n");

			} else if(value == 4){
				printProms(promotores, count_proms);

			} else if(value == -4){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: prom\033[0m\n");

			} else if(value == 5){
				printf("\nA ler ficheiro de promotores...\n");

			} else if(value == -5){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: reprom \033[0m\n");

			} else if(value == 6){
				printf("\nA terminar promotor...\n");

			} else if(value == -6){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: cancel <nome-do-executável-do-promotor>\033[0m\n");

			} else if(value == 7){
				printf("\033[35m\n> Tempo do sistema: %ds\033[0m\n", systemtime);

			} else if(value == -7){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: time\033[0m\n");

			} else if(value == 8){
				printHelp_Back();

			} else if(value == -8){ // erro de formatacao
				printf("\n\033[31mERRO na formatação: help\033[0m\n");

			} else if(value == 9){
				closeBack();
			} else if(value == -9){
				printf("\n\033[31mERRO na formatação: close\033[0m\n");
			} else
				printf("\n\033[31mERRO - Comando não existe\033[0m\n");

			printf("\n>> ");

		}
		else if (res > 0 && FD_ISSET(fd_backend, &fds))
		{
			// ler pedidos do frontend
			size = read(fd_backend, &mensagem, sizeof(msg));

			if (size > 0)
			{
				sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);

				value = verifyCmdFront(mensagem.command, mensagem.arguments);

				// verificação do tipo de comando
				if(strcmp(mensagem.command, "login") == 0){
					v = isUserValid(mensagem.user, mensagem.pass);

					if(v == -1){
						printf("\n\033[31m%s\033[0m", getLastErrorText());
						exit(1);
					}
					else if(v == 0){
						printf("\n\033[31m> User [%d] : Conta não existe ou a password está errada\033[0m", mensagem.pid);
						strcpy(mensagem.message, "\n\033[31m> Conta não existe ou a password está errada\033[0m\n");
						mensagem.value = 1;

						// envia mensagem
						size = sendto(mensagem, nomeFifoFront); 
						if(size < 0)
							printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

					} else{

						aux.pid = mensagem.pid;
						strcpy(aux.nome, mensagem.user);
						int i = addOnlineUser(online, aux, &online_count);

						if (i == 1 || i == -1){
							if (i == 1){
								strcpy(mensagem.message, "\n\033[31m>Já esta Logado\033[0m\n");
								printf("\n\033[31m> User [%d] : Já esta Logado\033[0m", mensagem.pid);
							}
							else{
								strcpy(mensagem.message, "\n\033[31m>O Máximo de Utilizadores foi atingido\033[0m\n");
								printf("\n\033[31m> User [%d] : O Máximo de Utilizadores foi atingido\033[0m", mensagem.pid);
							}
							mensagem.value = 1;

							// envia mensagem
							size = sendto(mensagem, nomeFifoFront); 
							if(size < 0)
								printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

						}
						else{
							mensagem.value = 0;
							printf("\n\033[32m> User [%d] : Fez login\033[0m\n", mensagem.pid);

							// envia mensagem
							size = sendto(mensagem, nomeFifoFront); 
							if(size < 0)
								printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

						}
					}
				} else if(value == 1){
					strcat(mensagem.arguments, " ");
					strcat(mensagem.arguments, mensagem.user);

					if (addItem(leilao, mensagem.arguments, &count_items) == 0){
						mensagem.value = 0;
						printf("\n\033[33m> User [%d] : Licitou <%s>\033[0m\n", mensagem.pid, mensagem.arguments);
					}
					else
						mensagem.value = 1;

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 2){

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão\033[0m\n", mensagem.pid);

					mensagem.value = countItems(leilao, count_items, 0, "", 0);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else{
						printItems(leilao, count_items, 0, " ", 0, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);
					
				} else if(value == 3){

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão da categoria %s\033[0m\n", mensagem.pid, mensagem.arguments);

					mensagem.value = countItems(leilao, count_items, 1, mensagem.arguments, 0);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else{
						printItems(leilao, count_items, 1, mensagem.arguments, 0, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 4){

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão do vendedor %s\033[0m\n", mensagem.pid, mensagem.arguments);

					mensagem.value = countItems(leilao, count_items, 2, mensagem.arguments, 0);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else{
						printItems(leilao, count_items, 2, mensagem.arguments, 0, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 5){

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão com valores <= %s€\033[0m\n", mensagem.pid, mensagem.arguments);

					sscanf(mensagem.arguments, "%d", &v);
					mensagem.value = countItems(leilao, count_items, 3, "", v);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else{
						printItems(leilao, count_items, 3, "", v, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}
					
					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 6){

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão com prazo <= %ss\033[0m\n", mensagem.pid, mensagem.arguments);

					sscanf(mensagem.arguments, "%d", &v);
					mensagem.value = countItems(leilao, count_items, 4, "", v);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else{
						printItems(leilao, count_items, 4, "", v, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 7){

					balance = getUserBalance(mensagem.user);
					if (balance - mensagem.offer < 0){
						mensagem.value = -2;
						printf("\n\033[31m> User [%d] : Não tem fundos para a licitação\033[0m\n", mensagem.pid);
					}
					else{
						auxit = getItembyId(leilao, count_items, mensagem.id_offer);
						int i = buyItem(leilao, &count_items, mensagem.id_offer, mensagem.offer, mensagem.user);
						if (i == 0){
							mensagem.value = 0;
							printf("\n\033[33m> User [%d] : Comprou o item %d\033[0m\n", mensagem.pid, mensagem.id_offer);
							updateItemsFile(leilao, count_items);

							// atualiza o saldo do licitador
							updateUserBalance(mensagem.user, balance - mensagem.offer);
							sprintf(mensagem.message, "O Utilizador %s comprou o item %d", mensagem.user, mensagem.id_offer);
							notificaAllFronts(online, online_count, mensagem.message, mensagem.pid);

							// atualiza o saldo do vendedor
							balance = getUserBalance(auxit.vendedor);
							updateUserBalance(auxit.vendedor, balance + mensagem.offer);

							saveUsersFile(FUSERS);

						}
						else if (i == 1){
							mensagem.value = 1;
							printf("\n\033[33m> User [%d] : Licitou %d€ no item %d\033[0m\n", mensagem.pid, mensagem.offer, mensagem.id_offer);
							updateItemsFile(leilao, count_items);
						}
						else if (i == 2){
							mensagem.value = 2;
							printf("\n\033[31m> User [%d] : Efetuou uma licitação menor que a atual\033[0m\n", mensagem.pid);
						}
						else{
							mensagem.value = -1;
							printf("\n\033[31m> User [%d] : Efetuou uma licitação inválida : Item %d não existe\033[0m\n", mensagem.pid, mensagem.id_offer);
						}
					}

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);
					
				} else if(value == 8){

					mensagem.value = 0;
					mensagem.cash = getUserBalance(mensagem.user);
					printf("\n\033[33m> User [%d] : Consultou o saldo\033[0m\n", mensagem.pid);
					
					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 9){
					
					if (loadUsersFile(FUSERS) != 1){
						mensagem.value = 0;
						balance = getUserBalance(mensagem.user);
						updateUserBalance(mensagem.user, mensagem.valor_add + balance);
						saveUsersFile(FUSERS);
						printf("\n\033[33m> User [%d] : Adicionou %d €\033[0m\n", mensagem.pid, mensagem.valor_add);
					}
					else
						mensagem.value = 1;

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);
					
				} else if(value == 10){

					printf("\n\033[33m> User [%d] : Quer saber o tempo\033[0m\n", mensagem.pid);
					mensagem.value = systemtime;

					// envia mensagem
					size = sendto(mensagem, nomeFifoFront); 
					if(size < 0)
						printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

				} else if(value == 11){
					printf("\n\033[33m> User [%d] : Precisa de ajuda\033[0m\n", mensagem.pid);

				} else if(value == 12){

					deleteOnlineUser(online, mensagem.user, mensagem.pid, &online_count);
					printf("\n\033[33m> User [%d] : Desconectou-se\033[0m\n", mensagem.pid);

				}

				printf("\n>> ");

			}
		}

		// update de tempo
		systemtime++;
		for(int i=0; i<count_items; i++)
			leilao[i].duracao--;

		// verifica se algum item expirou
		rm = verifyItems(leilao, &count_items, output);
		if(rm != 0){
			printf("\n> %d items expirarão\n",rm);
			for(int i=0; i<rm; i++){
				sscanf(output[i].text, "%d %s %s %s %d", &auxit.id, auxit.nome, auxit.vendedor, auxit.licitador, &auxit.ofertamax);

				if(auxit.ofertamax == 0) // nao foi vendido
					sprintf(mensagem.message, "O Item %s expirou e não foi vendido", auxit.nome);
				else{ // foi vendido

					balance = getUserBalance(auxit.licitador);

					if(balance - auxit.ofertamax > 0){
						sprintf(mensagem.message, "Comprou o item %d", auxit.id);
						printf("\n\033[33m> User [%d] : Comprou o item %d\033[0m\n", mensagem.pid, mensagem.id_offer);
						updateUserBalance(auxit.licitador, balance - auxit.ofertamax);
						mensagem.pid = getUserPid(online, online_count, auxit.licitador);

						// atualiza o saldo do vendedor
						balance = getUserBalance(auxit.vendedor);
						printf("\n - %d - %s -", balance, auxit.vendedor);
						updateUserBalance(auxit.vendedor, balance + auxit.ofertamax);

						saveUsersFile(FUSERS);

						union sigval val;
						val.sival_int = 4;
						sigqueue(mensagem.pid, SIGUSR1, val);

						// envia mensagem
						sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
						size = sendto(mensagem, nomeFifoFront); 
						if(size < 0)
							printf("\n\033[31mERRO não foi possível abrir o fifo do User [%d]\n", mensagem.pid);

						sprintf(mensagem.message, "O Utilizador %s comprou o item %d", auxit.licitador, auxit.id);
					}

				}

				mensagem.pid = getUserPid(online, online_count, auxit.licitador);
				notificaAllFronts(online, online_count, mensagem.message, mensagem.pid);
			}
			
			updateItemsFile(leilao, count_items);
			printf("\n>> ");
		}

	} while (1);
	
}