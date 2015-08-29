#include <string>
#include "iOS_Push_SSLCOMM.h"


std::string g_xxxx_ssl_file_key;

CSSLComm::CSSLComm(std::string ssl_file_key)
{
    //ctor
    m_sockfd = -1;
    m_pctx = NULL;
    m_pssl = NULL;
    m_pmeth = NULL;
    m_pserver_cert = NULL;
    m_pkey = NULL;

    m_tokenLen = htons(32);
    memset((void*)&m_data,0,sizeof(m_data));

    g_xxxx_ssl_file_key = ssl_file_key;
}

CSSLComm::~CSSLComm()
{
    //dtor
    Reset();

}
void CSSLComm::Reset()
{

    if(m_pssl)
    {
        SSL_shutdown(m_pssl);
        SSL_free(m_pssl);
        m_pssl = NULL;
    }
    if(m_pctx)
    {
        SSL_CTX_free(m_pctx);
        m_pctx = NULL;
    }
    if(m_sockfd > 2)
    {
        close(m_sockfd);
        m_sockfd = -1;
    }

}


bool CSSLComm::connected()
{
    if(m_sockfd < 2) return false;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    fd_set fdwrite;
    fd_set fdexcept;
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcept);
    FD_SET(m_sockfd,&fdwrite);
    FD_SET(m_sockfd,&fdexcept);
    int ret = select(m_sockfd+1,NULL,&fdwrite,&fdexcept,&timeout);
    if(ret == -1)
        return false;
    if(ret > 0)
    {
        if(FD_ISSET(m_sockfd,&fdexcept))
            return false;
        else if(FD_ISSET(m_sockfd,&fdwrite))
        {
            int err = 0;
            socklen_t len = sizeof(err);
            int result = getsockopt(m_sockfd,SOL_SOCKET,SO_ERROR,(char*)&err,&len);
            if(result < 0 || err != 0)
                return false;
            return true;
        }
    }
    return false;
}

int pem_passwd_cb_func(char *buf, int size, int rwflag, void *userdata)
{
    memcpy(buf, g_xxxx_ssl_file_key.c_str(), g_xxxx_ssl_file_key.length());
    return g_xxxx_ssl_file_key.length();
}


bool CSSLComm::ssl_connect(const char *host, int port, const char *certfile
    , const char *keyfile, const char* capath)
{
    Reset();

    strcpy(m_server_host, host);
    m_server_port = port;
    strcpy(m_cert_file, certfile);
    strcpy(m_key_file, keyfile);
    strcpy(m_ca_path, capath);

    int err;

    /* Load encryption & hashing algorithms for the SSL program */
    SSL_library_init();

    /* Load the error strings for SSL & CRYPTO APIs */
    SSL_load_error_strings();

    /* Create an SSL_METHOD structure (choose an SSL/TLS protocol version) */
    m_pmeth = SSLv3_method();

    /* Create an SSL_CTX structure */
    m_pctx = SSL_CTX_new((SSL_METHOD*)m_pmeth);
    if(!m_pctx)
    {
        printf("Could not get SSL Context\n");
        return false;
    }

/////////////////////////////////////
SSL_CTX_set_default_passwd_cb(m_pctx, pem_passwd_cb_func);

/////////////////////////////////////

    /* Load the CA from the Path */
    if(SSL_CTX_load_verify_locations(m_pctx, NULL, capath) <= 0)
    {
        /* Handle failed load here */
        printf("Failed to set CA location...\n");
        ERR_print_errors_fp(stderr);
        return false;
    }

    /* Load the client certificate into the SSL_CTX structure */
    if (SSL_CTX_use_certificate_file(m_pctx, certfile, SSL_FILETYPE_PEM) <= 0)
    {
        printf("Cannot use Certificate File\n");
        ERR_print_errors_fp(stderr);
        return false;
    }

    /* Load the private-key corresponding to the client certificate */
    if (SSL_CTX_use_PrivateKey_file(m_pctx, keyfile, SSL_FILETYPE_PEM) <= 0)
    {
        printf("Cannot use Private Key\n");
        ERR_print_errors_fp(stderr);
        return false;
    }

    /* Check if the client certificate and private-key matches */
    if (!SSL_CTX_check_private_key(m_pctx))
    {
        printf("Private key does not match the certificate public key\n");
        return false;
    }

    /* Set up a TCP socket */
    m_sockfd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(m_sockfd == -1)
    {
        printf("Could not get Socket\n");
        return false;
    }

    memset (&m_server_addr, '\0', sizeof(m_server_addr));
    m_server_addr.sin_family      = AF_INET;
    m_server_addr.sin_port        = htons(port);       /* Server Port number */
    m_phost_info = gethostbyname(host);
    if(m_phost_info)
    {
        /* Take the first IP */
        struct in_addr *address = (struct in_addr*)m_phost_info->h_addr_list[0];
        m_server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*address)); /* Server IP */

    }
    else
    {
        printf("Could not resolve hostname %s\n", host);
        return false;
    }

    /* Establish a TCP/IP connection to the SSL client */
    err = connect(m_sockfd, (struct sockaddr*) &m_server_addr, sizeof(m_server_addr));
    if(err == -1)
    {
        printf("Could not connect\n");
        return false;
    }

    /* An SSL structure is created */
    m_pssl = SSL_new(m_pctx);
    if(!m_pssl)
    {
        printf("Could not get SSL Socket\n");
        return false;
    }

    /* Assign the socket into the SSL structure (SSL and socket without BIO) */
    SSL_set_fd(m_pssl, m_sockfd);

    /* Perform SSL Handshake on the SSL client */
    err = SSL_connect(m_pssl);
    if(err == -1)
    {
        printf("Could not connect to SSL Server\n");
        return false;
    }
    return true;

}

void CSSLComm::PushNotification(int badgeNum, const char *pToken,const char *pMsg)
{
    //CMyLock lock(&m_lock);
    if(!connected())
    {
        ssl_connect(m_server_host, m_server_port, m_cert_file, m_key_file, m_ca_path);
    }
    int paylen = GenPayloadData(badgeNum,pMsg);
    GenPushData(pToken);
    int need_send_len = 35 + paylen;
    int ret = SSL_write(m_pssl, (void*)&m_data, 35 + paylen);
    if(ret <= 0)
    {
        printf("SSL_write failed\n");
    }
    else
    {
        printf("SSL_write %d bytes of %d bytes\n", ret, need_send_len);
    }
}

void CSSLComm::GenPushData(const char *pToken)
{
    char *ptr = m_data.szToken;
    *ptr++ = 0;
    memcpy(ptr,&m_tokenLen,2);
    ptr += 2;
    memcpy(ptr,pToken,32);
}

int CSSLComm::GenPayloadData(int badgeNum,const char *pMsg)
{
    char buf[256] = {0};
    char badgeBuf[3] = {0};
    strcpy(&m_data.szPayload[2], "{\"aps\":{");
    if(pMsg != NULL)
    {
        strcat(&m_data.szPayload[2], "\"alert\":");
        snprintf(buf, sizeof(buf)-1,"\"%s\",", pMsg);
        strcat(&m_data.szPayload[2],buf);
    }
    if(badgeNum > 99 || badgeNum < 0) badgeNum = 1;
    snprintf(badgeBuf, sizeof(badgeBuf)-1,"%d", badgeNum);
    strcat(&m_data.szPayload[2], "\"badge\":");
    strcat(&m_data.szPayload[2], badgeBuf);
    strcat(&m_data.szPayload[2], ",\"sound\":\"msg.wav\"}");
    /*
        int i = 0;
        while(payload->dictKey != NULL && i < 5)
        {
            sprintf(tmpBuff, "\"%s\":\"%s\"", payload->dictKey, payload->dictValue);
            strcat(messageBuff, tmpBuff);
            if(i < 4 && payload->dictKey[i + 1] != NULL)
            {
                strcat(messageBuff, ",");
            }
            i++;
        }
    */

    snprintf(buf, sizeof(buf)-1,",\"%s\":\"%d\"", "forum_id",88);
    strcat(&m_data.szPayload[2],buf);

    snprintf(buf, sizeof(buf)-1,",\"%s\":\"%d\"", "topic_id",999);
    strcat(&m_data.szPayload[2],buf);

    strcat(&m_data.szPayload[2],"}");
    int len = strlen(&m_data.szPayload[2]);
    assert(len <= 256);
    uint16_t payload_len = htons(len);
    memcpy(m_data.szPayload,&payload_len,sizeof(payload_len));
    return len + 2;
}

