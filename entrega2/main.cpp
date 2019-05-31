#include "unistd.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <ctime>
#include <iostream> // std::cin, std::cout
#include <queue>    // std::queue

using namespace std;

// Porta de conexão de socket será a 8080
#define PORT 8080

// Número de threads a serem criadas
#define NUMTHREADS 3

// dados da thread
typedef struct
{
    pthread_t pthread;
    pthread_mutex_t mutex_visits;
    pthread_mutex_t mutex_cond;
    pthread_cond_t block_thread;
    pthread_cond_t return_thread;
} THREAD_DATA;

// *********** VARIÁVEIS GLOBAIS ********* //
// do número de clientes conectados ao servidor.
int visitantes = 0;

// fila de sockets
queue<int> fila_socket;

// contador
int count = 0;

// timestamp de agora
std::string timestamp_now = std::to_string((int)time(NULL));
// nome do arquivo de log: timestamp.txt
std::string url = timestamp_now + ".txt";
char filename[20];

void log_to_file(char *file_name, char *info)
{
    time_t rawtime;
    struct tm *timeinfo;
    char timenow[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timenow, sizeof(timenow), "%Y-%m-%d %H:%M:%S", timeinfo);

    FILE *arq;
    arq = fopen(file_name, "a");
    if (arq == NULL)
    {
        printf("Erro. Não foi possível criar o arquivo de log.");
    }
    else
    {
        fprintf(arq, "%s - ", timenow);
        fprintf(arq, "%s\r\n", info);
        // puts("\r\n", arq);
    }
    fclose(arq);
    // open file
    // save text to file
    // close file
}

// ********* Método que trata as requisições ao servidor ********* //
void *process_request(void *t_data)
{
    THREAD_DATA *td = (THREAD_DATA *)t_data;
    while (!pthread_cond_wait(&td->block_thread, &td->mutex_cond))
    {

        int sock;
        sock = fila_socket.front();
        // printf("%d", sock);
        log_to_file(filename, "[OK] Socket removido da fila.");
        fila_socket.pop();
        char buffer[1024] = {0};
        char response[1024];
        char exit_message[1024] = "sair";

        pthread_mutex_lock(&td->mutex_visits);
        visitantes++;
        sprintf(response, "Você é o #%dº visitante!!! Muito obrigada :) Para sair basta enviar \"sair\".\n ", visitantes);
        pthread_mutex_unlock(&td->mutex_visits);
        log_to_file(filename, "[OK] Thread desbloqueada.");

        read(sock, buffer, 1024);
        printf("%s\n", buffer);
        char get[1024] = "GET /";
        if (!strcmp(buffer, get))
        {
            send(sock, response, strlen(response), 0);
            log_to_file(filename, "[ERRO] Mensagem de boas-vindas enviada.");
            printf("Ḿensagem de oi enviada\n");
            printf("Cliente saiu");

            close(sock);
            count--;
        }
        else
        {
            printf("404");
            close(sock);
            printf("Cliente saiu");
            count--;
        }
        bzero(buffer, 1024);
    }
}

int main()
{
    /********** Pega o tempo atual e cria um arquivo de log com o nome no formato yyyymmaahhmmss **********/
    time_t rawtime;
    struct tm *timeinfo;
    char timenow[80];
    // char filename[20];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(timenow, sizeof(timenow), "%Y%m%d%H%M%S", timeinfo);

    strcpy(filename, timenow);
    strcat(filename, ".txt");

    log_to_file(filename, "[OK] Servidor iniciado.");
    printf("Arquivo de log criado com o nome: %s\n", filename);

    FILE *arq;
    // // abrindo arquivo no modo de leitura
    // arq = fopen(url, "w");
    // if (arq == NULL) {
    //     printf("Erro, nâo foi possível criar o arquivo de log.");
    // }
    // else {
    //     fputc("Inicialização do servidor", arq);
    //     fclose(arq);
    // }

    // Print na tela
    // std::cout << timestamp_now << '\n' ;

    // printf("Timestamp: %d\n",(int)time(NULL));

    // Criação de vetor de threads, com tamanho NUMTHREADS definido pelo  #define acima
    THREAD_DATA thread[NUMTHREADS];

    // Inicialização do vetor de threads com valores nulos
    for (int i = 0; i < NUMTHREADS; i++)
    {
        pthread_mutex_init(&thread[i].mutex_visits, NULL);
        pthread_mutex_init(&thread[i].mutex_cond, NULL);
        pthread_cond_init(&thread[i].block_thread, NULL);
        pthread_cond_init(&thread[i].return_thread, NULL);
        pthread_create(&thread[i].pthread, NULL, process_request, (void *)&thread[i]);
    }

    log_to_file(filename, "[OK] threads inicializadas.");

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Criação de arquivo de descrição do socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        log_to_file(filename, "[ERRO] Falha na criação do socket.");
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Atando o socket à porta 8080 à força.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        log_to_file(filename, "[ERRO] Falha no bind.");
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        log_to_file(filename, "[ERRO]. Falha ao escutar por conexões.");
        perror("listen");
        exit(EXIT_FAILURE);
    }

    log_to_file(filename, "[OK] Socket criado. Aguardando conexões...");

    // Rodar para sempre para que o servidor fique escutando à espera de novas conexões.
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            log_to_file(filename, "[ERRO] Falha ao aceitar conexão.");
            perror("accept");
            exit(EXIT_FAILURE);
        }
        fila_socket.push(new_socket);
        if (count < NUMTHREADS)
        {
            pthread_mutex_lock(&thread[count].mutex_cond);
            printf("%d", new_socket);
            log_to_file(filename, "[OK] Socket adicionado à fila.");
            fila_socket.push(new_socket);
            pthread_cond_signal(&thread[count].block_thread);
            pthread_mutex_unlock(&thread[count].mutex_cond);
            count++;
        }
    }
}

// client:
// telnet localhost 8080
// envia qualquer mensagem
