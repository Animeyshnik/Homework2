#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

#define PORT 8080

std::vector<int> clients;
std::mutex clients_mutex;

void handle_client(int client_fd) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            std::cout << "Client disconnected.\n";
            close(client_fd);

            clients_mutex.lock();
            clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
            clients_mutex.unlock();

            break;
        }

        std::cout << "Received message from client: " << buffer << std::endl;

        clients_mutex.lock();
        for (int c : clients) {
            if (c != client_fd) { 
    		    if (send(c, buffer, bytes_received, 0) == -1) {
                    std::cerr << "Failed to send message to client " << c << std::endl;
                }
            }
        }
        clients_mutex.unlock();
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return -1;
    }

    std::cout << "Server is waiting for connections...\n";

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        clients_mutex.lock();
        clients.push_back(client_fd);
        clients_mutex.unlock();

        std::cout << "New client connected!\n";

        std::thread(handle_client, client_fd).detach();  
    }

    close(server_fd);
    return 0;
}

