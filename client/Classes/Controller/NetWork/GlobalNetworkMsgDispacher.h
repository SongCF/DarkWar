#pragma once

#include "cocos2d.h"
#include <queue>

using namespace std;
using namespace cocos2d;


//class Mutex
//{
//public:
//	Mutex(void);
//	~Mutex(void);
//	void Lock();
//	void UnLock();
//private:
//	CRITICAL_SECTION m_criticalSection;
//};

enum PackageType
{
	Type_Conncet_Suc = 0,
	Type_Conncet_Err,
	Type_Send_Err,
	Type_Recv_Err,
	Type_RecvData,		// 只有这种情况NetPackage才有数据
};
// 网络回调的消息包，收到的数据包
class GlobalNetPackage
{
private:
	PackageType type;
	char* headArray;
	char* bodyArray;
public:
	// 收到的数据包
	GlobalNetPackage(PackageType type,char* head,char* body)
	{
		headArray = head;
		bodyArray = body;
		this->type = type;
	}
	~GlobalNetPackage()
	{
		if(headArray)
			delete [] headArray;
		if(bodyArray)
			delete [] bodyArray;
	}
	PackageType getType(){return type;};
	char* getHead(){return headArray;};
	char* getBody(){return bodyArray;};
};

class AutoLuck_Unlock
{
public:
	AutoLuck_Unlock();
	~AutoLuck_Unlock();
};

// 全局的网络数据派发器
class GlobalNetworkMsgDispacher : public CCNode
{
private:
	queue<GlobalNetPackage*> msgQueue;
	static GlobalNetworkMsgDispacher* netDispacher;

	GlobalNetworkMsgDispacher(void);
protected:
	virtual void update(float delta);
public:
	static GlobalNetworkMsgDispacher* getMsgDispacher();
	// 会自己delete
	void addPakage(GlobalNetPackage* package);
	~GlobalNetworkMsgDispacher(void);
};

