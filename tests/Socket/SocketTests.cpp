#include "CppUTest/TestHarness.h"
#include "Socket.h"
#include <string>
#include <iostream>

TEST_GROUP(Socket)
{
    void setup()
    {
    }
    void teardown()
    {
    }
};

TEST(Socket, CreateTCPServerSocket)
{
    Socket tcpSocket(3393, AF_INET, SOCK_STREAM,AI_PASSIVE);

    CHECK( tcpSocket.create() >= 0);
}

TEST(Socket, BindTCPServerSocket)
{
    Socket tcpSocket(3393, AF_INET, SOCK_STREAM,AI_PASSIVE);
    tcpSocket.create();

    CHECK( tcpSocket.bind() == 0);
}

TEST(Socket, ListenTCPServerSocket)
{
     Socket tcpSocket(3393, AF_INET, SOCK_STREAM,AI_PASSIVE);
     tcpSocket.create() ;
     tcpSocket.bind() ;
     CHECK(tcpSocket.listen(50) >= 0);
}

TEST(Socket, ConstructorRaisesAnExceptionOnWrongParamaters)
{
    bool exception_raised(false);
    try
    {
        Socket tcpClient(4566, 0, SOCK_STREAM,0,"fake host");
    }
    catch(const std::invalid_argument& e)
    {
        exception_raised = true;
    }
    
    CHECK(  exception_raised == true );
}

TEST(Socket, NoExceptionRaisedWithValidParameters)
{
    bool exception_raised(false);
    try
    {
        Socket tcpClient(4566, 0, SOCK_STREAM,0,"localhost");
    }
    catch(const std::invalid_argument& e)
    {
        exception_raised = true;
    }
    
    CHECK(  exception_raised == false );
}

TEST(Socket, CreateTCPClient)
{
    Socket tcpClient(3393, 0, SOCK_STREAM,0,"localhost");

    CHECK( tcpClient.create() >= 0);
}

TEST(Socket, tcpClientConnect) //This test needs an internet connexion to run
{
    Socket httpClient(80, 0, SOCK_STREAM, 0, "example.com" );
    httpClient.create();

    CHECK( httpClient.connect() == 0 );
}

TEST(Socket, tcpClientSend)
{
    Socket httpClient(80, 0, SOCK_STREAM, 0, "example.com" );
    httpClient.create();
    httpClient.connect();

    std::string httpRequest = 
    "GET / HTTP/1.1\r\n"
    "Host: example.com\r\n\r\n";

    int bytes_sent = httpClient.send(httpClient.getSocket() , httpRequest.c_str());
    
    CHECK(bytes_sent == static_cast<int>(httpRequest.size()));
}

TEST(Socket, tcpClientReceive)//This test needs an internet connexion to run
{
    Socket httpClient(80, 0, SOCK_STREAM, 0, "example.com" );
    httpClient.create();
    httpClient.connect();

    const std::string httpRequest = 
    "GET / HTTP/1.1\r\n"
    "Host: example.com\r\n\r\n";

     httpClient.send(httpClient.getSocket() , httpRequest.c_str());
     std::string data_rcvd = httpClient.receive(httpClient.getSocket(), 4096);
     CHECK( data_rcvd.size() > 0 );    
}