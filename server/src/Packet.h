
#ifndef _PACKET_H_
#define _PACKET_H_

#include <string>
#include "protocol.h"


struct Packet{
    //由连接线程生成的临时账号。用作与fd的对应
    std::string fd_account;

    //用户未登录的时候client_id为0
    //用户登录后client_id填写为真实client_id
    unsigned int client_id;
    //标示客户端重新登录或注册了，connectserver需要刷新
    bool new_client_id;
    //用户未选择主英雄时main_hero_id填写为0
    //用户选择主英雄后main_hero_id填写为真实主英雄id
    unsigned int main_hero_id;
    //标示客户端重新选择英雄了，connectserver需要刷新
    bool new_main_hero_id;

    //实际的网络报文
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



