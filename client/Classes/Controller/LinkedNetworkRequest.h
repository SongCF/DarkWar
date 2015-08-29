#pragma once
#include "cocos2d.h"
#include <string>
#include "LFMessageBox.h"

using namespace std;
using namespace cocos2d;

class IRequest
{
public:
	// 获取总共需要请求的个数
	virtual int getRequestNum() = 0;
	// 开始发第index个请求
	virtual void request(int index) = 0;

	// 获取当前需要监听的消息MSG_****
	virtual string getMessgaeToListening(int index) = 0;

	// 收到消息
	// msg_data : 就是MSG_Data或者是flat的，这个随具体的命令
	// index	: 当前消息是属于第几个的
	// 返回true就代表正常，可以继续，false就代表错误，终端
	virtual bool receivedData(CCObject* msg_data,int index) = 0;

	// 通知超时了
	virtual void timeout(){};

	// 全部完毕，结束了
	virtual void end() = 0;
};
// 一个串接的网络请求，会发一系列的请求，对于失败的请求，必须是按照i开始一个一个的向下获取
// 用完了这个类就没用了，可以在end中释放掉了
class LinkedRequest : public CCObject
{
protected:
	IRequest* mRequest;
	int mStartIndex;
	vector<string> mMsgVec;
	bool linkedEnd;

	void cmdRsp(CCObject* msg_rsp)
	{
		// 移除监听
		removeObserver(mRequest->getMessgaeToListening(mStartIndex));

		bool suc = mRequest->receivedData(msg_rsp,mStartIndex);
		if (suc)
		{
			mStartIndex ++; // 成功一个
		}

		// 这是最后一个了
		if (mStartIndex == mRequest->getRequestNum())
		{
			linkedEnd = true;
			CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(LinkedRequest::timeout),this);
			mRequest->end();
		}
	}

	void addObserver(string msg)
	{
		CCNotificationCenter::sharedNotificationCenter()->addObserver(
			this,SEL_CallFuncO(&LinkedRequest::cmdRsp),msg.c_str(),NULL);
		mMsgVec.push_back(msg);
	}
	void removeObserver(string msg)
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,msg.c_str());
		for (vector<string>::iterator it = mMsgVec.begin();it != mMsgVec.end();)
		{
			if ( (*it) == msg)
			{
				it = mMsgVec.erase(it);
				continue;
			}
			it++;
		}
	}
	void removeAllObserver()
	{
		int num = mMsgVec.size();
		for (int i=0;i<num;i++)
		{
			CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,mMsgVec[i].c_str());
		}
		mMsgVec.clear();
	}

	void timeout(float time)
	{
		if (linkedEnd)
			return;
		mRequest->timeout();
	}
public:
	LinkedRequest(IRequest* request)
	{
		mRequest = request;
		mStartIndex = 0;
		linkedEnd = true;
	}
	~LinkedRequest()
	{
		removeAllObserver();
	}
	// 开始,从上次失败的那一个开始
	// 如果第一次调用，那么就是从第0个开始
	// 途中可能会有失败，那么成功计数就会停止
	void begin(float timeOut = -1)
	{
		if (timeOut > 0)
			CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(LinkedRequest::timeout),this,timeOut,0,0,false);
		
		int num = mRequest->getRequestNum();
		CCAssert(mStartIndex < num,"已经请求完毕了，不在需要调用了");
		for (int i=mStartIndex;i<num;i++)
		{
			linkedEnd = false;
			// 监听
			addObserver(mRequest->getMessgaeToListening(i));
			mRequest->request(i);
		}
	}
};
