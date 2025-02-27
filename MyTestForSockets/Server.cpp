#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 8080

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[1024];

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Waiting for connections..." << std::endl;

    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientSocket < 0) {
        std::cerr << "Accept failed" << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected!" << std::endl;

    int recvSize = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (recvSize < 0) {
        std::cerr << "Receive failed" << std::endl;
    } else {
        buffer[recvSize] = '\0';
        std::cout << "Message from client: " << buffer << std::endl;
    }

    send(clientSocket, "Hello from server!", 18, 0);

    close(clientSocket);
    close(serverSocket);
    return 0;
}

