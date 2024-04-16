#include "CppUTest/TestHarness.h"
#include "SSLsocket.h"
#include <string>

const std::string CERT_FILE("certificates/cert.pem");
const std::string KEY_FILE("certificates/key.pem");

TEST_GROUP(SSLsocket)
{
    void setup()
    {
        SSLsocket::GetInstance(TLS_server_method(), CERT_FILE, KEY_FILE);
    }
    void teardown()
    {
        SSLsocket::GetInstance()->Destroy();
    }
};


TEST(SSLsocket, SSLsocketInstanceIsUnique)
{

        SSLsocket *ssl_socket = SSLsocket::GetInstance(TLS_server_method(), CERT_FILE, KEY_FILE);
        SSLsocket *ssl_socket2 = SSLsocket::GetInstance(TLS_server_method(), CERT_FILE, KEY_FILE);

        CHECK(ssl_socket == ssl_socket2  );
}


TEST(SSLsocket, WrongCreationParametersRaisesAnException)
{

}

