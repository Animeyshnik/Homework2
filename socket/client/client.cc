#include "client.h"

// C++ STD headers
#include <stdexcept>

// POSIX headers
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX  80
#define PORT 8080
#define SA struct sockaddr

rau::Client::Client() {
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ == -1) {
                throw std::runtime_error("unable to create the client socket..");
        } 
}

void rau::Client::connectServer(const std::string& address) {
        if (address.empty())
        {
                throw std::logic_error("Address is empty!");
        }

        struct sockaddr_in servaddr;
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        // std::string::c_str() -> underlying C-style string
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(address.c_str());
        servaddr.sin_port = htons(PORT);
    if (connect(fd_, (SA*)&servaddr, sizeof(servaddr)) != 0) {
                throw std::runtime_error("connection with the server failed...");
        }
}

void rau::Client::start(){
    char buff[MAX];
        int n;
        for (;;) {
                bzero(buff, sizeof(buff));
                printf("Enter the string : ");
                n = 0;
                while ((buff[n++] = getchar()) != '\n');
                write(fd_, buff, sizeof(buff));
                bzero(buff, sizeof(buff));
                read(fd_, buff, sizeof(buff));
                printf("From Server : %s", buff);
                if ((strncmp(buff, "exit", 4)) == 0) {
                        printf("Client Exit...\n");
                        break;
                }
        }
}                                                                                                                    

