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

			s = sendto(mensagem, BACK_FIFO); // envia mensagem
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");
		}

		close(fd_frontend);
		unlink(nomeFifoFront);

		printf("\n\033[32mAdeus\033[0m\n");

		exit(0);
	}

	//fd_frontend = open(nomeFifoFront, O_RDONLY);
	msg resposta;
	int size = read(fd_frontend, &resposta, sizeof(msg));
	if(size > 0)
		if(n == 4)
			printf("\n\033[32m> %s\033[0m\n\n>> ", resposta.message);
		else
			printf("\n\033[35m> %s\033[0m\n\n>> ", resposta.message);
		
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	int value, s, res, size;
	fd_set fdsf;
	char string[150], command[7], arguments[150];
	msg mensagem, resposta;
	int valor, id, duracao;
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


	mensagem.pid = getpid();
	strcpy(mensagem.command, "login");
	strcpy(mensagem.user, argv[1]);
	strcpy(mensagem.pass, argv[2]);

	// abrir fifo do backend
	int fd_backend = open(BACK_FIFO, O_WRONLY);
	if (fd_backend == -1){
		printf("\n\033[31mERRO o servidor não está a correr\033[0m\n");
		close(fd_frontend);
		unlink(nomeFifoFront);
		exit(1);
	}
	// envia mensagem
	s = sendto(mensagem, BACK_FIFO); 
	if(s < 0)
		printf("\n\033[31mERRO no envio\n\033[0m");

	// receber a resposta
	fd_frontend = open(nomeFifoFront, O_RDONLY);
	if (fd_frontend == -1){
		printf("\n\033[31mERRO não foi possivel abrir o fifo\n");
		close(fd_frontend);
		unlink(nomeFifoFront);
		exit(1);
	}
	read(fd_frontend, &resposta, sizeof(msg));

	if (resposta.value == 1){
		printf("%s", resposta.message);
		close(fd_frontend);
		unlink(nomeFifoFront);
		exit(1);
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

		value = verifyCmdFront(command, arguments); // verifica comando

		// verificação do tipo de comando
		if(value == 1){

			// envia mensagem
			s = sendto(mensagem, BACK_FIFO); 
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");

			// receber a resposta
			resposta = recivefrom(nomeFifoFront);
			if(resposta.value == -5){
				printf("\n\033[31mERRO na leitura\n\033[0m");
				return 1;
			}

			if (resposta.value == 0)
				printf("\n\033[32mProduto Adicionado com sucesso\033[0m\n");
			else
				printf("\n\033[31mErro a adicionar o produto\033[0m\n");

		} else if(value == -1){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: sell <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\033[0m\n");

		} else if(value == 2 || value == 3 || value == 4 || value == 5 || value == 6){

			// envia mensagem
			s = sendto(mensagem, BACK_FIFO); 
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");

			// receber a resposta
			resposta = recivefrom(nomeFifoFront);
			if(resposta.value == -5){
				printf("\n\033[31mERRO na leitura\n\033[0m");
				return 1;
			}

			if (resposta.value == 0)
				printf("\033[31m%s\033[0m", resposta.message);
			else
				for (int i = 0; i < resposta.value; i++)
					printf("%s", resposta.output[i].text);
			 
		} else if(value == -2){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: list \033[0m\n");

		} else if(value == -3){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: licat <categoria>\033[0m\n");

		} else if(value == -4){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: lisel <username-do-vendedor>\033[0m\n");

		} else if(value == -5){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: lival <preço-máximo>\033[0m\n");

		} else if(value == -6){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: litime <hora-em-segundos>\033[0m\n");

		} else if(value == 7){
			sscanf(arguments, "%d %d", &mensagem.id_offer, &mensagem.offer);

			// envia mensagem
			s = sendto(mensagem, BACK_FIFO); 
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");

			// receber a resposta
			resposta = recivefrom(nomeFifoFront);
			if(resposta.value == -5){
				printf("\n\033[31mERRO na leitura\n\033[0m");
				return 1;
			}

			if (resposta.value == 0)
				printf("\n\033[32mComprou o item %d\033[0m\n", resposta.id_offer);
			else if (resposta.value == 1)
				printf("\n\033[32mLicitou o item %d com %d€\033[0m\n", resposta.id_offer, resposta.offer);
			else if(resposta.value == -2)
				printf("\n\033[31mNão tem fundos suficientes para a licitação\033[0m\n");
			else
				printf("\n\033[31mLicitação inválida\033[0m\n");
			
		} else if(value == -7){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: buy <id> <valor>\033[0m\n");

		} else if(value == 8){
			// envia mensagem
			s = sendto(mensagem, BACK_FIFO); 
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");

			// receber a resposta
			resposta = recivefrom(nomeFifoFront);
			if(resposta.value == -5){
				printf("\n\033[31mERRO na leitura\n\033[0m");
				return 1;
			}

			printf("\n\033[32mSaldo: %d€\033[0m\n", resposta.cash);

		} else if(value == -8){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: cash\033[0m\n");

		} else if(value == 9){
			sscanf(arguments, "%d", &mensagem.valor_add);

			// envia mensagem
			s = sendto(mensagem, BACK_FIFO);
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");

			// receber a resposta
			resposta = recivefrom(nomeFifoFront);
			if(resposta.value == -5){
				printf("\n\033[31mERRO na leitura\n\033[0m");
				return 1;
			}

			if (resposta.value == 0)
				printf("\n\033[32mSaldo adicionado com sucesso\033[0m\n");
			else
				printf("\n\033[31mErro a adicionar o saldo\033[0m\n");
			
		} else if(value == -9){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: add <valor>\033[0m\n");

		} else if(value == 10){
			s = sendto(mensagem, BACK_FIFO); // envia mensagem
			if(s < 0)
				printf("\n\033[31mERRO no envio\n\033[0m");

			// receber a resposta
			resposta = recivefrom(nomeFifoFront);
			if(resposta.value == -5){
				printf("\n\033[31mERRO na leitura\n\033[0m");
				return 1;
			}
			printf("\033[35m\n> Tempo do sistema: %ds\033[0m\n", resposta.value);

		} else if(value == -10){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: time\033[0m\n");

		} else if(value == 11){
			printHelp_Front();

		} else if(value == -11){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: help\033[0m\n");

		} else if(value == 12){
			union sigval val;
			val.sival_int = 0;
			sigqueue(PID, SIGINT, val);

		} else if(value == -12){ // erro de formatacao
			printf("\n\033[31mERRO na formatação: exit\033[0m\n");

		} else 
			printf("\n\033[31mERRO - Comando não existe\033[0m\n");

		printf("\n>> ");

	} while (1);
}