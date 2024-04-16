#ifndef __SSLSOCKET_H__
#define __SSLSOCKET_H__

#include <mutex>
#include <string>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class SSLsocket
{

public:

    SSLsocket(SSLsocket &other) = delete;

    SSLsocket& operator=(const SSLsocket &) = delete;

    static SSLsocket *GetInstance(const SSL_METHOD* ssl_method = TLS_server_method(), const std::string& cert_file="", 
                                                               const std::string& key_file = "" );
    void Destroy();

    int setCurrentSocket(int socket);
    int accept();
    int shutdownCurrentSocket();
    std::string receive(const int bufferSize);
    int send(const std::string& dataTosend);
    bool hasReceivedCertificate() const;
    bool cmpRemoteHostCert() const;

protected:
    SSLsocket(const SSL_METHOD* ssl_method, const std::string& cert_file="", 
                        const std::string& key_file = "" );
    ~SSLsocket() {};

private:
    static SSLsocket * s_pinstance;
    static std::mutex s_mutex;;

    SSL_CTX * m_ssl_context;
    const SSL_METHOD * m_ssl_method;
    const std::string m_cert_file;
    const std::string m_key_file;
    SSL * m_ssl_socket;
    int m_socket;
    
};


#endif