#pragma once

#include "protocol.h"
#include <map>
#include <vector>

using std::vector;
using std::map;

//ϵͳ���͵��������Ϣ


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

	//����Ϣ��������ȡһ����Ϣ����
	//ȡ���������ݲ�ͻ����ɾ����
	bool getOneMessage(MarqueeMessage& _out);
	int getCurMsgCount();
};