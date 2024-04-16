#include"SSLsocket.h"
#include <iostream>
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

SSLsocket* SSLsocket::s_pinstance{nullptr};
std::mutex SSLsocket::s_mutex;



    SSLsocket::SSLsocket(const SSL_METHOD* ssl_method, const std::string& cert_file,  const std::string& key_file ):
    m_ssl_method(ssl_method),
    m_cert_file(cert_file),
    m_key_file(key_file),
    m_ssl_socket(nullptr)
    {
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();

        m_ssl_context = SSL_CTX_new(m_ssl_method);
        if(!m_ssl_context)
        {
           throw std::invalid_argument("SSL context creation failled" );
        }
        
        if( m_ssl_method == TLS_server_method() )
        {
            if( !SSL_CTX_use_certificate_file(m_ssl_context, m_cert_file.c_str(), SSL_FILETYPE_PEM) ||
                 !SSL_CTX_use_PrivateKey_file(m_ssl_context, m_key_file.c_str(), SSL_FILETYPE_PEM) )
                 {
                        throw std::invalid_argument("Couldnt Load certificate files" );
                 }
        }
    }

void SSLsocket::Destroy() 
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if(s_pinstance != nullptr )
    {
        SSL_CTX_free(m_ssl_context);
        m_ssl_context = nullptr;
        delete s_pinstance;
        s_pinstance = nullptr;
    }
}

bool SSLsocket::connect( int socket)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    
    bool retval{false};

    if( m_ssl_method == TLS_client_method() && m_ssl_socket == nullptr )//only a client can connect to a server
    {
        m_ssl_socket = SSL_new(m_ssl_context);
        if( m_ssl_socket )
        {
            SSL_set_fd(m_ssl_socket, socket);
            if(SSL_connect(m_ssl_socket) == 1)
            {
                retval = true;
            }
            else
            {
                SSL_shutdown(m_ssl_socket);
                SSL_free(m_ssl_socket);
                m_ssl_socket = nullptr;
            }
        }
    }
    return retval;
}

bool SSLsocket::accept(int socket)
{
    std::lock_guard<std::mutex> lock(s_mutex);

    bool retval{false};
    if(m_ssl_method == TLS_server_method() && m_ssl_socket == nullptr )
    {
         m_ssl_socket = SSL_new(m_ssl_context);
        if( m_ssl_socket )
        {
            SSL_set_fd(m_ssl_socket, socket);
            if(SSL_accept(m_ssl_socket) == 1 )
            {
                retval = true;
            }
            else
            {
                SSL_shutdown(m_ssl_socket);
                SSL_free(m_ssl_socket);
                m_ssl_socket = nullptr;
            }
        }       
    }
    return retval;
}


void SSLsocket::shutdownCurrentSSLsocket()
{
    std::lock_guard<std::mutex> lock(s_mutex);

    SSL_shutdown(m_ssl_socket);
    SSL_free(m_ssl_socket);
    m_ssl_socket = nullptr;
}

int SSLsocket::send(const std::string& dataToSend)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    return SSL_write(m_ssl_socket, dataToSend.c_str(), dataToSend.size() );
}

std::string SSLsocket::receive(const unsigned int bufferSize )
{
        std::lock_guard<std::mutex> lock(s_mutex);

        std::string data_buffer(bufferSize, ' ');
        std::string ret_data;
        char *char_buffer = const_cast<char*>(data_buffer.data());
        int bytes_received = SSL_read( m_ssl_socket, char_buffer, bufferSize  );

        if(bytes_received > 0 )
            ret_data =  data_buffer.substr(0, bytes_received);
        else
            ret_data = "";
        return ret_data;
}

bool SSLsocket::hasReceivedCertificate() const
{    
    bool retval{false};

    if( m_ssl_socket != nullptr && SSL_get_peer_certificate(m_ssl_socket) != nullptr  )
    {
        retval = true;
    }

    return retval;
}
SSLsocket *SSLsocket::GetInstance(const SSL_METHOD* ssl_method, const std::string& cert_file, const std::string& key_file )
{
    std::lock_guard<std::mutex> lock(s_mutex);

    if (s_pinstance == nullptr)
    {
        s_pinstance = new SSLsocket(ssl_method, cert_file, key_file);
    }
    return s_pinstance;
}