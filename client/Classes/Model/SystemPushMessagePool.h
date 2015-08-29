#pragma once

#include "protocol.h"
#include <map>
#include <vector>

using std::vector;
using std::map;

//系统推送的跑马灯消息


class SystemPushMessagePool
{
private:
	SystemPushMessagePool(){}
	map<unsigned int, MarqueeMessage> _msgQuene;
	static SystemPushMessagePool* _sysPushMsgPool;
public:
	static SystemPushMessagePool* getPool();
	void addMessage(vector<MarqueeMessage>& vec);
	void addMessage(MarqueeMessage& msg);

	//从消息队列里面取一下消息出来
	//取出来后数据层就会把它删除。
	bool getOneMessage(MarqueeMessage& _out);
	int getCurMsgCount();
};