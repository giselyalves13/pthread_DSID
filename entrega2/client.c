#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define NUMTHREADS 10
int main(int argc, char *argv[]) {
	for (int i=0; i < NUMTHREADS; i++) {

		// Criação do arquivo html
  	char str[10];
  	sprintf(str, "%d", i);
  	char *filename[100];
  	strcpy(filename, "client");
  	strcat(filename, str);
  	strcat(filename, ".html");

		FILE *arq;
		arq = fopen(filename, "w");
		if (arq == NULL) {
			printf("Erro: não foi possível criar o arquivo html.");
		} else {
			fprintf(arq, "<html>\n<head><title>Cliente</title></head>\n<body>\n");
		}
		fclose(arq);

		//Criar um for em volta da conexão para criar varios clientes

		int sock;
		struct sockaddr_in server;
		char message[1024], server_reply[1024];

		// Criação do socket
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			printf("Não foi possível criar o socket.");
		}
		puts("Socket criado");


		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_family = AF_INET;
		server.sin_port = htons(8080);

		// Conexão ao servidor remoto
		if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
			perror("Conexão falhou. Erro.");
			return 1;
		}

		puts("Conectado\n");

		char exit_message[1024] = "sair";
		//keep communicating with server
		// while(1)
		// {
		// printf("Digite uma mensagem: ");
		// scanf("%s" , message);
		sprintf(message, "GET /");
		if (!strcmp(&message[1024], "sair"))
		{
			close(sock);
		}

		// Envio de dados para o servidor
		if (send(sock, message, strlen(message), 0) < 0)
		{
			puts("Falha no envio da mensagem.");
			return 1;
		}

		// Recebe uma resposta do servidor
		if (recv(sock, server_reply, 1024, 0) < 0)
		{
			puts("Falha ao receber resposta do servidor.");
		}

		puts("Resposta do servidor: ");
		puts(server_reply);
		arq = fopen(filename, "a");
		if (arq == NULL) {
			printf("Erro: não foi possível abrir o arquivo html.");
		} else {
			fprintf(arq, "<h1>%s</h1>\n</body></html>", server_reply);
		}
		fclose(arq);
		// return 0;
	}
	return 0;
}
