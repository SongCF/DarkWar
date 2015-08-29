#pragma once
#include <pthread.h>
#include "ODSocket.h"
#include <string>

using namespace std;

class RequestMessage;
extern RequestMessage* g_HeartBeatMsg;

// 自动锁
class MessageAutoLock
{
public:
	MessageAutoLock();
	~MessageAutoLock();
};

// 请求包
class RequestMessage
{
private:
	char* data;
	int length;
public:
	// 我会拷贝一份
	RequestMessage(char* head,int headLength,char* body,int bodyLength)
	{
		data = new char[headLength + bodyLength];
		length = bodyLength + headLength;
		// 拷贝一份
		memcpy(data,head,headLength);
		memcpy(data+headLength,body,bodyLength);
	}
	// 我会拷贝一份
	RequestMessage(RequestMessage* msg)
	{
		data = new char[msg->getDataLength()];
		length = msg->getDataLength();
		// 拷贝一份
		memcpy(data,msg->getData(),length);
	}
	~RequestMessage()
	{
		if(data)
			delete [] data;
	}

	char* getData(){return data;};
	int getDataLength(){return length;};

};

// 网络处理的代码
class NetWorkThreadProc
{
	friend class CmdHelper;
private:

	string serverIp;
	unsigned short serverPort;

	bool isRuning;

	void closeNetwork();
	NetWorkThreadProc(void);

	static NetWorkThreadProc* mProc;
public:
	static NetWorkThreadProc* getProc();

	// serverIp为NULL就用默认的
	// serverPort为负数也用默认的
	void initNetwork(const char* serverIp = NULL,unsigned short serverPort = 0);

	const char* getCurrentIp();
	unsigned short getCurrentPort();

	void startThread();

	// 会自己delete，发送完毕，失败等会自己delete的
	// 注意RequestMessage构造时会拷贝一份
	// 所以，你发送端的数据自己管理
	void sendData(RequestMessage* msg);
	// 获取等待的任务数
	int getWaitedTaskNum();
	~NetWorkThreadProc(void);
};

