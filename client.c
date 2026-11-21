#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void){
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return 1;
    }
    // where the server is
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);
    // 3. connect (dial the server)
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0){
        perror("connect failed");
        close(sock);
        return 1;
    }
    // 4. send a message
    char *msg = "i love you kue";
    send(sock, msg, strlen(msg), 0);

    char buffer[1024] = {0};
    ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0){
        printf("server says: %s\n", buffer);
    }
    close(sock);
    return 0;
}