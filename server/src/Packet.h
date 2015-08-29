
#ifndef _PACKET_H_
#define _PACKET_H_

#include <string>
#include "protocol.h"


struct Packet{
    //�������߳����ɵ���ʱ�˺š�������fd�Ķ�Ӧ
    std::string fd_account;

    //�û�δ��¼��ʱ��client_idΪ0
    //�û���¼��client_id��дΪ��ʵclient_id
    unsigned int client_id;
    //��ʾ�ͻ������µ�¼��ע���ˣ�connectserver��Ҫˢ��
    bool new_client_id;
    //�û�δѡ����Ӣ��ʱmain_hero_id��дΪ0
    //�û�ѡ����Ӣ�ۺ�main_hero_id��дΪ��ʵ��Ӣ��id
    unsigned int main_hero_id;
    //��ʾ�ͻ�������ѡ��Ӣ���ˣ�connectserver��Ҫˢ��
    bool new_main_hero_id;

    //ʵ�ʵ����籨��
    char* packdata;

    Packet()
    {
        fd_account = "";
        client_id = 0;
        new_client_id = false;
        main_hero_id = 0;
        new_main_hero_id = false;
    };

    ~Packet(){delete packdata;};
};

#endif //_PACKET_H_



