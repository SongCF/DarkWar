#pragma once
#include <vector>
#include <string>
#include "cocos2d.h"
using namespace std;
using namespace cocos2d;

#define MSG_GLOBAL_TICK		"__MSG_GLOBAL_TICK__"

enum TrigelType
{
	Trigle_Delay = 0,	// 延迟多久之后通知
	Trigle_AtTime,		// 到某个服务器时间时通知
};

struct OneTrigle
{
	TrigelType type;
	// 对于定点通知，这个loop没用
	bool loop;
	// 时间，秒数
	// 通知的间隔时间或者是定点通知的时间
	unsigned int time;
	// 发送通知的消息
	string notifyMsg;

	/**
	 * 见makeTrigleAtTime和makeTrigleDelay快速创建的方法
	 */
	OneTrigle()
	{
		loop = false;
		type = Trigle_Delay;
		time = 0;
		//memset(this,0,sizeof(OneTrigle));
	}

	/**
	 * 构建一个Trigle配置
	 * 用于定时触发。比如在2014-06-09号通知，那么就用这个接口
	 * time是绝对时间
	 */
	static OneTrigle makeTrigleAtTime(string notifyMsg,unsigned int time)
	{
		OneTrigle ret;
		ret.type = Trigle_AtTime;
		ret.loop = false;
		ret.notifyMsg = notifyMsg;
		ret.time = time;
		return ret;
	}

	/**
	 * 延迟通知，也就是在当前时间之后time秒通知
	 * 如果想每一帧都通知，那么time填0，loop填true即可
	 * 如果想循环通知，就loop为true即可
	 */
	static OneTrigle makeTrigleDelay(string notifyMsg,unsigned int time,bool loop)
	{
		OneTrigle ret;
		ret.type = Trigle_Delay;
		ret.loop = loop;
		ret.notifyMsg = notifyMsg;
		ret.time = time;
		return ret;
	}
};

struct TrigleItem
{
	unsigned itemId;	// 自动生成的Id
	OneTrigle trigle;
	// 距离上次的通知，已经过了多少秒数了
	float timeFromLastNotify;
	TrigleItem()
	{
		itemId = 0;
		timeFromLastNotify = 0;
		//memset(this,0,sizeof(TrigleItem));
	}
};

// 通知传送的数据
class TickObjectData : public CCObject
{
public:
	// 据上次的触发，时间过去了多少秒
	float timePastFromLast; 
};

// 全局的一个tick触发器
// 每一个tick他都会触发一次消息发送
class GlobalTickTrigle : public CCNode
{
protected:
	static GlobalTickTrigle* mTrigle;
	unsigned int mId;
	vector<TrigleItem> mTrigles;
protected:
	void update(float delta);
	void postNotification(const char* msg,float time);
private:
	GlobalTickTrigle(void);
public:
	static GlobalTickTrigle* getTrigle();

	// 添加一个触发器
	// 返回该触发器的Id
	// 请妥善保管这个Id，以后移除都靠这个Id,如果是一次性的，其实这个Id就不用保存了。
	// 
	// 触发的时候会发送一个TickObjectData包体的通知
	// 所以要想接受通知，需要监听通知消息：CCNotificationCenter::sharedNotificationCenter()->addObserver()
	// 常用写法：
	// addOneTrigle(OneTrigle::makeTrigleAtTime("",1365454644));
	// addOneTrigle(OneTrigle::makeTrigleDelay("",1));
	unsigned int addOneTrigle(OneTrigle trigle);
	// 添加一个触发器，并且让target监听这个trigle的消息
	// 见removeTrigleAndRemoveListen
	unsigned int addOneTrigleAndListen(OneTrigle trigle,CCObject *target,SEL_CallFuncO selector);

	bool getTrigleById(unsigned int trigleId,OneTrigle& data);

	// 移除一个触发器
	// 推荐使用removeTrigle方法
	// 这样预防bug
	void removeTrigleWithoutClearId(unsigned int trigleId);
	// 移除，并置trigleId为0
	void removeTrigle(unsigned int& trigleId);
	// 移除并置trigleId为0，然后移除target监听的这个trigle的消息
	// 见addOneTrigleAndListen
	void removeTrigleAndRemoveListen(unsigned int& trigleId,CCObject *target);

	~GlobalTickTrigle(void);
};

