
#include <iostream>
#include "Socket.h"
#include "SSLsocket.h"
#include <string>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <random>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    const std::string cert_file(argv[4]);

    std::random_device rd;
    std::mt19937 gen(rd());

    int min = 1;
    int max = 10000;
    std::uniform_int_distribution<int> dist(min, max);
    SSLsocket *ssl_socket;
    int message_count = 0;
    try
    {

        ssl_socket = SSLsocket::GetInstance(TLS_client_method(), cert_file);
        constexpr unsigned int BUFFER_SIZE = 1024;

        while (true)
        {
            Socket tcpClient(port_number, 0, SOCK_STREAM, 0, server_address);

            if (tcpClient.create() < 0)
            {
                std::cerr << "Socket creation failled with error : " << tcpClient.getErroNo() << std::endl;
                return 1;
            }

            if (tcpClient.connect() != 0)
            {
                std::cerr << "Could not connect to the remote server " << std::endl;
                return 1;
            }

            ssl_socket->connect(tcpClient.getSocket());
            if ( ssl_socket->cmpRemoteHostCert() == true)
            {
                json data_to_send_json;
                data_to_send_json["DeviceName"] = device_name;
                data_to_send_json["Measurement"] = dist(gen);
                std::string data_to_send = data_to_send_json.dump() + "\n";
                ssl_socket->send(data_to_send);
                ssl_socket->receive(BUFFER_SIZE);
                tcpClient.close();
                ssl_socket->shutdownCurrentSSLsocket();
                std::cout << device_name << " message count " << ++message_count << std::endl;
            }
            else
            {
                ssl_socket->send("No data");
                ssl_socket->receive(BUFFER_SIZE);
                tcpClient.close();
                ssl_socket->shutdownCurrentSSLsocket();
                std::cerr << "Remote Server Authentication failled, no data will be sent to remote party" << std::endl;
            }
            sleep(5);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    ssl_socket->Destroy();
    return 0;
}
