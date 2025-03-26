#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define PORT 8080

int client_fd;

void receive_messages() {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            std::cout << "Message from another client: " << buffer << std::endl;
        } else if (bytes_received == 0) {
            std::cout << "Server closed the connection.\n";
            break;
        } else {
            std::cerr << "Failed to receive message\n";
            break;
        }
    }
}

int main() {
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(PORT);

    if (connect(client_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection to server failed\n";
        close(client_fd);
        return -1;
    }

    std::cout << "Connected to server. You can start typing your messages.\n";

    std::thread(receive_messages).detach();

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        std::cout << "Enter message: ";
        std::cin.getline(buffer, sizeof(buffer));

        if (strcmp(buffer, "exit") == 0) {
            std::cout << "Exiting...\n";
            break;
        }

        if (send(client_fd, buffer, strlen(buffer), 0) == -1) {
            std::cerr << "Failed to send message to server\n";
        }
    }

    close(client_fd);
    return 0;
}

