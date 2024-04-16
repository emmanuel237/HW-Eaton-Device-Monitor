#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string>
#include <cstring>

class Socket
{

public:
    Socket(const unsigned int portNumber, const unsigned int ipAddrType, const unsigned int sockType,
     const unsigned int flags, const std::string&  remoteHostAddress = std::string() );

     int create();
     int bind();
     int listen(unsigned const int maxNberConnections);
     int accept();
     int connect();
     std::string receive(const int socket, const unsigned int bufferSize);
     int send(const int socket, const std::string& dataToSend);
     void close(const int socket=-1);
     std::string getLastRemoteClientAddress() const
     {
        return m_last_connected_client_address;
     }

     int getErroNo() const 
     {
            return errno;
     }

     int getSocket() const
     {
        return m_socket;
     }

    ~Socket();

    Socket() = delete;
    Socket(const Socket& other ) = delete;
    Socket& operator = (const Socket& other ) = delete;
protected:

private:

std::string m_last_connected_client_address;
bool m_is_closed;
int m_socket; 
struct addrinfo *m_address;

};


#endif


