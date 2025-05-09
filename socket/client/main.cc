#include <exception>

#include "client.h"

int main(int argc, char** argv)
{
    try
    {
        rau::Client client;

        client.connectServer("127.0.0.1");
        client.start();
    }
    catch (const std::exception& e)
    {

    }
}                                                                                                                    

