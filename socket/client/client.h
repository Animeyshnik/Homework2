#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>

namespace rau{

class Client{
public:
    Client();
    Client(const Client& rhs) = delete;
    Client& operator=(const Client& rhs) = delete;
    void connectServer(const std::string& address);
    void start();
private:
    int fd_;
};

}

#endif
           
