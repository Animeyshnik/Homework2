#ifndef __SERVER_H__
#define __SERVER_H__

// C++ STD headers
#include <unordered_map>

// POSIX headers
#include <pthread.h>

namespace rau
{

struct ClientDescriptor
{
    std::size_t id;
    int         fd;
    pthread_t   thread;
};
// 0 -> { 0, 4 }
// 1 -> { 1, 5 }
// ...
using Clients = std::unordered_map<int, ClientDescriptor>;

class Server
{
public:
    Server();
    Server(const Server& rhs) = delete;
    Server& operator=(const Server& rhs) = delete;
    void start();
    void stop();
private:
    int         fd_;
    std::size_t seqId_;
    Clients     clients_;
};

};

#endif
             
