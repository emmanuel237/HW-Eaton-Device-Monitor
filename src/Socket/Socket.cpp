#include "Socket.h"
#include<array>
#include <iostream>

Socket::Socket( unsigned int portNumber, const unsigned int ipAddrType, const unsigned int sockType,
     const unsigned int flags, const std::string&  remoteHostAddress):
    m_is_closed(false)
     {
        struct addrinfo hints;
        memset(&hints,0, sizeof(hints));
        hints.ai_family =  ipAddrType;
        hints.ai_socktype = sockType;
        hints.ai_flags = flags;

        int res;
        if(remoteHostAddress.empty() ) 
        {
            res =  getaddrinfo(NULL, std::to_string(portNumber).c_str(), &hints, &m_address);
        }
        else
        {
           res =  getaddrinfo(remoteHostAddress.c_str(), std::to_string(portNumber).c_str(), &hints, &m_address);
        }

        if ( res != 0)
        {
            throw std::invalid_argument("Invalid Network Parameter , errno : " + std::to_string(errno ) );
        }

     }
     
     int Socket::create()
     {
            m_socket = socket(m_address->ai_family, m_address->ai_socktype, 
                                                                                m_address->ai_protocol);
            return m_socket;
     }

     int Socket::bind()
     {
         return ::bind(m_socket, m_address->ai_addr, 
                                m_address->ai_addrlen);
     }

    int Socket::connect()
    {
        return  ::connect(m_socket, m_address->ai_addr, m_address->ai_addrlen);
    }

     int Socket::listen( const unsigned int maxNberConnections )
     {
        return ::listen(m_socket, maxNberConnections);
     }

     int Socket::accept()
     {
        struct sockaddr_storage client_address;
        socklen_t client_len = sizeof(client_address);
        int socket_client = ::accept(m_socket, (struct sockaddr*)&client_address, &client_len);
        if( socket_client >= 0 )
        {
            char address_buffer[256];
            getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0 ,NI_NUMERICHOST);
            m_last_connected_client_address = std::string(address_buffer);
        }
        return socket_client;
     }

    std::string Socket::receive(const int socket, const unsigned int bufferSize)
    {
        std::string data_buffer(bufferSize, ' ');
        std::string ret_data;
        char *char_buffer = const_cast<char*>(data_buffer.data());
        int bytes_received = recv(socket, char_buffer, bufferSize,0);

        if(bytes_received > 0 )
            ret_data =  data_buffer.substr(0, bytes_received);
        else
            ret_data = "";
        return ret_data;
    }

    int Socket::send(const int socket,  const std::string& dataToSend)
    {
        int bytes_sent = ::send(socket, dataToSend.data(), dataToSend.size(), 0 );
        return bytes_sent;
    }

    void Socket::close(const int socket)
    {
        if (socket == -1)
        {
            if (m_is_closed == false)
            {

                ::close(m_socket);
                if (m_address != nullptr)
                {
                    freeaddrinfo(m_address);
                    m_address = nullptr;
                }
                m_is_closed = true;
            }
        }
        else
        {
            ::close(socket);
        }
    }

    Socket::~Socket()
    {
        if(  m_is_closed == false )
        {
            close();
        }
    }