#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define PORT 8080
#define POOL 2
pthread_mutex_t mutex_visits;
int visitantes = 0;
int count_t = 0;

void *process_request(void * sock) {
    char buffer[1024] = {0};
    char response[1024];
    char exit_message[1024] = "sair\r\n";

    pthread_mutex_lock(&mutex_visits);
    visitantes++;
    sprintf(&response, "Você é o #%dº visitante!!! Muito obrigada :) Para sair basta enviar \"sair\".\n ", visitantes);
    pthread_mutex_unlock(&mutex_visits);

    sock = (int*)sock;
    while(1) {
        read(sock, buffer, 1024);
        printf("%s\n", buffer);
        if(!strcmp(&buffer, &exit_message)) {
            //não to conseguindo bater buffer que to recebendo com a mensagem de exit
            close(sock);
            count_t--;
            printf("count %d", count_t);
            pthread_exit(NULL);
        }
        send(sock, response, strlen(response), 0);
        printf("Hello message sent\n");
    }

}
// Ao invés de enviar a mensagem de visitantes, vai enviar uma string com um cabeçalho http response com essa mesma mensagem dentro de uma estrutura simples de html

int main() {
    printf("oi");
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    pthread_mutex_init(&mutex_visits, NULL);
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
        printf("Conexão");
        if (count_t < POOL){
            pthread_t thread;
            count_t++;
            printf("count %d", count_t);
            pthread_create(&thread, NULL, process_request, (void *)new_socket);
        }else{
            printf("Sem espaço irmão");
        }
        
    }
}

// o pool ainda não faz a tribuição certa. Quando uma requisição chega e não tem mais threads livres, a requisição precisa esperar uma das threads acabar

