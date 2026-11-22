#include <stdio.h>
#include <string.h>
#include <unistd.h>      // close()
#include <arpa/inet.h>   // htons(), htonl()
#include <sys/socket.h>  // socket(), bind(), listen(), accept(), send(), recv()
#include <netinet/in.h>  // struct sockaddr_in, INADDR_ANY

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        return 1;
    }

    // Restart the server quickly without "Address already in use"
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        return 1;
    }
    printf("waiting for client...\n");

    int client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) {
        perror("accept failed");
        close(server_fd);
        return 1;
    }
    printf("client connected\n");

    char buffer[1024];
    char reply[1024];

    while (1) {
        // ---- OUTPUT (receive from client) ----
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            printf("client disconnected\n");
            break;
        }
        printf("client: %s\n", buffer);

        // ---- INPUT (type a reply) ----
        // keep asking until the reply is not empty
        do {
            printf("you: ");
            fflush(stdout);
            if (fgets(reply, sizeof(reply), stdin) == NULL) {
                reply[0] = '\0';
                break;
            }
            reply[strcspn(reply, "\n")] = 0;   // strip newline
        } while (strlen(reply) == 0);

        if (strlen(reply) == 0) break;          // stdin closed (Ctrl+D)

        send(client_fd, reply, strlen(reply), 0);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}