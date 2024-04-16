#include"SSLsocket.h"

SSLsocket* SSLsocket::s_pinstance{nullptr};
std::mutex SSLsocket::s_mutex;



    SSLsocket::SSLsocket(const SSL_METHOD* ssl_method, const std::string& cert_file,  const std::string& key_file )
    {
        m_ssl_method = ssl_method;
        m_ssl_context = SSL_CTX_new(m_ssl_method);
    }

void SSLsocket::Destroy() 
{
    SSL_CTX_free(m_ssl_context);
    delete s_pinstance;
    s_pinstance = nullptr;
}

SSLsocket *SSLsocket::GetInstance(const SSL_METHOD* ssl_method, const std::string& cert_file, const std::string& key_file )
{
    s_mutex.lock();
    if (s_pinstance == nullptr)
    {
        s_pinstance = new SSLsocket(ssl_method, cert_file, key_file);
    }
    s_mutex.unlock();
    return s_pinstance;
}