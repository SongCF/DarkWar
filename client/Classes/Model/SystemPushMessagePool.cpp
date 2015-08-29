#include "SystemPushMessagePool.h"

#define MsgQueneMax 10

SystemPushMessagePool* SystemPushMessagePool::_sysPushMsgPool = 0;

SystemPushMessagePool* SystemPushMessagePool::getPool()
{
	if (! _sysPushMsgPool) _sysPushMsgPool = new SystemPushMessagePool;
	return _sysPushMsgPool;
}



void SystemPushMessagePool::addMessage(vector<MarqueeMessage>& vec)
{
	for (int i=0,count=vec.size(); i<count && _msgQuene.size() < MsgQueneMax; ++i)
	{
		_msgQuene[vec.at(i).uid] = vec.at(i);
	}
}
void SystemPushMessagePool::addMessage(MarqueeMessage& msg)
{
	if(_msgQuene.size() < MsgQueneMax)
		_msgQuene[msg.uid] = msg;
}
bool SystemPushMessagePool::getOneMessage(MarqueeMessage& _out)
{
	if (_msgQuene.size() > 0)
	{
		_out = _msgQuene.begin()->second;
		_msgQuene.erase(_msgQuene.begin());
		return true;
	}else {
		return false;
	}
}

int SystemPushMessagePool::getCurMsgCount()
{
	return _msgQuene.size();
}