
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <string.h>
#include <sys/wait.h>
#include "utility/logs.h"
#include "common/SDConfiguration.h"
#include "SDThreadPool.h"
#include "ConnServerThread.h"
#include "ProcServerThread.h"

#include "os.h"
#include "PacketQueue.h"



static std::string g_server_ip;
static unsigned int g_server_port;

static std::string g_db_ip;
static std::string g_db_username;
static std::string g_db_psw;


//接收数据包队列
static PacketQueue* g_pack_import_q;
//发送数据包队列
static PacketQueue* g_pack_outport_q;


//test
ProcServerThread* g_procServer;

void create_robots()
{
    //先创建10个顶级牛人
    {
        
    }

    g_procServer->create_robots();
    /*
    for(unsigned int level=29; level>=6; --level)
    {
        
        g_procServer->create_robot(Profession_Yemanren, level,robot_power_hard);
        g_procServer->create_robot(Profession_Chike, level,robot_power_hard);
        g_procServer->create_robot(Profession_Fashi, level,robot_power_hard);

        g_procServer->create_robot(Profession_Yemanren, level,robot_power_mid);
        g_procServer->create_robot(Profession_Chike, level,robot_power_mid);
        g_procServer->create_robot(Profession_Fashi, level,robot_power_mid);

        g_procServer->create_robot(Profession_Yemanren, level,robot_power_mid);
        g_procServer->create_robot(Profession_Chike, level,robot_power_mid);
        g_procServer->create_robot(Profession_Fashi, level,robot_power_mid);

        g_procServer->create_robot(Profession_Yemanren, level,robot_power_easy);
        g_procServer->create_robot(Profession_Chike, level,robot_power_easy);
        g_procServer->create_robot(Profession_Fashi, level,robot_power_easy);

        g_procServer->create_robot(Profession_Yemanren, level,robot_power_easy);
        g_procServer->create_robot(Profession_Chike, level,robot_power_easy);
        g_procServer->create_robot(Profession_Fashi, level,robot_power_easy);

        g_procServer->create_robot(Profession_Yemanren, level,robot_power_easy);
        g_procServer->create_robot(Profession_Chike, level,robot_power_easy);
        g_procServer->create_robot(Profession_Fashi, level,robot_power_easy);
        
        LogMsg("level[%u] finished", level);
    }
    */
}




int main(int argc, char* argv[])
{
	char property_file[256];

    LogInit(argv[0]);

	if(argc >= 2)
	{
		strcpy(property_file, argv[1]);
	}
	else
	{
		LogMsg("start server failed. argc=[%d]", argc);
        return 1;
	}

	//ignore some signal
	signal(SIGPIPE, SIG_IGN);
    

    //load config
    SDConfiguration conf;
    if(!conf.load(argv[1]))
    {
        LogMsg("load confige file failed. file path=[%s]", argv[1]);
        return 1;
    }
    g_server_ip = conf.getString("server_ip");
    if(g_server_ip == "")
    {
        LogMsg("server ip is null");
        return 1;
    }
    g_server_port = (unsigned int)conf.getInt("server_port");
    if(g_server_port == 0)
    {
        LogMsg("server_port is null");
        return 1;
    }
    g_db_ip = conf.getString("db_ip");
    if(g_db_ip == "")
    {
        LogMsg("db_ip is null");
        return 1;
    }
    g_db_username = conf.getString("db_username");
    if(g_db_username == "")
    {
        LogMsg("db_username is null");
        return 1;
    }
    g_db_psw = conf.getString("db_psw");
    if(g_db_psw == "")
    {
        LogMsg("db_psw is null");
        return 1;
    }
    
    //
    g_pack_import_q = new PacketQueue(100000);
    g_pack_outport_q = new PacketQueue(100000);
    

    //
    ProcServerThread ProcServer(g_pack_import_q, g_pack_outport_q
        , g_db_ip, g_db_username, g_db_psw);
    g_procServer = &ProcServer;

    create_robots();

    //g_procServer->change_robot_zb_qianghua();

    return 0;
}

