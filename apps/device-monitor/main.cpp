
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
#include <map>

using json = nlohmann::json;

std::mutex console_mutex;
std::deque<std::string> data_buffer;


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

        if (tcpServer.listen(100) < 0)
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
                 constexpr unsigned int BUFFER_SIZE = 1024;
                 std::string data_received;
                data_received = ssl_socket->receive(BUFFER_SIZE);
                 if(data_received.empty() == false && data_received.at(data_received.size() - 1)  == '\n')
                 {
                        data_received = data_received.substr(0, data_received.size() - 1);//removing the leading "\n"
                        data_buffer.push_front(data_received);
                 }
                ssl_socket->send("OK");
                 tcpServer.close(client_socket);
                 ssl_socket->shutdownCurrentSSLsocket();
             }
     }
        tcpServer.close();
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

    std::map<std::string, unsigned long> message_count;
     
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if(data_buffer.size() > 0 )
        {
            //std::cout << "Data Received : " << data_buffer.back() << std::endl;
            json parsed_data= json::parse( data_buffer.back() );
             data_buffer.pop_back();
            std::string device_name = parsed_data["DeviceName"];
            //int measurement = parsed_data["Measurement"];

            message_count[device_name] = message_count[device_name] + 1;
             console_mutex.lock();
             std::cout << "=========== Message Count ================" << std::endl;
            int total = 0;
            for( const auto& msg_count : message_count )
            {
                    std::cout << msg_count.first  << " : " << msg_count.second << std::endl;;
                    total = total + msg_count.second;  
            }
            std::cout << "Total message count : " << total << std::endl;
            console_mutex.unlock();
        }
    }

    communicationThread.join();
    ssl_socket->Destroy();
    
    return 0;
}