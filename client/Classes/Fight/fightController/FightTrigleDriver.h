#pragma once
#include "cocos2d.h"
using namespace cocos2d;

// 触发的时间到了就发这个消息
#define MSG_TRIGLE_TIME			"MSG_TRIGLE_TIME" 
// 额外的延时，时间到了
#define MSG_TRIGLE_PAUSE_END	"MSG_TRIGLE_PAUSE_END"

// 战斗的触发驱动器
// 就想一个时钟，告知界面一步一步的跳动
class FightTrigleDriver : public CCNode
{
protected:
	float mTrigleGapTime;
	bool mPause;
	bool mAutoSendTrigleMsg;

	float mExtraWait;
	float mCurrentTrigledTime;

	virtual void onEnter();
	virtual void onExit();
private:
	void tick(float time);
public:
	FightTrigleDriver(void);
	// 设置触发的时延
	// 每过时间t就会发送一个消息
	void setTrigleGapTime(float t);

	// 自动发消息
	void setAutoTrigleFalse();

	// 开始trigle
	void trigle();
	// 停止
	void stop();

	// 暂停Trigle一会时间
	// 会累积的
	void pauseTrigleSomeTime(float time);
	// 清空延时的时间
	void clearPauseTime();

	float getCurrentPauseTimeLeft();

	// 暂时触发
	void pauseTrigle();
	// 恢复触发
	// 默认会清空pauseTrigleSomeTime设置的时间
	// 然后接着计时，触发。
	// 如果不清空，那么就会先把pause的时间完毕了之后才继续计时
	void resumeTrigle(bool clearPause = true);

	// 发送触发消息
	void postTrigleTime();
	// 延时完毕的消息
	void postTriglePauseEnd();

	~FightTrigleDriver(void);
};

