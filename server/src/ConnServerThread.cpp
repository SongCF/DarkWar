

#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <utility>
#include <string.h>

#include "ConnServerThread.h"
#include "utility/os.h"
#include "logs.h"
#include "memguard.h"

//支持最大并发数
#define EPOLL_MAX_FD 100000

ConnServerThread::ConnServerThread( std::string server_ip,unsigned int server_port
    ,PacketQueue * import_q,PacketQueue * outport_q)
:SDTripleThreadPool(1,1,1)
{
    m_server_ip = server_ip;
    m_server_port = server_port;

    m_import_q = import_q;
    m_outport_q = outport_q;

    pthread_mutex_init(&m_datamap_lock, NULL);
}

ConnServerThread::~ConnServerThread()
{
    pthread_mutex_destroy(&m_datamap_lock);
}

void ConnServerThread::doIt()
{
    m_listen_fd = OS::CreateTCPSock();
    if(m_listen_fd < 0)
    {
        LogMsg("ConnServer CreateTCPSock failed. server abort");
        
        exit(1);
        return;
    }

    if(OS::Bind(m_listen_fd, m_server_port, m_server_ip.c_str()) < 0)
    {
        LogMsg("ConnServer bind failed. server abort");
        
        exit(1);
        return;
    }

    if(OS::Listen(m_listen_fd) < 0)
    {
        LogMsg("ConnServer Listen failed. server abort");
        
        exit(1);
        return;
    }

    //当前设置epoll最大并发10万
    struct epoll_event* event_arr = new struct epoll_event[EPOLL_MAX_FD];
    m_epoll_fd = epoll_create(EPOLL_MAX_FD);
    if(m_epoll_fd < 0)
    {
        LogMsg("epoll_create failed. server abort");
        
        exit(1);
        return;
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = m_listen_fd;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &event) != 0)
    {
        LogMsg("epoll_ctl failed. server abort");
        
        exit(1);
    }

    while(1)
    {
        int nfds = epoll_wait(m_epoll_fd, event_arr, EPOLL_MAX_FD, -1);
        if (nfds == -1) 
        {
            if(errno == EINTR)
            {
                continue;
            }
            
            int tmp_errno = errno;
            LogMsg("epoll_wait failed. server abort[%s]", strerror(tmp_errno));
            exit(1);
        }
        
        for (int i = 0; i < nfds; ++i)
        {
            if (event_arr[i].data.fd == m_listen_fd)
            {
                //有新连接
                int fd = OS::Accept(m_listen_fd);
                if(fd < 0)
                {
                    LogMsg("OS::Accept failed.");
                    continue;
                }
                if(fd >= EPOLL_MAX_FD)
                {
                    LogMsg("fd beyond %d. fd=[%d]", EPOLL_MAX_FD, fd);
                    close(fd);
                    continue;
                }
#ifdef _DEBUG
char* peer_ip;
int peer_port;
OS::GetPeerInfor(fd,peer_ip,peer_port);
LogMsg("%s, line %d, new client connected. fd=[%d] ip=[%s] port=[%d]"
    , __FILE__, __LINE__, fd, peer_ip, peer_port);
#endif //_DEBUF

                event.events = EPOLLIN ;
                event.data.fd = fd;
                if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event) != 0)
                {
                    LogMsg("epoll_ctl failed. server abort");
                    exit(1);
                }
            }
            else
            {
                //锁住datamap
                pthread_mutex_lock(&m_datamap_lock);
                if(event_arr[i].events & EPOLLIN)
                {
                    DealwithReadableSocket(event_arr[i].data.fd);
                }
                if(event_arr[i].events & EPOLLOUT)
                {
                    DealwithWritableSocket(event_arr[i].data.fd);
                }
                pthread_mutex_unlock(&m_datamap_lock);
            }
        }
    }
}

void ConnServerThread::doIt2()
{
    while(1)
    {
        //从入队列阻塞读取
        Packet* pack;
        m_import_q->pop(pack);

        //锁住datamap
        pthread_mutex_lock(&m_datamap_lock);
        DealwithPacketFromQueue(pack);
        pthread_mutex_unlock(&m_datamap_lock);
        
        delete pack;
    }
}

void ConnServerThread::doIt3()
{
    while(1)
    {
        sleep(30);

        //锁住datamap
        pthread_mutex_lock(&m_datamap_lock);
        
        //检查是否有超时idle的socket
        checkTimeoutSocket();

        pthread_mutex_unlock(&m_datamap_lock);
    }
}


void ConnServerThread::DealwithReadableSocket(int fd)
{
    //是否已经生成临时账号
    std::map<int, fd_info>::iterator fd_pos = m_map_fd2info.find(fd);
    if(fd_pos == m_map_fd2info.end())
    {
        fd_info info;
        info.fd = fd;
        info.fd_account = GenerateTmpaccount();

        m_map_fd2info.insert(make_pair(fd, info));
        m_map_account2fd.erase(info.fd_account);
        m_map_account2fd.insert(make_pair(info.fd_account, fd));

        fd_pos = m_map_fd2info.find(fd);
    }

    //刷新idle时间戳
    fd_pos->second.last_req_time = time(0);

    bool isWholePacketRecved = false;
    char* head_buf = NULL;
    char* pack_buf = NULL;
    
    //
    if(fd_pos->second.recv_buf != NULL)
    {//上次还有包未读取完
#ifdef _DEBUG
LogMsg("%s, line %d,has packet not finished recving."
    "fd=[%d] recv_buf_len=[%d] recv_offset=[%d] "
    , __FILE__, __LINE__, fd, fd_pos->second.recv_buf_len, fd_pos->second.recv_offset);
#endif //_DEBUF


        int need_recv_len = fd_pos->second.recv_buf_len - fd_pos->second.recv_offset;
        int recved_len = recv(fd, fd_pos->second.recv_buf + fd_pos->second.recv_offset
                            , need_recv_len, MSG_DONTWAIT);
        if(recved_len <= 0)
        {
            DestroySocket(fd);
            return;
        }
        if(recved_len < need_recv_len)
        {//还没有收完
            fd_pos->second.recv_offset += recved_len;
            return;
        }

        //数据接收完成
        if(fd_pos->second.isJustPacketHead)
        {//只是接收完成包头
            isWholePacketRecved = false;

            head_buf = fd_pos->second.recv_buf;
            fd_pos->second.recv_buf = NULL;
            fd_pos->second.recv_buf_len = 0;
            fd_pos->second.recv_offset = 0;
            fd_pos->second.isJustPacketHead = false;
        }
        else
        {//整个包体都接收完
            isWholePacketRecved = true;
            
            pack_buf = fd_pos->second.recv_buf;
            fd_pos->second.recv_buf = NULL;
            fd_pos->second.recv_buf_len = 0;
            fd_pos->second.recv_offset = 0;
            fd_pos->second.isJustPacketHead = false;
        }
    }
    else
    {
        //从头开始接收报文
        head_buf = new char[sizeof(SPHead)];
        int recved_len = recv(fd, head_buf, sizeof(SPHead), MSG_DONTWAIT);
        if(recved_len <= 0)
        {
#ifdef _DEBUG
LogMsg("%s, line %d,recv failed.fd=[%d]", __FILE__, __LINE__, fd);
#endif //_DEBUF

            delete head_buf;
            head_buf = NULL;
            DestroySocket(fd);
            return;
        }
        if(recved_len < sizeof(SPHead))
        {//包头被拆分
#ifdef _DEBUG
LogMsg("%s, line %d,packet head has been splited. fd=[%d]", __FILE__, __LINE__, fd);
#endif //_DEBUF

            fd_pos->second.recv_buf = head_buf;
            fd_pos->second.recv_buf_len = sizeof(SPHead);
            fd_pos->second.recv_offset = recved_len;
            fd_pos->second.isJustPacketHead = true;

            return;
        }

        //这里只是完成包头的接收
        isWholePacketRecved = false;
    }

    if(!isWholePacketRecved)
    {//前面只完成了包头的接收
        //包头接收完，开始接收包体
        SPHead* head = (SPHead*)(head_buf);
        CMemGuard head_buf_guard(head_buf);
        
        if(!isPacketHeadLegal(head))
        {
            DestroySocket(fd);
            return;
        }
        pack_buf = new char[sizeof(SPHead) + ntohl(head->data_len)];
        memcpy(pack_buf, head_buf, sizeof(SPHead));
        if(head->data_len != 0)
        {
            int recved_len = 
                recv(fd, pack_buf+sizeof(SPHead), ntohl(head->data_len), MSG_DONTWAIT);
            if(recved_len <= 0)
            {
                delete pack_buf;
                DestroySocket(fd);
                return;
            }
            if(recved_len < ntohl(head->data_len))
            {//包未收完，暂存
                fd_pos->second.recv_buf = pack_buf;
                fd_pos->second.recv_buf_len = sizeof(SPHead) + ntohl(head->data_len);
                fd_pos->second.recv_offset = sizeof(SPHead) + recved_len;
                fd_pos->second.isJustPacketHead = false;
                

#ifdef _DEBUG
    LogMsg("%s, line %d,packet not finished recving."
        "fd=[%d] recv_buf_len=[%d] recv_offset=[%d] "
        , __FILE__, __LINE__, fd, fd_pos->second.recv_buf_len, fd_pos->second.recv_offset);
#endif //_DEBUF


                return;
            }
        }
    }

    
    //整个包都接收完了
    Packet* pack = new Packet;
    pack->fd_account = fd_pos->second.fd_account;
    pack->client_id = fd_pos->second.client_id;
    pack->main_hero_id = fd_pos->second.main_hero_id;
    pack->packdata = pack_buf;
    m_outport_q->push(pack);
}

void ConnServerThread::DealwithWritableSocket(int fd)
{//只有在发送返回EAGAIN后，才会监听EPOLLOUT事件

#ifdef _DEBUG
LogMsg("%s, line %d,ConnServerThread::DealwithWritableSocket."
    , __FILE__, __LINE__);
#endif //_DEBUF

    std::map<int, fd_info>::iterator fd_pos = m_map_fd2info.find(fd);
    if(fd_pos == m_map_fd2info.end())
    {
        //这种情况不应该出现
        LogMsg("%s, line %d, shit. fd=[%d]", __FILE__, __LINE__, fd);
        DestroySocket(fd);
        
        return;
    }

    //取出待发送的数据包 
    int need_send_len = fd_pos->second.send_buf_len - fd_pos->second.send_offset;
    int send_len = send(fd, fd_pos->second.send_buf + fd_pos->second.send_offset
                        , need_send_len, MSG_DONTWAIT);
    if(send_len <= 0)
    {
        //socket异常，交由read事件统一处理
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLIN;
        if(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
        {
            LogMsg("%s, line %d, epoll_ctl failed. fd=[%d]", __FILE__, __LINE__, fd);
            DestroySocket(fd);
            return;
        }
        return;
    }
    if(send_len < need_send_len)
    {
        fd_pos->second.send_offset += send_len;
        return;
    }

    //包发送完成
    fd_pos->second.send_buf_len = 0;
    fd_pos->second.send_offset = 0;
    delete fd_pos->second.send_buf;
    fd_pos->second.send_buf = NULL;

    //恢复监听read事件
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
    {
        LogMsg("%s, line %d, epoll_ctl failed. fd=[%d]", __FILE__, __LINE__, fd);
        DestroySocket(fd);
        return;
    }
}



void ConnServerThread::DealwithPacketFromQueue(Packet * pack)
{
#ifdef _DEBUG
LogMsg("%s, line %d,ConnServerThread::DealwithPacketFromQueue", __FILE__, __LINE__);
#endif //_DEBUG

    //找fd
    int fd;
    
    //如果pack中fd_account为空，则表示是服务器主动发起的消息。以hero_id为索引去找
    //否则直接用fd_account去找
    if(pack->fd_account == "")
    {
        //先找fd_account
        std::map<unsigned int, std::string>::iterator heroid_pos
            = m_map_heroid2account.find(pack->main_hero_id);
        if(heroid_pos == m_map_heroid2account.end())
        {
            //找不到，表示该英雄已经断开并且还没有重连成功
#ifdef _DEBUG
LogMsg("%s, line %d, PacketState_SendFailed. hero_id=[%u]", __FILE__, __LINE__
        , pack->main_hero_id);
#endif //_DEBUG

            return;
        }
        else
        {
            //
            std::map<std::string, int>::iterator account_pos 
                = m_map_account2fd.find(heroid_pos->second);
            if(account_pos == m_map_account2fd.end())
            {//找不到，则表示用户已经断开连接。
#ifdef _DEBUG
        LogMsg("%s, line %d, PacketState_SendFailed. account=[%s]", __FILE__, __LINE__
            , pack->fd_account.c_str());
#endif //_DEBUG

                //删除映射 
                m_map_heroid2account.erase(pack->main_hero_id);

                return;
            }
            
            fd = account_pos->second;
        }
    }
    else
    {
        //
        std::map<std::string, int>::iterator account_pos 
            = m_map_account2fd.find(pack->fd_account);
        if(account_pos == m_map_account2fd.end())
        {//找不到，则表示用户已经断开连接。
#ifdef _DEBUG
    LogMsg("%s, line %d, PacketState_SendFailed. account=[%s]", __FILE__, __LINE__
        , pack->fd_account.c_str());
#endif //_DEBUG

            return;
        }
        fd = account_pos->second;
    }

    
    

    //
    std::map<int, fd_info>::iterator fd_pos = m_map_fd2info.find(fd);
    if(fd_pos == m_map_fd2info.end())
    {
        //这种情况不应该出现
        LogMsg("%s, line %d, shit. fd=[%d]", __FILE__, __LINE__, fd);
        DestroySocket(fd);
        
        return;
    }

    //判断当前fd是否login
    if(fd_pos->second.client_id == 0 || pack->new_client_id)
    {
        fd_pos->second.client_id = pack->client_id;
    }
    if(fd_pos->second.main_hero_id == 0 || pack->new_main_hero_id)
    {
        fd_pos->second.main_hero_id = pack->main_hero_id;

        //如果hero_id确定，则加入到heroid2info的映射中
        if(pack->main_hero_id > 0)
        {
            //先查找是否已经有映射
            std::map<unsigned int, std::string>::iterator pos = m_map_heroid2account.find(pack->main_hero_id);
            if(pos == m_map_heroid2account.end())
            {
                //创建一个
                m_map_heroid2account.insert(make_pair(pack->main_hero_id, pack->fd_account));
            }
            else
            {
                //更新
                pos->second = pack->fd_account;
            }
        }
    }

    //
    if(fd_pos->second.send_buf != NULL)
    {//fd下已经有待发送的数据,接起来
#ifdef _DEBUG
LogMsg("%s, line %d,has packet not finished sending."
    "fd=[%d] buf_total_len=[%d] recv_offset=[%d] "
    , __FILE__, __LINE__, fd, fd_pos->second.send_buf_len, fd_pos->second.send_offset);
#endif //_DEBUF

        SPHead* head = (SPHead*)(pack->packdata);
        int buf_len = fd_pos->second.send_buf_len - fd_pos->second.send_offset 
                        + sizeof(SPHead) + ntohl(head->data_len);
        char* buf = new char[buf_len];
        
#ifdef _DEBUG
LogMsg("%s, line %d,memcpy(%p, %p, %d)."
    , __FILE__, __LINE__
    , buf, fd_pos->second.send_buf+fd_pos->second.send_offset, buf_len);
#endif //_DEBUF

        memcpy(buf, fd_pos->second.send_buf+fd_pos->second.send_offset
            , fd_pos->second.send_buf_len - fd_pos->second.send_offset);
        memcpy(buf + fd_pos->second.send_buf_len - fd_pos->second.send_offset
            , pack->packdata, sizeof(SPHead) + htonl(head->data_len));
        
        delete fd_pos->second.send_buf;
        fd_pos->second.send_buf = NULL;

        fd_pos->second.send_buf = buf;
        fd_pos->second.send_buf_len = buf_len;
        fd_pos->second.send_offset = 0;

        return;
    }

    //
    SPHead* head = (SPHead*)(pack->packdata);
    int need_send_len = sizeof(SPHead) + ntohl(head->data_len);
    int send_len = send(fd, pack->packdata,need_send_len, MSG_DONTWAIT);
    if((send_len <= 0) && (errno != EAGAIN))
    {//连接断开，发给业务线程进行push
#ifdef _DEBUG
LogMsg("%s, line %d, send failed. [%s]", __FILE__, __LINE__, strerror(errno));
#endif //_DEBUG

        DestroySocket(fd);
        
        return;
    }
    if((send_len <= 0) && (errno == EAGAIN))
    {
#ifdef _DEBUG
LogMsg("%s, line %d,packet not finished sending."
    "fd=[%d] send_len=[%d] need_send_len=[%d] "
    , __FILE__, __LINE__, fd, send_len, need_send_len);
#endif //_DEBUF

        //放入待发送缓存
        fd_pos->second.send_buf = new char[need_send_len];
        fd_pos->second.send_buf_len = need_send_len;
        fd_pos->second.send_offset = 0;
        memcpy(fd_pos->second.send_buf, pack->packdata, need_send_len);

        //监听写事件
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLOUT | EPOLLIN;
        if(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
        {
            LogMsg("%s, line %d, epoll_ctl failed. fd=[%d]"
                    , __FILE__, __LINE__, fd);
            DestroySocket(fd);
            
            return;
        }

        return;
    }

    if(send_len < need_send_len)
    {
#ifdef _DEBUG
LogMsg("%s, line %d,packet not finished sending."
    "fd=[%d] send_len=[%d] need_send_len=[%d] "
    , __FILE__, __LINE__, fd, send_len, need_send_len);
#endif //_DEBUF


        //放入待发送缓存
        fd_pos->second.send_buf = new char[need_send_len - send_len];
        fd_pos->second.send_buf_len = need_send_len - send_len;
        fd_pos->second.send_offset = 0;
        memcpy(fd_pos->second.send_buf, pack->packdata + send_len
            , need_send_len - send_len);

        //监听写事件
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = EPOLLOUT | EPOLLIN;
        if(epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0)
        {
            LogMsg("%s, line %d, epoll_ctl failed. fd=[%d]"
                    , __FILE__, __LINE__, fd);
            DestroySocket(fd);
            
            return;
        }

        return;
    }

    if(send_len == need_send_len)
    {//发送完
    
        return;
    }
}

void ConnServerThread::checkTimeoutSocket()
{
    time_t current_time = time(0);

    std::vector<int> fd_vec;
    
    std::map<int, fd_info>::iterator fd_pos = m_map_fd2info.begin();
    while(fd_pos != m_map_fd2info.end())
    {
        if(current_time - fd_pos->second.last_req_time > ClientMaxIdleTime)
        {
            fd_vec.insert(fd_vec.end(), fd_pos->second.fd);
        }
        
        ++fd_pos;
    }

    for(int i=0; i<fd_vec.size(); ++i)
    {
#ifdef _DEBUG
LogMsg("%s, line %d,fd=[%d] timeout."
    , __FILE__, __LINE__, fd_vec[i]);
#endif //_DEBUF

        DestroySocket(fd_vec[i]);
    }
}

void ConnServerThread::DestroySocket(int fd)
{

    //销毁正式账号对应
    std::map<int, fd_info>::iterator fd_pos
        = m_map_fd2info.find(fd);
    if(fd_pos != m_map_fd2info.end())
    {
        delete fd_pos->second.send_buf;
        fd_pos->second.send_buf = NULL;
        delete fd_pos->second.recv_buf;
        fd_pos->second.recv_buf = NULL;
        
        m_map_account2fd.erase(fd_pos->second.fd_account);
        m_map_fd2info.erase(fd);
        m_map_heroid2account.erase(fd_pos->second.main_hero_id);
    }

    OS::CloseSock(fd);
}

std::string ConnServerThread::GenerateTmpaccount()
{
    static unsigned int i_tmp_account = 1;
    i_tmp_account++;

    char account[16];
    sprintf(account, "%u", i_tmp_account);

    return account;
}


bool ConnServerThread::isPacketHeadLegal(SPHead * head)
{
    if((ntohs(head->protocol_tag) != SimpleProtocol_Tag_Client))
    {
#ifdef _DEBUG
LogMsg("%s, line %d, packethead illegal. protocol_tag=[%d]"
                    , __FILE__, __LINE__, (int)ntohs(head->protocol_tag));
#endif
        return false;
    }

    if(ntohl(head->data_len) > 1024*5)
    {
#ifdef _DEBUG
LogMsg("%s, line %d, packethead illegal. data_len=[%d]"
                    , __FILE__, __LINE__, (int)ntohl(head->data_len));
#endif

        return false;
    }

    return true;
}

bool ConnServerThread::isPacketHeadAllowedWhenNotLogin(SPHead * head)
{
    if(!isPacketHeadLegal(head))
    {
        return false;
    }

    return true;
}



