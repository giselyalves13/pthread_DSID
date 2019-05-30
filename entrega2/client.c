#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
int main(int argc , char *argv[])
{
	//Criar um for em volta da conexão para criar varios clientes
	//receber a string do server e guardar num arquivo
	int sock;
	struct sockaddr_in server;
	char message[1024] , server_reply[1024];
	//Tem que aumentar o tamanho do char de reply
	// Criação do socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Não foi possível criar o socket.");
	}
	puts("Socket criado");

	// server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons( 8080 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
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
		if(!strcmp(&message[1024], "sair")){
			close(sock);
		}

		//Send some data
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Falha no envio da mensagem.");
			return 1;
		}

		//Receive a reply from the server
		if( recv(sock , server_reply , 1024 , 0) < 0)
		{
			puts("Falha ao receber resposta do servidor.");
			// break;
		}

		puts("Resposta do servidor: ");
		puts(server_reply);
	// }

	// puts("Para sair envie 'sair'");
	// scanf("%s" , message);
	// send(sock , message , strlen(message), 0);

	// close(sock);
	return 0;
}

// O usuário não vai enviar mensagem, vai enviar só uma string com cabeçalho http get e receber a resposta do servidor, outro cabeçalho http, que pode ser salvo em arquivo html
