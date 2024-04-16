
#include<iostream>
#include "Socket.h"
#include "SSLsocket.h"
#include <string>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include<list>
#include <algorithm>
#include <thread>
#include <deque>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::mutex console_mutex;
std::deque<std::string> data_buffer;

void tcpConnectionsHandler_old(const unsigned int portNumber)
{
    try
    {
        SSLsocket *ssl_socket = SSLsocket::GetInstance();
        console_mutex.lock();

        std::cout << "Configuring Local address  ..." << std::endl;
        Socket tcpServer(portNumber, AF_INET, SOCK_STREAM, AI_PASSIVE);
        std::cout << "Creating socket..." << std::endl;

        if (tcpServer.create() < 0)
        {
            std::cout << "Socket creation failled with error : " << tcpServer.getErroNo() << std::endl;
            return;
        }

        if (tcpServer.bind() != 0)
        {
            std::cout << "Could not bind to local port  errno : " << tcpServer.getErroNo() << std::endl;
            return;
        }

        std::cout << "Listening ..." << std::endl;

        if (tcpServer.listen(50 < 0))
        {
            std::cout << "Failled to listen to the socket " << std::endl;
            return;
        }

        fd_set sockets_master;
        FD_ZERO(&sockets_master);
        FD_SET(tcpServer.getSocket(), &sockets_master);
        std::vector<int> client_sockets;
        client_sockets.push_back(tcpServer.getSocket());

        console_mutex.unlock();

        while (true)
        {
            fd_set socket_reads;
            socket_reads = sockets_master;

            if (select(*std::max_element(client_sockets.begin(), client_sockets.end()) + 1, &socket_reads, 0, 0, 0) < 0)
            {
                std::cout << "select() failled " << std::endl;
                return;
            }

            for (auto socket : client_sockets)
            {
                if (FD_ISSET(socket, &socket_reads))
                {
                    if (socket == tcpServer.getSocket())
                    {
                        int socket_client = tcpServer.accept();
                        if (socket_client < 0)
                        {
                            std::cout << "accep() failled " << std::endl;
                            return;
                        }
                        FD_SET(socket_client, &sockets_master);
                        client_sockets.push_back(socket_client);
                        console_mutex.lock();
                        std::cout << "New connection from : " << tcpServer.getLastRemoteClientAddress() << std::endl;
                        console_mutex.unlock();
                    }
                    else
                    {
                         std::string dataReceived;
                         std::cout << "Lablel  1 " << std::endl;
                        if( ssl_socket->accept(socket) )
                        {
                            std::cout << "Lablel  2" << std::endl;
                        //std::string dataReceived = tcpServer.receive(socket, 1024);
                         constexpr unsigned int BUFFER_SIZE = 2048;
                         dataReceived = ssl_socket->receive(BUFFER_SIZE);
                        }

                        if (dataReceived.empty() == true)
                        {
                            FD_CLR(socket, &sockets_master);
                            tcpServer.close(socket);
                            auto socket_to_remove = std::find(client_sockets.begin(), client_sockets.end(), socket);
                            if (socket_to_remove != client_sockets.end())
                            {
                                client_sockets.erase(socket_to_remove);
                            }
                            continue;
                        }
                        else
                        {
                            // std::cout << "Data received : " << dataReceived << std::endl;
                            data_buffer.push_front(dataReceived);
                        }


                    }
                }
            }
        }
        tcpServer.close();
        ssl_socket->shutdownCurrentSSLsocket();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}


void tcpConnectionsHandler(const unsigned int portNumber)
{
        SSLsocket *ssl_socket = SSLsocket::GetInstance();
        console_mutex.lock();

        std::cout << "Configuring Local address  ..." << std::endl;
        Socket tcpServer(portNumber, AF_INET, SOCK_STREAM, AI_PASSIVE);
        std::cout << "Creating socket..." << std::endl;

        if (tcpServer.create() < 0)
        {
            std::cerr << "Socket creation failled with error : " << tcpServer.getErroNo() << std::endl;
            return;
        }

        if (tcpServer.bind() != 0)
        {
            std::cerr << "Could not bind to local port  errno : " << tcpServer.getErroNo() << std::endl;
            return;
        }

        std::cout << "Listening ..." << std::endl;

        if (tcpServer.listen(50) < 0)
        {
            std::cerr << "Failled to listen to the socket " << std::endl;
            return;
        }

        console_mutex.unlock();

    while (true)
    {
            int client_socket = tcpServer.accept();

              if( ssl_socket->accept(client_socket ) )
            {
                 constexpr unsigned int BUFFER_SIZE = 2048;
                 std::string dataReceived = ssl_socket->receive(BUFFER_SIZE);
                 if (dataReceived.empty() == false )
                 {
                        data_buffer.push_front(dataReceived);
                        tcpServer.close(client_socket);
                        ssl_socket->shutdownCurrentSSLsocket();
                        
                 }
                 else
                 {
                    //close local socket
                    //tcpServer.close(client_socket );
                 }
             }
        }
        ssl_socket->shutdownCurrentSSLsocket();
}

int  main(int argc, char* argv[])
{
    if(argc < 4)
    {
        std::cout << "Invalid input parameters : " << std::endl << "Usage : " <<"./device-monitor.exe PortNumber certificateFile privateKeyFile" << std::endl;
        return 1;
    }
    const unsigned int port_number = std::atoi(argv[1]);
    const std::string cert_file(argv[2]);
    const std::string key_file(argv[3]);

    SSLsocket *ssl_socket = SSLsocket::GetInstance(TLS_server_method(), cert_file, key_file);
    std::thread communicationThread(tcpConnectionsHandler, port_number);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if(data_buffer.size() > 0 )
        {
            console_mutex.lock();
            std::cout << "Data Received : " << data_buffer.back() << std::endl;
            console_mutex.unlock();
            data_buffer.pop_back();
        }
    }

    communicationThread.join();
    ssl_socket->Destroy();
    
    return 0;
}