#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024] = "Hello from client!";

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(clientSocket);
        return 1;
    }

    send(clientSocket, buffer, strlen(buffer), 0);

    int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize < 0) {
        std::cerr << "Receive failed" << std::endl;
    } else {
        buffer[recvSize] = '\0';
        std::cout << "Message from server: " << buffer << std::endl;
    }

    close(clientSocket);
    return 0;
}

