
#include <iostream>
#include "Socket.h"
#include <string>
#include <time.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cout << "Missing input arguments: " << std::endl
                  << "Usage : "
                  << "./device.exe ServerAddress   ServerPort DeviceName Certificate" << std::endl;
        return 1;
    }

    const std::string server_address(argv[1]);
    const unsigned int port_number = std::atoi(argv[2]);
    const std::string device_name(argv[3]);
    const std::string certificate(argv[4]);

    std::cout << "Configuring Local address  ..." << std::endl;
    try
    {
        Socket tcpClient(port_number, 0, SOCK_STREAM, 0, server_address);

        std::cout << "Creating socket..." << std::endl;

        if (tcpClient.create() < 0)
        {
            std::cout << "Socket creation failled with error : " << tcpClient.getErroNo() << std::endl;
            return 1;
        }

        if (tcpClient.connect() != 0)
        {
            std::cout << "Could not connect to the remote server " << std::endl;
            return 1;
        }

        while (true)
        {
            tcpClient.send(tcpClient.getSocket(), device_name.c_str());
            sleep(2);
        }

        tcpClient.close();
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
