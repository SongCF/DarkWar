
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

void child_proc()
{
    //
    g_pack_import_q = new PacketQueue(100000);
    g_pack_outport_q = new PacketQueue(100000);
    
    //start connect thread
    ConnServerThread ConnServer(g_server_ip, g_server_port
        , g_pack_outport_q, g_pack_import_q);
    if(!ConnServer.startThreads())
    {
        LogMsg("ConnServer start failed");
        return;
    }


    //start process thread
    ProcServerThread ProcServer(g_pack_import_q, g_pack_outport_q
        , g_db_ip, g_db_username, g_db_psw);
    g_procServer = &ProcServer;
    if(!ProcServer.startThreads())
    {
        LogMsg("ProcServer start failed");
        return;
    }

    

    while(1)
    {
        sleep(1);
    }
}


void signal_hander(int sig)  
{//处理子进程退出信号  
    pid_t pid;  
    
    if ((pid=fork())!=0)
    {
        wait(NULL);

        LogMsg("###########################restart child");
        
        signal(SIGCHLD,signal_hander);
    }
    else
    {
        sleep(2);
        child_proc();
    }
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
    
    pid_t pid;  
    if ((pid=fork())!=0)
    {  
         //父进程作为一个monitor监控子进程
         signal(SIGCHLD,signal_hander);

         while(1)
         {
            sleep(1);
         }
    }
    else
    {
        child_proc();
    }

    return 0;
}

