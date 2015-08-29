#include "NetWorkThreadProc.h"
#include "cocos2d.h"
#include <queue>
#include "protocol.h"
#include "GlobalNetworkMsgDispacher.h"

#ifdef WIN32
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

using namespace cocos2d;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define DEFAULT_SERVER_IP		"192.168.2.13"
//#define DEFAULT_SERVER_IP		"118.192.93.181"

#else
#define DEFAULT_SERVER_IP		"118.192.93.181"
//#define DEFAULT_SERVER_IP		"192.168.2.100"
#endif
#define DEFAULT_SERVER_PORT		23566

#define SEND_TIME_OUT	2000	// 发送超时
#define RECV_TIME_OUT	3600000	// 接收超时

static pthread_t s_netSendThread;
static pthread_t s_netRecvThread;
static ODSocket cSocket; 

enum NetworkStat
{
	NetworkStat_NotConnect = 1,
	NetworkStat_Connected   = 2,
};
static NetworkStat g_NetworkStat = NetworkStat_NotConnect;

enum RecvThreadStat
{
	RecvThreadStat_Check     = 1,
	RecvThreadStat_Recv      = 2,
};
static RecvThreadStat g_RecvThreadStat = RecvThreadStat_Check;

// 发送消息队列
static queue<RequestMessage*> requestMsgQueue;
static pthread_mutex_t    requestMsgQueueMutex;

//心跳包，需要有上层填写。由send线程负责一段时间发送一次
RequestMessage* g_HeartBeatMsg = NULL;
time_t g_LastHeartBeatTime = time(0);

//多久发一次心跳
#define HeartBeatTimeGap 60

MessageAutoLock::MessageAutoLock()
{
	pthread_mutex_lock(&requestMsgQueueMutex);
}
MessageAutoLock::~MessageAutoLock()
{
	pthread_mutex_unlock(&requestMsgQueueMutex);
}
// 网络字节序的头部校验
bool isHeadValied(SPHead* head)
{
	int pro_tag = ntohs(head->protocol_tag);
	int pro_ver = ntohs(head->protocol_ver);

	if ( pro_tag != SimpleProtocol_Tag_Server)
	{
		CCLog("isHeadValied ---> false ,protocol_tag != SimpleProtocol_Tag_Server,currenttag = %d,needTag = %d",ntohl(head->protocol_tag),SimpleProtocol_Tag_Server);
		return false;
	}
	if ( pro_ver != Current_ProtocolVersion)
	{
		CCLog("isHeadValied ---> false ,protocol_tag != %d,currenttag = %d",Current_ProtocolVersion,ntohl(head->protocol_ver));
		return false;
	}
	return true;
}

void closeConnection()
{
	if (cSocket.Close() != 0)
	{
		CCLog("ODSocket :closeConnection Close Error,errCode = %d!",cSocket.GetError());
	}
	if (cSocket.Clean() != 0)
	{
		CCLog("ODSocket :closeConnection Clean Error,errCode = %d!",cSocket.GetError());
	}
	//
	g_NetworkStat = NetworkStat_NotConnect;
	CCLog(" -----Network closed-----");
	//
	g_NetworkStat = NetworkStat_NotConnect;


	MessageAutoLock lock;
	RequestMessage* temp;
	int count = 0;
	while(!requestMsgQueue.empty())
	{
		count ++;
		temp = requestMsgQueue.front();
		requestMsgQueue.pop();
		delete temp;
	}
	CCLog("%s --> throwed %d request packet(s)",__FUNCTION__,count);


	// 这也不需要，因为ODSocket::Clean()有调用这个函数
//#ifdef WIN32
//	WSACleanup();
//#endif
}

bool connectToServer()
{
	if (g_NetworkStat != NetworkStat_NotConnect)
		return true;

	if (cSocket.Init() == -1)
	{
		CCLog("\n==============================================================");
		CCLog("ODSocket: connectToServer Init error,errorCode = %d!", cSocket.GetError());
		CCLog("==============================================================\n");
		// 通知链接失败
		GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));
		return false;
	}
	if (cSocket.Create(AF_INET,SOCK_STREAM,0) == false)
	{
		CCLog("\n==============================================================");
		CCLog("ODSocket: connectToServer Create error,errorCode = %d!",cSocket.GetError());
		CCLog("==============================================================\n");

		// 通知链接失败
		GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));
		return false;
	}
	if (cSocket.Connect(NetWorkThreadProc::getProc()->getCurrentIp(),NetWorkThreadProc::getProc()->getCurrentPort()) == false)
	{
		CCLog("\n==========================================================================");
		CCLog("ODSocket: connectToServer[%s:%d] Connect error,errorCode = %d!",NetWorkThreadProc::getProc()->getCurrentIp(),NetWorkThreadProc::getProc()->getCurrentPort(),cSocket.GetError());
		CCLog("============================================================================\n");

		closeConnection();

		// 通知链失败
		GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));

		return false;
	}
	CCLog("\n\n");
	CCLog("\n==============================================================");
	CCLog("ODSocket : connectToServer %s-%d Success!",NetWorkThreadProc::getProc()->getCurrentIp(),NetWorkThreadProc::getProc()->getCurrentPort());
	CCLog("==============================================================\n");
	CCLog("\n\n");
	//cSocket.setT

	//设置状态
	g_NetworkStat = NetworkStat_Connected;

	// 设置超时
	{
		cSocket.SetRecvTimeOut(RECV_TIME_OUT);
		cSocket.SetSendTimeOut(SEND_TIME_OUT);
	}

	// 这个在init时已经调用了，这里不需要了，而且WSAStartup应该是winSocket第一个调用的函数
	// 所以写在这里也是不对的
//#ifdef WIN32
//	WORD wVersionRequested;
//	WSADATA wsaData;
//	int err;
//
//	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
//	wVersionRequested = MAKEWORD(2, 2);
//
//	err = WSAStartup(wVersionRequested, &wsaData);
//	if (err != 0) {
//		/* Tell the user that we could not find a usable */
//		/* Winsock DLL.                                  */
//		CCLog("WSAStartup failed");
//
//		closeConnection();
//
//		return false;
//	}
//#endif

	// 通知链接成功
	GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Suc,NULL,NULL));

	return true;
}

void lfSleep(long milltime)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	Sleep(milltime);
#else
	usleep(milltime*1000);
#endif
}


// 负责发送数据，发送成功后修改接受线程的状态为recv，让接受线程开始接受请求返回包(心跳+数据包都是)
static void* sendTask(void* data)
{
	while (true)
	{
		//如果接收线程当前还在收上一个请求包的响应则不再发送新的请求
		if(g_RecvThreadStat == RecvThreadStat_Recv)
		{
			lfSleep(100);
			continue;
		}

		//如果需要心跳，则先心跳。否则才从队列中取请求包
		int time_gap = time(0) - g_LastHeartBeatTime;
		RequestMessage* msg = NULL;
		if(time_gap >= HeartBeatTimeGap && g_HeartBeatMsg != NULL)
		{
			//更新时间
			g_LastHeartBeatTime = time(0);

			msg = new RequestMessage(g_HeartBeatMsg);
		}
		else
		{
			// 加锁
			pthread_mutex_lock(&requestMsgQueueMutex);
			if (requestMsgQueue.empty())
			{
				// 解锁
				pthread_mutex_unlock(&requestMsgQueueMutex);
			}
			else
			{
				msg = requestMsgQueue.front();
				requestMsgQueue.pop();

				// 解锁
				pthread_mutex_unlock(&requestMsgQueueMutex);
			}
		}

		//当前没有需要发送的请求
		if(msg == NULL)
		{
			lfSleep(100);
			continue;
		}

		//发送
		{
			//判断当前网络是否连上
			if(g_NetworkStat == NetworkStat_NotConnect)
			{
				CCLog("%s:%d --> g_NetworkStat == NetworkStat_NotConnect post send error",__FUNCTION__,__LINE__);

				// 通知发送数据出错
				GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Send_Err,NULL,NULL));

				delete msg;

				continue;
			}

			CCLog("sendTask : send data .....");
			int l = cSocket.Send(msg->getData(),msg->getDataLength(),0);
			//
			if ( l != msg->getDataLength())
			{
				CCLog("sendTask : send data error! length = %d, sended length = %d",msg->getDataLength(),l);

				// 通知发送数据出错
				GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Send_Err,NULL,NULL));

				delete msg;

				//出问题了
				closeConnection();

				continue;
			}
			else
			{
				delete msg;

				//发送成功，修改recv线程的标志
				g_RecvThreadStat = RecvThreadStat_Recv;

				CCLog("sendTask : send data success");
			}
		}
	}
	return 0;
}

//recv线程需要负责网络检测，重连。
static void* recvTask(void* data)
{
	while (true)
	{
		if(g_NetworkStat == NetworkStat_NotConnect)
		{
			//发起网络连接
			if(connectToServer())
			{
				//连接成功，修改相应状态
				g_NetworkStat = NetworkStat_Connected;

				//recv线程初始化为check状态 
				g_RecvThreadStat = RecvThreadStat_Check;

				continue;
			}
			else
			{
				lfSleep(2000);
				continue;
			}
		}

		//if(g_RecvThreadStat == RecvThreadStat_Check)
		//{
		//	//通过select检测网络状态
		//	fd_set fdset;
		//	FD_ZERO(&fdset);
		//	FD_SET(cSocket.getSock(), &fdset);
		//	struct timeval timev;
		//	timev.tv_sec = 0;
		//	timev.tv_usec = 0;

		//	//非阻塞select 
		//	int ret = select(cSocket.getSock() + 1, &fdset, 0, 0, &timev);
		//	if(ret == 0)
		//	{
		//		//一切正常休息一下
		//		lfSleep(250);
		//	}
		//	else
		//	{
		//		//有recv fdset有返回，在没有发送请求包的情况下，只能是网络断掉了
		//		closeConnection();
		//		// 通知接收数据出错
		//		GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));
		//		continue;
		//	}
		//}
		//else if(g_RecvThreadStat == RecvThreadStat_Recv)
		{
			CCLog("recvTask : recv data .....");
			// 读取头部
			char* head = new char[sizeof(SPHead)];
			int l = cSocket.Recv(head,sizeof(SPHead),0);
			if ( l != sizeof(SPHead))
			{
				CCLog("recvTask : recv Head error , total length = %d, readed length = %d",sizeof(SPHead),l);

				// 通知接收数据出错
				if(g_RecvThreadStat == RecvThreadStat_Check)
				{
					CCLog("recvTask : recv Head error,g_RecvThreadStat == RecvThreadStat_Check");
					GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));
				}
				else
				{
					CCLog("recvTask : recv Head error,g_RecvThreadStat != RecvThreadStat_Check");
					GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Recv_Err,NULL,NULL));
				}
				

				delete head;

				// 出问题了
				closeConnection();
				continue;
			}

			if (!isHeadValied((SPHead*)head))
			{
				CCLog("recvTask : recv Head is inValied");

				// 通知接收数据出错
				// 通知接收数据出错
				if(g_RecvThreadStat == RecvThreadStat_Check)
				{
					CCLog("recvTask : recv Head is inValied   g_RecvThreadStat == RecvThreadStat_Check");
					GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));
				}
				else
				{
					CCLog("recvTask : recv Head is inValied   g_RecvThreadStat != RecvThreadStat_Check");
					GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Recv_Err,NULL,NULL));
				}

				delete head;

				//出问题了
				closeConnection();
				continue;
			}

			CCLog("recvTask : recv head success!");

			// 获取数据包
			int data_len = ntohl(((SPHead*)head)->data_len);
			char* body = new char[data_len];

			int l_body = cSocket.Recv(body,data_len,0);
			if ( l_body != data_len)
			{
				CCLog("recvTask : recv body error , total length = %d, readed length = %d",data_len,l_body);

				// 通知接收数据出错
				// 通知接收数据出错
				if(g_RecvThreadStat == RecvThreadStat_Check)
				{
					CCLog("recvTask : recv body error,g_RecvThreadStat == RecvThreadStat_Check");
					GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Conncet_Err,NULL,NULL));
				}
				else
				{
					CCLog("recvTask : recv body error,g_RecvThreadStat != RecvThreadStat_Check");
					GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Recv_Err,NULL,NULL));
				}


				delete body;
				delete head;

				//出问题了
				closeConnection();
				continue;
			}

			CCLog("recvTask : recv body success!");

			// 通知接收数据成功
			GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_RecvData,head,body));

			//修改标志
			g_RecvThreadStat = RecvThreadStat_Check;
		}
		/*else
		{
			CCAssert(false, "stat error");
			lfSleep(1000);
			continue;
		}*/
	}
	return 0;
	
}

NetWorkThreadProc* NetWorkThreadProc::mProc = NULL;

NetWorkThreadProc::NetWorkThreadProc(void)
{
	// 初始化锁
	pthread_mutex_init(&requestMsgQueueMutex, NULL);
	serverIp = DEFAULT_SERVER_IP;
	serverPort = DEFAULT_SERVER_PORT;
	isRuning = false;
}

NetWorkThreadProc* NetWorkThreadProc::getProc()
{
	if (mProc == NULL)
	{
		mProc = new NetWorkThreadProc();
	}
	return mProc;
}
void NetWorkThreadProc::closeNetwork()
{
	closeConnection();
}

void NetWorkThreadProc::initNetwork(const char* serverIp /* = NULL */,unsigned short serverPort /* = 0 */)
{
	if (serverIp != NULL && strlen(serverIp) > 0)
	{
		this->serverIp = serverIp;
	}
	if (serverPort > 0)
	{
		this->serverPort = serverPort;
	}
}
const char* NetWorkThreadProc::getCurrentIp()
{
	return serverIp.c_str();
}
unsigned short NetWorkThreadProc::getCurrentPort()
{
	return serverPort;
}
void NetWorkThreadProc::startThread()
{
	if (isRuning)
	{
		CCLog("%s : --> is already running,no need run again");
		return;
	}
	//pthread_
	pthread_create(&s_netSendThread, NULL, sendTask, NULL);
	pthread_create(&s_netRecvThread, NULL, recvTask, NULL);

	isRuning = true;
}

void NetWorkThreadProc::sendData(RequestMessage* msg)
{
	// 还没有连接到网络
	if (g_NetworkStat == NetworkStat_NotConnect)
	{
		CCLog("Error : %s:%d---> g_NetworkStat == NetworkStat_NotConnect .throw packet.",__FUNCTION__,__LINE__);
		// 通知还没连接到网络
		GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Send_Err,NULL,NULL));
		delete msg;
		return;
	}
	MessageAutoLock lock;
	//// 有数据还没处理
	//if (requestMsgQueue.size() > 0)
	//{
	//	CCAssert(false,"为了以后方便，就不要一次发多个请求");
	//	// 通知发送错误
	//	GlobalNetworkMsgDispacher::getMsgDispacher()->addPakage(new GlobalNetPackage(Type_Send_Err,NULL,NULL));
	//	delete msg;
	//	return;
	//}
	requestMsgQueue.push(msg);
}
//
int NetWorkThreadProc::getWaitedTaskNum()
{
	MessageAutoLock lock;
	int num = requestMsgQueue.size();
	return num;
}

NetWorkThreadProc::~NetWorkThreadProc(void)
{
}
