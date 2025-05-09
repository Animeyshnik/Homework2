#include <exception>
#include <iostream>

#include "server.h"


int main(int argc, char** argv)
{
    try
    {
        rau::Server server;
        server.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}                                                                                                                   

