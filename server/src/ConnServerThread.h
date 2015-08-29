
#ifndef _CONNECTSERVERTHREAD_H_
#define _CONNECTSERVERTHREAD_H_

#include <string>
#include <time.h>
#include <map>
#include "SDThreadPool.h"
#include "PacketQueue.h"

//客户端多久没有发送过请求就会被服务器断开连接
#define ClientMaxIdleTime 180

struct fd_info
{
    int fd;
    std::string fd_account; //为了对应fd而生成的临时账号

    //用户未登录的时候client_id为0
    //用户登录后client_id填写为真实client_id
    unsigned int client_id;
    //用户未选择主英雄时main_hero_id填写为0
    //用户选择主英雄后main_hero_id填写为真实主英雄id
    unsigned int main_hero_id;

    //由于发送时EAGAIN，需要缓冲的数据流
    char* send_buf;
    int send_buf_len;  //整个buf中的数据总长
    int send_offset;  //需要发送的起始偏移

    //由于接收时EAGAIN，还未能凑足需要的数据
    char* recv_buf;
    int recv_buf_len; //整个buf的长度
    int recv_offset; //已经接收到的字节数
    bool isJustPacketHead; //为true表示head被拆分了，现在buf中等待接收的只是包头而已。

    //上一次请求时间戳
    time_t last_req_time;

    fd_info()
    {
        client_id = 0;
        main_hero_id = 0;
             
        send_buf = NULL;
        send_buf_len = 0;
        send_offset = 0;
        recv_buf = NULL;
        recv_buf_len = 0;
        recv_offset = 0;
        isJustPacketHead = false;

        last_req_time = time(0);
    }

    ~fd_info()
    {
        delete send_buf;
        delete recv_buf;
    }
};



class ConnServerThread: public SDTripleThreadPool
{
public:
	ConnServerThread(std::string server_ip, unsigned int server_port
        , PacketQueue* import_q
        , PacketQueue* outport_q);
	~ConnServerThread();

protected:
    //处理网络事件
    void doIt();
    //处理业务队列
    void doIt2();
    //处理客户端超时
    void doIt3();

    void DealwithReadableSocket(int fd);
    void DealwithWritableSocket(int fd);
    void DealwithPacketFromQueue(Packet* pack);

    void DestroySocket(int fd);
    bool isPacketHeadLegal(SPHead* head);
    bool isPacketHeadAllowedWhenNotLogin(SPHead* head);
    std::string GenerateTmpaccount();

    void checkTimeoutSocket();

private:
    std::string m_server_ip;
    unsigned int m_server_port;

    int m_epoll_fd;
    int m_listen_fd;

    //
    PacketQueue* m_import_q;
    PacketQueue* m_outport_q;

    //
    pthread_mutex_t m_datamap_lock;
    std::map<int, fd_info> m_map_fd2info;
    std::map<std::string, int> m_map_account2fd;
    std::map<unsigned int, std::string> m_map_heroid2account;
};



#endif //_CONNECTSERVERTHREAD_H_


