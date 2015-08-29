#include "FightTrigleDriver.h"

#define FLOAT_ZERO	0.0000001f

FightTrigleDriver::FightTrigleDriver(void)
{
	mTrigleGapTime = 0;
	mPause = false;
	mExtraWait = 0;
	mCurrentTrigledTime = 0;
	mAutoSendTrigleMsg = true;
}

void FightTrigleDriver::onEnter()
{
	CCNode::onEnter();
}
void FightTrigleDriver::onExit()
{
	CCNode::onExit();
}
// 开始trigle
void FightTrigleDriver::trigle()
{
	unschedule(schedule_selector(FightTrigleDriver::tick));
	schedule(schedule_selector(FightTrigleDriver::tick));
}
void FightTrigleDriver::setAutoTrigleFalse()
{
	mAutoSendTrigleMsg = false;
}
void FightTrigleDriver::stop()
{
	unschedule(schedule_selector(FightTrigleDriver::tick));
}

void FightTrigleDriver::tick(float time)
{
	if (mPause)
	{
		return;
	}

	// 延时计算
	if (mExtraWait > FLOAT_ZERO)
	{
		mExtraWait -= time;

		if (mExtraWait > FLOAT_ZERO)
		{// 还在延时中...
			return;
		}
		else
		{
			postTriglePauseEnd();
		}
	}
	
	// 触发计时，到时就通知
	mCurrentTrigledTime += time;
	if (mCurrentTrigledTime >= mTrigleGapTime)
	{
		if (mAutoSendTrigleMsg)
		{
			postTrigleTime();
		}
		mCurrentTrigledTime = 0;
	}

}
void FightTrigleDriver::setTrigleGapTime(float t)
{
	mTrigleGapTime = t;
}

void FightTrigleDriver::pauseTrigleSomeTime(float time)
{
	mExtraWait += time;
}

float FightTrigleDriver::getCurrentPauseTimeLeft()
{
	return mExtraWait;
}
void FightTrigleDriver::clearPauseTime()
{
	mExtraWait = 0.0f;
}

void FightTrigleDriver::pauseTrigle()
{
	mPause = true;
}
void FightTrigleDriver::resumeTrigle(bool clearPause /*= true*/)
{
	mPause = false;

	if (clearPause)
	{
		clearPauseTime();
	}
}

// 发送触发消息
void FightTrigleDriver::postTrigleTime()
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_TRIGLE_TIME,NULL);
}

void FightTrigleDriver::postTriglePauseEnd()
{
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_TRIGLE_PAUSE_END,NULL);
}

FightTrigleDriver::~FightTrigleDriver(void)
{

}