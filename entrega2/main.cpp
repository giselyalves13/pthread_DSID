#include "unistd.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <iostream>       // std::cin, std::cout
#include <queue>          // std::queue

using namespace std;

#define PORT 8080
#define NUMTHREADS 3

pthread_mutex_t mutex_visits;
pthread_mutex_t mutex_cond;
pthread_cond_t  block_thread;
int visitantes = 0;
queue<int> fila_socket;

void *process_request(void * n) {
    pthread_cond_wait(&block_thread, &mutex_cond);
    int sock;
    sock = fila_socket.front();
    // puts("blabla");
    printf("%d", sock);
    fila_socket.pop();
    printf("%d", fila_socket);
    char buffer[1024] = {0};
    char response[1024]; //Tem que aumentar o tamanho do char
    char exit_message[1024] = "sair\r\n";

    pthread_mutex_lock(&mutex_visits);
    visitantes++;
    sprintf(response,"Você é o #%dº visitante!!! Muito obrigada :) Para sair basta enviar \"sair\".\n ", visitantes);
    pthread_mutex_unlock(&mutex_visits);

    // sock = (int*)sock;
    while(1) {
        read(sock, buffer, 1024);
        printf("%s\n", buffer);
       
        if(!strcmp(&buffer[1024], &exit_message[1024])) {
            close(sock);
            pthread_exit(NULL);
        }
        send(sock, response, strlen(response), 0);
        printf("Ḿensagem de oi enviada\n");
    }
}

int main() {
    for(int i = 0; i < NUMTHREADS; i++){
        pthread_t thread;
        pthread_create(&thread, NULL, process_request, (void *)NULL);
    }

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    pthread_mutex_init(&mutex_visits, NULL);
    pthread_mutex_init(&mutex_cond, NULL);
    pthread_cond_init (&block_thread, NULL);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        fila_socket.push(new_socket);
        pthread_cond_signal(&block_thread);
        // pthread_t thread;
        // pthread_create(&thread, NULL, process_request, (void *)new_socket);
    }
}

// client:
// telnet localhost 8080
// envia qualquer mensagem
