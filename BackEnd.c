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
	int size, v, rm;
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

			// verificação do tipo de comando
			if (strcmp(command, "users") == 0)
			{
				if (countWords(arguments, strlen(arguments)) != 0)
					printf("\n\033[31mERRO na formatação: users \033[0m\n");
				else
					printOnlineUsers(online, online_count);
			}
			else if (strcmp(command, "list") == 0)
			{
				if (countWords(arguments, strlen(arguments)) != 0)
					printf("\n\033[31mERRO na formatação: list \033[0m\n");
				else
				{

					count_output = countItems(leilao, count_items, 0, " ", 0);
					if (count_output == 0)
						printf("\n\033[31m> Não há items\033[0m\n");
					else
					{
						printItems(leilao, count_items, 0, " ", 0, output);
						for (int x = 0; x < count_output; x++)
							printf("%s", (output + x)->text);
					}
				}
			}
			else if (strcmp(command, "kick") == 0)
			{
				if (countWords(arguments, strlen(arguments)) != 1)
					printf("\n\033[31mERRO na formatação: kick <username>\033[0m\n");
				else
				{
					int i = deleteOnlineUser(online, arguments, 0, &online_count);
					if (i > 0)
						printf("\n\033[35m> User [%d] : foi expluso\033[0m\n", i);
					union sigval val;
					val.sival_int = 1;
					sigqueue(i, SIGUSR1, val);
				}
			}
			else if (strcmp(command, "prom") == 0)
			{
				if (countWords(arguments, strlen(arguments)) != 0)
					printf("\n\033[31mERRO na formatação: prom \033[0m\n");
				else
					printProms(promotores, count_proms);
			}
			else if (strcmp(command, "reprom") == 0)
			{
				if (countWords(arguments, strlen(arguments)) != 0)
					printf("\n\033[31mERRO na formatação: prom \033[0m\n");
				else
				{
					printf("\nA ler ficheiro de promotores...\n");
				}
			}
			else if (strcmp(command, "cancel") == 0)
			{
				if (countWords(arguments, strlen(arguments)) != 1)
					printf("\n\033[31mERRO na formatação: cancel <nome-do-executável-do-promotor>\033[0m\n");
				else
				{
					printf("\nA terminar promotor...\n");
				}
			}
			else if(strcmp(command, "time") == 0)
				printf("\033[35m\n> Tempo do sistema: %ds\033[0m\n", systemtime);
			else if (strcmp(command, "help") == 0)
				printHelp_Back();
			else if (strcmp(command, "close") == 0){
				updateItemsFile(leilao, count_items);
				closeBack();
			}
			else
				printf("\nERRO - comando nao existe\n");

			printf("\n>> ");
		}
		else if (res > 0 && FD_ISSET(fd_backend, &fds))
		{
			// ler pedidos do frontend
			size = read(fd_backend, &mensagem, sizeof(msg));

			if (size > 0)
			{

				if (strcmp(mensagem.command, "login") == 0)
				{

					if (isUserValid(mensagem.user, mensagem.pass) == -1)
					{
						printf("\n\033[31m%s\033[0m", getLastErrorText());
						exit(1);
					}
					else if (isUserValid(mensagem.user, mensagem.pass) == 0)
					{
						printf("\n\033[31m> User [%d] : Conta nao existe\033[0m", mensagem.pid);
						strcpy(mensagem.message, "\n\033[31m>Conta nao existe\033[0m\n");
						mensagem.value = 1;

						// resposta ao frontend
						sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}
						int s2 = write(fd_frontend, &mensagem, sizeof(msg));
						close(fd_frontend);
					}
					else
					{

						aux.pid = mensagem.pid;
						strcpy(aux.nome, mensagem.user);
						int i = addOnlineUser(online, aux, &online_count);

						if (i == 1 || i == -1)
						{
							if (i == 1)
								strcpy(mensagem.message, "\n\033[31m>Já esta Logado\033[0m\n");
							else
								strcpy(mensagem.message, "\n\033[31m>O Máximo de Utilizadores foi atingido\033[0m\n");

							printf("\n\033[31m> User [%d] : %s\033[0m", mensagem.pid, mensagem.message);

							mensagem.value = 1;

							// resposta ao frontend
							sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
							int fd_frontend = open(nomeFifoFront, O_WRONLY);
							if (fd_frontend == -1)
							{
								printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
								exit(1);
							}
							int s2 = write(fd_frontend, &mensagem, sizeof(msg));
							close(fd_frontend);
						}
						else
						{
							mensagem.value = 0;
							printf("\n\033[32m> User [%d] : Fez login\033[0m\n", mensagem.pid);

							// resposta ao frontend
							sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
							int fd_frontend = open(nomeFifoFront, O_WRONLY);
							if (fd_frontend == -1)
							{
								printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
								exit(1);
							}
							int s2 = write(fd_frontend, &mensagem, sizeof(msg));
							close(fd_frontend);
						}
					}
				}
				else if (strcmp(mensagem.command, "sell") == 0)
				{
					if (addItem(leilao, mensagem.arguments, &count_items) == 0)
					{
						mensagem.value = 0;
						printf("\n\033[33m> User [%d] : Licitou <%s>\033[0m\n", mensagem.pid, mensagem.arguments);
					}
					else
						mensagem.value = 1;

					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "cash") == 0)
				{

					mensagem.value = 0;
					mensagem.cash = getUserBalance(mensagem.user);
					printf("\n\033[33m> User [%d] : Consultou o saldo\033[0m\n", mensagem.pid);
					
					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "add") == 0)
				{

					if (loadUsersFile(FUSERS) != 1)
					{
						mensagem.value = 0;
						balance = getUserBalance(mensagem.user);
						updateUserBalance(mensagem.user, mensagem.valor_add + balance);
						saveUsersFile(FUSERS);
						printf("\n\033[33m> User [%d] : Adicionou %d €\033[0m\n", mensagem.pid, mensagem.valor_add);
					}
					else
					{
						mensagem.value = 1;
					}

					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "list") == 0)
				{

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão\033[0m\n", mensagem.pid);

					mensagem.value = countItems(leilao, count_items, 0, "", 0);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else
					{
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}

						printItems(leilao, count_items, 0, " ", 0, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}
					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "licat") == 0)
				{

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão da categoria %s\033[0m\n", mensagem.pid, mensagem.arguments);

					mensagem.value = countItems(leilao, count_items, 1, mensagem.arguments, 0);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else
					{
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}

						printItems(leilao, count_items, 1, mensagem.arguments, 0, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}
					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "lisel") == 0)
				{

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão do vendedor %s\033[0m\n", mensagem.pid, mensagem.arguments);

					mensagem.value = countItems(leilao, count_items, 2, mensagem.arguments, 0);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else
					{
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}

						printItems(leilao, count_items, 2, mensagem.arguments, 0, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}
					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "lival") == 0)
				{

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão com valores <= %s€\033[0m\n", mensagem.pid, mensagem.arguments);

					sscanf(mensagem.arguments, "%d", &v);
					mensagem.value = countItems(leilao, count_items, 3, "", v);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else
					{
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}

						printItems(leilao, count_items, 3, "", v, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}
					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "litime") == 0)
				{

					printf("\n\033[33m> User [%d] : Fez a listagem do leilão com prazo <= %ss\033[0m\n", mensagem.pid, mensagem.arguments);

					sscanf(mensagem.arguments, "%d", &v);
					mensagem.value = countItems(leilao, count_items, 4, "", v);
					if (mensagem.value == 0)
						strcpy(mensagem.message, "Não foram encontrados items");
					else
					{
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}

						printItems(leilao, count_items, 4, "", v, output);
						for (int x = 0; x < mensagem.value; x++)
							strcpy(mensagem.output[x].text, (output + x)->text);
					}
					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
				}
				else if (strcmp(mensagem.command, "time") == 0)
				{
					printf("\n\033[33m> User [%d] : Quer saber o tempo\033[0m\n", mensagem.pid);
					mensagem.value = systemtime;

					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);

				}
				else if (strcmp(mensagem.command, "exit") == 0)
				{
					deleteOnlineUser(online, mensagem.user, mensagem.pid, &online_count);
					printf("\n\033[33m> User [%d] : Desconectou-se\033[0m\n", mensagem.pid);
				}
				else if (strcmp(mensagem.command, "buy") == 0)
				{
					balance = getUserBalance(mensagem.user);
					if (balance - mensagem.offer < 0){
						mensagem.value = -2;
						printf("\n\033[31m> User [%d] : Não tem fundos para a licitação\033[0m\n", mensagem.pid);
					}
					else
					{
						int i = buyItem(leilao, &count_items, mensagem.id_offer, mensagem.offer, mensagem.user);
						if (i == 0)
						{
							mensagem.value = 0;
							printf("\n\033[33m> User [%d] : Comprou o item %d\033[0m\n", mensagem.pid, mensagem.id_offer);
							updateItemsFile(leilao, count_items);
							updateUserBalance(mensagem.user, balance - mensagem.offer);
							saveUsersFile(FUSERS);
							sprintf(mensagem.message, "O Utilizador %s comprou o item %d", mensagem.user, mensagem.id_offer);
							notificaAllFronts(online, online_count, mensagem.message, mensagem.pid);
						}
						else if (i == 1)
						{
							mensagem.value = 1;
							printf("\n\033[33m> User [%d] : Licitou %d€ no item %d\033[0m\n", mensagem.pid, mensagem.offer, mensagem.id_offer);
							updateItemsFile(leilao, count_items);
						}
						else if (i == 2)
						{
							mensagem.value = 2;
							printf("\n\033[31m> User [%d] : Efetuou uma licitação menor que a atual\033[0m\n", mensagem.pid);
						}
						else
						{
							mensagem.value = -1;
							printf("\n\033[31m> User [%d] : Efetuou uma licitação inválida : Item %d não existe\033[0m\n", mensagem.pid, mensagem.id_offer);
						}
					}

					// resposta ao frontend
					sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
					int fd_frontend = open(nomeFifoFront, O_WRONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
						exit(1);
					}
					int s2 = write(fd_frontend, &mensagem, sizeof(msg));
					close(fd_frontend);
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

				if(auxit.ofertamax == 0)
					sprintf(mensagem.message, "O Item %s expirou e não foi vendido", auxit.nome);
				else{
					balance = getUserBalance(auxit.licitador);

					if(balance - auxit.ofertamax > 0){
						sprintf(mensagem.message, "Comprou o item %d", auxit.id);
						printf("\n\033[33m> User [%d] : Comprou o item %d\033[0m\n", mensagem.pid, mensagem.id_offer);
						updateUserBalance(auxit.licitador, balance - auxit.ofertamax);
						saveUsersFile(FUSERS);
						mensagem.pid = getUserPid(online, online_count, auxit.licitador);

						union sigval val;
						val.sival_int = 4;
						sigqueue(mensagem.pid, SIGUSR1, val);

						// resposta ao frontend
						sprintf(nomeFifoFront, FRONT_FIFO, mensagem.pid);
						int fd_frontend = open(nomeFifoFront, O_WRONLY);
						if (fd_frontend == -1)
						{
							printf("\n\033[31mERRO nao foi possivel abrir o fifo do User [%d]\n", mensagem.pid);
							exit(1);
						}
						int s2 = write(fd_frontend, &mensagem, sizeof(msg));
						close(fd_frontend);
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

	/*//--------------------------------------------------------------------------------------------------------------
	// Meta 1 testes
	int i;
	char username[50], userpass[50];
	int pid, meu_pid, pid_filho;
	int canal[2];
	int n;
	char buf[200];
	printf("deseja testar que funcionalidade?\n\n1-utilizadores\n2-promotor\n3-items\n4-comandos\n\n>> ");
	scanf("%d", &i);
	switch (i) {

			case 1:
					if( loadUsersFile(FUSERS) == -1 ){ printf("\n\033[31m%s\033[0m\n", getLastErrorText()); exit(1);}
					// pede os dados ao utilizador
					printf("login: ");
					scanf("%s %s", username, userpass);
					if(isUserValid(username, userpass) == -1){ printf("\n\033[31m%s\033[0m\n", getLastErrorText()); exit(1);}
					else if(isUserValid(username, userpass) == 0){ printf("\n\033[31mErro conta nao existe\033[0m\n"); exit(1);}
					return 0;

			case 2:
					meu_pid = getpid();
					printf("\nBackend>> O meu PID: %d\n", meu_pid);

					if( loadPromotoresFile(FPROMOTORES) == -1 )
							return -1;

					// verificacao da criacao do pipe
					if(pipe(canal) < 0){
							printf("\n\033[31mErro na criacao do pipe\033[0m\n");
							exit(1);
					}

					pid = fork();
					if(pid == -1){
							printf("\n\033[31mErro no fork()\033[0m\n");
							exit(1);

					} else if(pid == 0){ // Promotor:

							pid_filho = getpid(); // pid do promotor
							printf("\nPromotor>> O meu PID: %d\n", pid_filho);

							// redirecionar stdout para o canal
							close(STDOUT_FILENO);
							dup(canal[1]);
							close(canal[0]);

							execlp("./promotor_oficial", "./promotor_oficial", NULL);
					} else{ // Backend:

							while(1){
									// ler o canal
									n = read(canal[0], buf, 199);
									buf[n] = 0;
									fprintf(stdout,"Promotor>> %s", buf);
									// limpar a memoria
									fflush(stdout);
									memset(buf, 0, 199);
							}

					}
					return 0;

			case 3:
					if( loadItemsFile(FITEMS) == -1 )
							exit(1);
					printItems(0," ");
					return 0;

			case 4:
			break;
	}
	setbuf(stdin,NULL);

 //--------------------------------------------------------------------------------------------------------------
*/
}