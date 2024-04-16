#include "CppUTest/TestHarness.h"
#include "SSLsocket.h"
#include "Socket.h"
#include <string>
#include <iostream>
/* Running some of these unit tests requires an internet connection*/

TEST_GROUP(SSLsocket)
{
    void setup()
    {
        SSLsocket::GetInstance();
    }
    void teardown()
    {
        SSLsocket::GetInstance()->Destroy();
    }
};


TEST(SSLsocket, SSLsocketInstanceIsUnique)
{

        SSLsocket *ssl_socket = SSLsocket::GetInstance();
        SSLsocket *ssl_socket2 = SSLsocket::GetInstance();

        CHECK(ssl_socket == ssl_socket2  );
}

TEST(SSLsocket, SSLconnect)
{
    Socket httpsClient(443, 0, SOCK_STREAM, 0, "example.org" );
    httpsClient.create();
    httpsClient.connect();
    
    CHECK( SSLsocket::GetInstance()->connect(httpsClient.getSocket()) == true  );

}

TEST(SSLsocket, SSLclientReceivesCertificate)
{
    Socket httpsClient(443, 0, SOCK_STREAM, 0, "example.org" );
    httpsClient.create();
    httpsClient.connect();
     SSLsocket::GetInstance()->connect(httpsClient.getSocket());

     CHECK( SSLsocket::GetInstance()->hasReceivedCertificate() == true );
}


TEST(SSLsocket, SSLclientSend)
{
     Socket httpsClient(443, 0, SOCK_STREAM, 0, "example.org" );
    httpsClient.create();
    httpsClient.connect();
     SSLsocket::GetInstance()->connect(httpsClient.getSocket());   

     const std::string dataToSend = 
    "Host: example.org:443\r\n"
    "Connection: close\r\n"
    "User-Agent: https_simple\r\n"
    "\r\n";
    CHECK(SSLsocket::GetInstance()->send(dataToSend) == static_cast<int>( dataToSend.size()) );
}

TEST(SSLsocket, SSLclientReceive)
{
    Socket httpsClient(443, 0, SOCK_STREAM, 0, "example.org" );
    httpsClient.create();
    httpsClient.connect();
     SSLsocket::GetInstance()->connect(httpsClient.getSocket());   

     const std::string dataToSend = 
    "Host: example.org:443\r\n"
    "Connection: close\r\n"
    "User-Agent: https_simple\r\n"
    "\r\n";
    SSLsocket::GetInstance()->send(dataToSend) ;

    constexpr unsigned int BUFFER_SIZE = 4084;
    std::string server_reponse = SSLsocket::GetInstance()->receive(BUFFER_SIZE);

    CHECK( server_reponse.size() > 0 );
}