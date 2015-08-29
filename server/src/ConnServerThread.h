
#ifndef _CONNECTSERVERTHREAD_H_
#define _CONNECTSERVERTHREAD_H_

#include <string>
#include <time.h>
#include <map>
#include "SDThreadPool.h"
#include "PacketQueue.h"

//�ͻ��˶��û�з��͹�����ͻᱻ�������Ͽ�����
#define ClientMaxIdleTime 180

struct fd_info
{
    int fd;
    std::string fd_account; //Ϊ�˶�Ӧfd�����ɵ���ʱ�˺�

    //�û�δ��¼��ʱ��client_idΪ0
    //�û���¼��client_id��дΪ��ʵclient_id
    unsigned int client_id;
    //�û�δѡ����Ӣ��ʱmain_hero_id��дΪ0
    //�û�ѡ����Ӣ�ۺ�main_hero_id��дΪ��ʵ��Ӣ��id
    unsigned int main_hero_id;

    //���ڷ���ʱEAGAIN����Ҫ�����������
    char* send_buf;
    int send_buf_len;  //����buf�е������ܳ�
    int send_offset;  //��Ҫ���͵���ʼƫ��

    //���ڽ���ʱEAGAIN����δ�ܴ�����Ҫ������
    char* recv_buf;
    int recv_buf_len; //����buf�ĳ���
    int recv_offset; //�Ѿ����յ����ֽ���
    bool isJustPacketHead; //Ϊtrue��ʾhead������ˣ�����buf�еȴ����յ�ֻ�ǰ�ͷ���ѡ�

    //��һ������ʱ���
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
    //���������¼�
    void doIt();
    //����ҵ�����
    void doIt2();
    //����ͻ��˳�ʱ
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


