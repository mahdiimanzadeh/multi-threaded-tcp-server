#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define THREAD_POOL_SIZE 5

typedef struct {
    int client_socket;
    struct sockaddr_in client_address;
} client_t;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;
client_t *clients[THREAD_POOL_SIZE];
int client_count = 0;

void *handle_client(void *arg) {
    int new_socket;
    char buffer[BUFFER_SIZE];
    char *filename = "server_file.txt";
    int file_fd;

    while (1) {
        pthread_mutex_lock(&mutex);
        
        while (client_count == 0) {
            pthread_cond_wait(&condition_var, &mutex);
        }
        
        new_socket = clients[--client_count]->client_socket;
        free(clients[client_count]);
        
        pthread_mutex_unlock(&mutex);
        
        int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
        buffer[bytes_read] = '\0';
        printf("Message from client: %s\n", buffer);

        if (strncmp(buffer, "WRITE:", 6) == 0) {
            char *message = buffer + 6;
            
            pthread_mutex_lock(&file_mutex);
            
            file_fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644);
            if (file_fd < 0) {
                perror("File open error");
                pthread_mutex_unlock(&file_mutex);
                close(new_socket);
                continue;
            }
            
            write(file_fd, message, strlen(message));
            close(file_fd);
            
            pthread_mutex_unlock(&file_mutex);
            
            write(new_socket, "Write successful\n", 17);
        } else if (strncmp(buffer, "READ", 4) == 0) {
            pthread_mutex_lock(&file_mutex);
            
            file_fd = open(filename, O_RDONLY);
            if (file_fd < 0) {
                perror("File open error");
                pthread_mutex_unlock(&file_mutex);
                close(new_socket);
                continue;
            }
            
            int n;
            while ((n = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
                write(new_socket, buffer, n);
            }
            close(file_fd);
            
            pthread_mutex_unlock(&file_mutex);
        } else {
            write(new_socket, "Invalid command\n", 16);
        }
        
        close(new_socket);
    }

    return NULL;
}

void *thread_function(void *arg) {
    handle_client(NULL);
    return NULL;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    pthread_t thread_pool[THREAD_POOL_SIZE];

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&thread_pool[i], NULL, thread_function, NULL);
    }
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Socket failed");
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("Bind failed");
    }
    
    if (listen(server_fd, 3) < 0) {
        error("Listen failed");
    }
    
    printf("Server listening on port %d\n", PORT);
    
    while ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))) {
        client_t *client = malloc(sizeof(client_t));
        client->client_socket = new_socket;
        client->client_address = address;
        
        pthread_mutex_lock(&mutex);
        clients[client_count++] = client;
        pthread_cond_signal(&condition_var);
        pthread_mutex_unlock(&mutex);
    }
    
    if (new_socket < 0) {
        error("Accept failed");
    }
    
    close(server_fd);
    
    return 0;
}
