#pragma once
#include "cocos2d.h"
#include "LFMessageBox.h"
#include "CmdHelper.h"
#include "LFStrings.h"

using namespace cocos2d;

// 会发送这个消息通知已经超时了
#define  MSG_WAIT_TIME_OUT	"WAIT_TIME_OUT"

// 最多等10秒钟
#define		MAX_TIME_WAIT	30.0f
// 等待服务器通知
#define		MAX_TIME_WAIT_SERVER_CALL	(MAX_TIME_WAIT*2.0f)

class TimeoutMonitor;
static TimeoutMonitor* mTimeMonitor = NULL;
class TimeoutMonitor : public CCNode
{
private:
	TimeoutMonitor()
	{
		this->init();
		this->onEnter();
		this->onEnterTransitionDidFinish();
	}
public:
	static TimeoutMonitor* getMonitor()
	{
		if (mTimeMonitor == NULL)
		{
			mTimeMonitor = new TimeoutMonitor();
		}
		return mTimeMonitor;
	}

	// 开始监视，超时之后自动断连接
	void startMonitor(float time_wait = MAX_TIME_WAIT)
	{
		CCLOG("%s:time_wait = %f",__FUNCTION__,time_wait);
		unschedule(schedule_selector(TimeoutMonitor::timeOutCallback));
		schedule(schedule_selector(TimeoutMonitor::timeOutCallback),time_wait);
	}

	void cancleMonitor()
	{
		CCLOG("%s",__FUNCTION__);
		unschedule(schedule_selector(TimeoutMonitor::timeOutCallback));
	}

	void timeOutCallback(float t)
	{
		CCLOG("TimeoutMonitor::---------------TimeOut-----------");
		unschedule(schedule_selector(TimeoutMonitor::timeOutCallback));
		
		LFMessageBox::alert(LFStrings::getValue("timeout_title"),LFStrings::getValue("wait_timeout"));
		CmdHelper::getHelper()->forceCloseNetwork();
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_WAIT_TIME_OUT,NULL);
	}
};
