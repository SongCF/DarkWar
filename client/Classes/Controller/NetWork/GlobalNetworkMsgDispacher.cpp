#include "GlobalNetworkMsgDispacher.h"
#include <pthread.h>
#include "CmdHelper.h"

static pthread_mutex_t    s_msgQueueMutex;

GlobalNetworkMsgDispacher* GlobalNetworkMsgDispacher::netDispacher = NULL;

AutoLuck_Unlock::AutoLuck_Unlock()
{
	pthread_mutex_lock(&s_msgQueueMutex);
	//CCLog("------pthread_mutex_lock------");
}
AutoLuck_Unlock::~AutoLuck_Unlock()
{
	pthread_mutex_unlock(&s_msgQueueMutex);
	//CCLog("------pthread_mutex_unlock------");
}


GlobalNetworkMsgDispacher::GlobalNetworkMsgDispacher(void)
{
	init();

	 pthread_mutex_init(&s_msgQueueMutex, NULL);
	//只要调用这两个方法节点就可以处于运行状态。
	this->onEnter();
	this->onEnterTransitionDidFinish();
	this->scheduleUpdate();
}

GlobalNetworkMsgDispacher* GlobalNetworkMsgDispacher::getMsgDispacher()
{
	if (netDispacher == NULL)
	{
		netDispacher = new GlobalNetworkMsgDispacher();
	}
	return netDispacher;
}

void GlobalNetworkMsgDispacher::addPakage(GlobalNetPackage* package)
{
	AutoLuck_Unlock lock;

	msgQueue.push(package);
}

void GlobalNetworkMsgDispacher::update(float delta)
{
	AutoLuck_Unlock lock;

	//CCLog("GlobalNetworkMsgDispacher::update");

	if (msgQueue.size() > 0)
	{
		GlobalNetPackage* package = msgQueue.front();
		msgQueue.pop();
		switch (package->getType())
		{
		case Type_Conncet_Suc:
			CmdHelper::getHelper()->connectServerSuccess();
			break;
		case Type_Conncet_Err:
			CmdHelper::getHelper()->connectServerError();
			break;
		case Type_Send_Err:
			CmdHelper::getHelper()->sendDataError();
			break;
		case Type_Recv_Err:
			CmdHelper::getHelper()->recvDataError();
			break;
		case Type_RecvData:
			CmdHelper::getHelper()->dealWithPackage(package->getHead(),package->getBody());
			break;
		default:
			break;
		}

		// 删除包
		delete package;
	}
}


GlobalNetworkMsgDispacher::~GlobalNetworkMsgDispacher(void)
{
}
