#include "funcs.h"

#define PID getpid()

int fd_frontend;
char nomeFifoFront[100];

void handlesig(int s, siginfo_t *i, void *v)
{

	int n = i->si_value.sival_int;

	if (n == 1 || n == 2 || n == 0)
	{
		if (n == 1)
			printf("\n\033[31m> O Backend expulsou-o\033[0m\n");
		else if (n == 2)
			printf("\n\033[35m> O Backend fechou\033[0m\n");
		else
		{
			msg mensagem;
			mensagem.pid = PID;
			strcpy(mensagem.command, "exit");
			int fd_backend = open(BACK_FIFO, O_WRONLY);
			if (fd_backend == -1)
			{
				printf("\n\033[31mERRO o servidor não está a correr");
				exit(1);
			}

			s = write(fd_backend, &mensagem, sizeof(msg));
			if (s <= 0)
				printf("\n\033[31mERRO no envio\n");
			close(fd_backend);
		}

		close(fd_frontend);
		unlink(nomeFifoFront);

		printf("\n\033[32mAdeus\033[0m\n");

		exit(0);
	}

	fd_frontend = open(nomeFifoFront, O_RDONLY);
	msg resposta;
	read(fd_frontend, &resposta, sizeof(msg));
	printf("\n\033[35m> %s\033[0m\n\n>> ", resposta.message);
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	int s, res, size;
	fd_set fdsf;
	struct timeval timeout;

	// verificacao da formatacao
	if (argc != 3)
	{
		printf("\n\033[31mERRO na formatação: ./FrontEnd <username> <password>\033[0m\n");
		return 1;
	}

	// criar fifo
	sprintf(nomeFifoFront, FRONT_FIFO, PID);
	if (mkfifo(nomeFifoFront, 0666) == -1)
	{
		printf("\n\033[31mERRO nao foi possivel criar o fifo\033[0m\n");
		return 1;
	}

	char string[150], command[7], arguments[150];
	msg mensagem, resposta;
	int valor, id, duracao;

	mensagem.pid = getpid();
	strcpy(mensagem.command, "login");
	strcpy(mensagem.user, argv[1]);
	strcpy(mensagem.pass, argv[2]);

	// abrir fifo do backend
	int fd_backend = open(BACK_FIFO, O_WRONLY);
	if (fd_backend == -1)
	{
		printf("\n\033[31mERRO o servidor não está a correr\033[0m\n");
		return 1;
	}
	s = write(fd_backend, &mensagem, sizeof(msg));
	if (s <= 0)
		printf("\n\033[31mERRO no envio\n");
	close(fd_backend);

	// receber a resposta
	fd_frontend = open(nomeFifoFront, O_RDONLY);
	if (fd_frontend == -1)
	{
		printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
		return 1;
	}
	read(fd_frontend, &resposta, sizeof(msg));

	if (resposta.value == 1)
	{
		printf("%s", resposta.message);
		return 1;
	}

	// handlers de sinais
	signal(SIGINT, (void (*)(int))handlesig);
	struct sigaction sa;
	sa.sa_sigaction = handlesig;
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);

	printf("\n\033[1;33m⋉  Frontend [%d] ⋊\033[0m\n\n>> ",PID);
	do
	{

		// limpar a vars
		arguments[0] = 0;

		scanf("%[^\n]", string);

		// divisão do comando em duas partes: <comando> <argumentos>
		sscanf(string, "%s %[^\n]", command, arguments);
		setbuf(stdin, NULL);

		strcpy(mensagem.command, command);
		strcpy(mensagem.arguments, arguments);

		// verificação do tipo de comando
		if (strcmp(command, "sell") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 5)
				printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");
			else
			{
				item i;
				strcat(arguments, " ");
				strcat(arguments, mensagem.user);
				strcat(arguments, " -");
				strcpy(mensagem.arguments, arguments);
				if (sscanf(arguments, "%s %s %d %d %d %s %s", i.nome, i.categoria, &i.preco, &i.preco_ja, &i.duracao, i.vendedor, i.licitador) != 0)
				{

					int fd_backend = open(BACK_FIFO, O_WRONLY);
					if (fd_backend == -1)
					{
						printf("\n\033[31mERRO o servidor não está a correr");
						return 1;
					}

					s = write(fd_backend, &mensagem, sizeof(msg));
					if (s <= 0)
						printf("\n\033[31mERRO no envio\n");
					close(fd_backend);

					// receber a resposta
					int fd_frontend = open(nomeFifoFront, O_RDONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
						return 1;
					}
					read(fd_frontend, &resposta, sizeof(msg));

					if (resposta.value == 0)
					{
						printf("\n\033[32mProduto Adicionado com sucesso\033[0m\n");
					}
					else
						printf("\nErro a adicionar o produto\n");
				}
				else
					printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");
			}
		}
		else if (strcmp(command, "list") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 0)
				printf("\n\033[31mERRO na formatação: list \033[0m\n");
			else
			{
				int fd_backend = open(BACK_FIFO, O_WRONLY);
				s = write(fd_backend, &mensagem, sizeof(msg));
				if (s <= 0)
					printf("\n\033[31mERRO no envio\n");
				close(fd_backend);

				// receber a resposta
				int fd_frontend = open(nomeFifoFront, O_RDONLY);
				if (fd_frontend == -1)
				{
					printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
					return 1;
				}
				read(fd_frontend, &resposta, sizeof(msg));

				if (resposta.value == 0)
					printf("\033[31m%s\033[0m", resposta.message);
				else
					for (int i = 0; i < resposta.value; i++)
						printf("%s", resposta.output[i].text);
			}
		}
		else if (strcmp(command, "licat") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 1)
			{
				printf("\n\033[31mERRO na formatação: licat <categoria>\033[0m\n");
			}
			else
			{
				int fd_backend = open(BACK_FIFO, O_WRONLY);
				s = write(fd_backend, &mensagem, sizeof(msg));
				if (s <= 0)
					printf("\n\033[31mERRO no envio\n");
				close(fd_backend);

				// receber a resposta
				int fd_frontend = open(nomeFifoFront, O_RDONLY);
				if (fd_frontend == -1)
				{
					printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
					return 1;
				}
				read(fd_frontend, &resposta, sizeof(msg));

				if (resposta.value == 0)
					printf("\033[31m%s\033[0m", resposta.message);
				else
					for (int i = 0; i < resposta.value; i++)
						printf("%s", resposta.output[i].text);
			}
		}
		else if (strcmp(command, "lisel") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 1)
				printf("\n\033[31mERRO na formatação: lisel <username-do-vendedor>\033[0m\n");
			else
			{
				int fd_backend = open(BACK_FIFO, O_WRONLY);
				s = write(fd_backend, &mensagem, sizeof(msg));
				if (s <= 0)
					printf("\n\033[31mERRO no envio\n");
				close(fd_backend);

				// receber a resposta
				int fd_frontend = open(nomeFifoFront, O_RDONLY);
				if (fd_frontend == -1)
				{
					printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
					return 1;
				}
				read(fd_frontend, &resposta, sizeof(msg));

				if (resposta.value == 0)
					printf("\033[31m%s\033[0m", resposta.message);
				else
					for (int i = 0; i < resposta.value; i++)
						printf("%s", resposta.output[i].text);
			}
		}
		else if (strcmp(command, "lival") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 1)
				printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");
			else
			{
				int fd_backend = open(BACK_FIFO, O_WRONLY);
				s = write(fd_backend, &mensagem, sizeof(msg));
				if (s <= 0)
					printf("\n\033[31mERRO no envio\n");
				close(fd_backend);

				// receber a resposta
				int fd_frontend = open(nomeFifoFront, O_RDONLY);
				if (fd_frontend == -1)
				{
					printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
					return 1;
				}
				read(fd_frontend, &resposta, sizeof(msg));

				if (resposta.value == 0)
					printf("\033[31m%s\033[0m", resposta.message);
				else
					for (int i = 0; i < resposta.value; i++)
						printf("%s", resposta.output[i].text);
			}
		}
		else if (strcmp(command, "litime") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 1)
				printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");
			else
			{
				int fd_backend = open(BACK_FIFO, O_WRONLY);
				s = write(fd_backend, &mensagem, sizeof(msg));
				if (s <= 0)
					printf("\n\033[31mERRO no envio\n");
				close(fd_backend);

				// receber a resposta
				int fd_frontend = open(nomeFifoFront, O_RDONLY);
				if (fd_frontend == -1)
				{
					printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
					return 1;
				}
				read(fd_frontend, &resposta, sizeof(msg));

				if (resposta.value == 0)
					printf("\033[31m%s\033[0m", resposta.message);
				else
					for (int i = 0; i < resposta.value; i++)
						printf("%s", resposta.output[i].text);
			}
		}
		else if (strcmp(command, "time") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 0)
				printf("\n\033[31mERRO na formatação: time\033[0m\n");
			else
			{
				printf("\nHora atual em segundos\n");
				printf("%d", printTime());
			}
		}
		else if (strcmp(command, "buy") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 2)
				printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
			else
			{
				if (sscanf(arguments, "%d %d", &mensagem.id_offer, &mensagem.offer) != 0)
				{

					int fd_backend = open(BACK_FIFO, O_WRONLY);
					if (fd_backend == -1)
					{
						printf("\n\033[31mERRO o servidor não está a correr");
						return 1;
					}

					s = write(fd_backend, &mensagem, sizeof(msg));
					if (s <= 0)
						printf("\n\033[31mERRO no envio\n");
					close(fd_backend);

					// receber a resposta
					int fd_frontend = open(nomeFifoFront, O_RDONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
						return 1;
					}
					read(fd_frontend, &resposta, sizeof(msg));

					if (resposta.value == 0)
					{
						printf("\n\033[32mComprou o item %d\033[0m\n", resposta.id_offer);
					}
					else if (resposta.value == 1)
					{
						printf("\n\033[32mLicitou o item %d com %d€\033[0m\n", resposta.id_offer, resposta.offer);
					}
					else if(resposta.value == -2)
					{
						printf("\n\033[31mNão tem fundos suficientes para a licitação\033[0m\n");
					}
					else
					{
						printf("\n\033[31mLicitação inválida\033[0m\n");
					}
				}

				else
					printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");
			}
		}
		else if (strcmp(command, "cash") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 0)
				printf("\n\033[31mERRO na formatação: cash\033[0m\n");
			else
			{

				int fd_backend = open(BACK_FIFO, O_WRONLY);
				if (fd_backend == -1)
				{
					printf("\n\033[31mERRO o servidor não está a correr");
					return 1;
				}

				s = write(fd_backend, &mensagem, sizeof(msg));
				if (s <= 0)
					printf("\n\033[31mERRO no envio\n");
				close(fd_backend);

				// receber a resposta
				int fd_frontend = open(nomeFifoFront, O_RDONLY);
				if (fd_frontend == -1)
				{
					printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
					return 1;
				}
				read(fd_frontend, &resposta, sizeof(msg));
				printf("\n\033[32mSaldo: %d€\033[0m\n", resposta.cash);

			}
		}
		else if (strcmp(command, "add") == 0)
		{
			if (countWords(arguments, strlen(arguments)) != 1)
				printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
			else
			{
				if (sscanf(arguments, "%d", &mensagem.valor_add) != 0)
				{

					int fd_backend = open(BACK_FIFO, O_WRONLY);
					if (fd_backend == -1)
					{
						printf("\n\033[31mERRO o servidor não está a correr");
						return 1;
					}

					s = write(fd_backend, &mensagem, sizeof(msg));
					if (s <= 0)
						printf("\n\033[31mERRO no envio\n");
					close(fd_backend);

					// receber a resposta
					int fd_frontend = open(nomeFifoFront, O_RDONLY);
					if (fd_frontend == -1)
					{
						printf("\n\033[31mERRO nao foi possivel abrir o fifo\n");
						return 1;
					}
					read(fd_frontend, &resposta, sizeof(msg));

					if (resposta.value == 0)
						printf("\n\033[32mSaldo adicionado com sucesso\033[0m\n");
					else
						printf("\n\033[31mErro a adicionar o saldo\033[0m\n");
				}
				else
					printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");
			}
		}
		else if (strcmp(command, "help") == 0)
			printHelp_Front();
		else if (strcmp(command, "exit") == 0)
		{
			union sigval val;
			val.sival_int = 0;
			sigqueue(PID, SIGINT, val);
		}
		else
			printf("\nERRO - comando nao existe\n");

		printf("\n>> ");

	} while (1);
}
