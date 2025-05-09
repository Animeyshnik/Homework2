#include "server.h"

// C++ STD headers
#include <stdexcept>
#include <iostream>

// POSIX headers
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX  80
#define PORT 8080
#define SA struct sockaddr

rau::Server::Server()
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0)
    {
        throw std::runtime_error("Unable to create the socket.");
    }
        seqId_ = 0UL;
}

void* processClient(void* data);

void rau::Server::start()
{
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        // Binding newly created socket to given IP and verification
        if ((bind(fd_, (SA*)&servaddr, sizeof(servaddr))) != 0)
    {
                throw std::runtime_error("Unable to bind the server socket.");
        }

        // Now server is ready to listen and verification
        if (listen(fd_, 5) != 0) 
    {
        throw std::runtime_error("Unable to start listening...");
        }

    while (true)
    {
        // Consider using std::strerror
        struct sockaddr_in cli;
        socklen_t len = sizeof(cli);
        int connfd = accept(fd_, (SA*)&cli, &len);
        if (connfd < 0)
        {
            std::cerr << "Unable to accept the client." << std::endl;
            continue;
        }

                // std::unordered_map::emplace { iterator, bool }
                // std::unordered_map::iterator { key, value }
                ClientDescriptor& descriptor = clients_.emplace(seqId_, ClientDescriptor()).first->second;
                descriptor.fd = connfd;
                descriptor.id = seqId_++;

        pthread_create(&descriptor.thread, NULL, processClient, &descriptor);
    }
}

void sendMessage(int fd, const char* data, std::size_t size)
{
        // std::unique_ptr can be used instead buff
        char* buff = new char[size + 4];
        char* sizePtr = reinterpret_cast<char*>(size);
        buff[0] = sizePtr[0];
        buff[1] = sizePtr[1];
        buff[2] = sizePtr[2];
        buff[3] = sizePtr[3];

        for (std::size_t i = 0; i < size; i++)
        {
                buff[i + 4] = data[i];
        }

        write(fd, buff, size + 4);

        delete buff;
}

// endianess lsb/msb
void sendId(const ClientDescriptor& desc)
{
        sendMessage(desc.fd, reinterpret_cast<char*>(&desc.id), sizeof(desc.id));
}

// Function designed for chat between client and server.
void* processClient(void* data)
{
        rau::ClientDescriptor* desc = reinterpret_cast<rau::ClientDescriptor*>(data);
    int connfd = desc->fd;

        char buff[MAX];
        int n;
        // infinite loop for chat
        for (;;) {
                bzero(buff, MAX);

                // read the message from client and copy it in buffer
                read(connfd, buff, sizeof(buff));
                // print buffer which contains the client contents
                printf("From client: %s\t To client : ", buff);
                bzero(buff, MAX);
                n = 0;
                // copy server message in the buffer
                while ((buff[n++] = getchar()) != '\n');

                // and send that buffer to client
                write(connfd, buff, sizeof(buff));

                // if msg contains "Exit" then server exit and chat ended.
                if (strncmp("exit", buff, 4) == 0) {
                        printf("Server Exit...\n");
                        break;
                }
        }

    return nullptr;
}                                                                                                                    

