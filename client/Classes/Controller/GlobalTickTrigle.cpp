#include "GlobalTickTrigle.h"
#include "ServerTime.h"
#include <map>

GlobalTickTrigle* GlobalTickTrigle::mTrigle = NULL;

GlobalTickTrigle::GlobalTickTrigle(void)
{
	init();
	this->mId = 0;
	//只要调用这两个方法节点就可以处于运行状态。
	this->onEnter();
	this->onEnterTransitionDidFinish();
	this->scheduleUpdate();
}

GlobalTickTrigle* GlobalTickTrigle::getTrigle()
{
	if (mTrigle == NULL)
	{
		mTrigle = new GlobalTickTrigle();
	}
	return mTrigle;
}

// 添加一个触发器
// 返回该触发器的Id
unsigned int GlobalTickTrigle::addOneTrigle(OneTrigle trigle)
{
	// 检验
	{
		if (trigle.type == Trigle_AtTime && trigle.loop == true)
		{
			CCLog("Waring : %s --> trigle.type == Trigle_AtTime,but loop is true,we will ignore loop");
			trigle.loop = false;
		}

		map<string,bool> temp;
		for (int i=0,count=mTrigles.size(); i<count; i++)
		{
			if (temp.find(mTrigles[i].trigle.notifyMsg) != temp.end())
			{
				CCAssert(false,"已经有一个notifyMsg相同的存在了，接受通知时会有bug，不推荐这么做！");
				continue;
			}
			temp.insert(pair<string,bool>(mTrigles[i].trigle.notifyMsg,true));
		}
	}

	mId ++;

	TrigleItem item;
	item.trigle = trigle;
	item.itemId = mId;
	item.timeFromLastNotify = 0;

	mTrigles.push_back(item);

	return item.itemId;
}

unsigned int GlobalTickTrigle::addOneTrigleAndListen(OneTrigle trigle,CCObject *target,SEL_CallFuncO selector)
{
	CCNotificationCenter::sharedNotificationCenter()->addObserver(target,selector,trigle.notifyMsg.c_str(),NULL);
	return addOneTrigle(trigle);
}

bool GlobalTickTrigle::getTrigleById(unsigned int trigleId,OneTrigle& data)
{
	bool found = false;
	vector<TrigleItem>::iterator it = mTrigles.begin();
	for (;it != mTrigles.end(); it++)
	{
		if (it->itemId == trigleId)
		{
			data = it->trigle;
			found = true;
			break;
		}
	}
	return found;
}

// 移除一个触发器
void GlobalTickTrigle::removeTrigleWithoutClearId(unsigned int trigleId)
{
	vector<TrigleItem>::iterator it = mTrigles.begin();
	for (;it != mTrigles.end(); it++)
	{
		if (it->itemId == trigleId)
		{
			mTrigles.erase(it);
			break;
		}
	}
}

void GlobalTickTrigle::removeTrigle(unsigned int& trigleId)
{
	removeTrigleWithoutClearId(trigleId);
	trigleId = 0;
}

void GlobalTickTrigle::removeTrigleAndRemoveListen(unsigned int& trigleId,CCObject *target)
{
	OneTrigle temp;
	if (getTrigleById(trigleId,temp))
	{
		CCNotificationCenter::sharedNotificationCenter()->removeObserver(target,temp.notifyMsg.c_str());
	}
	removeTrigle(trigleId);
}

void GlobalTickTrigle::postNotification(const char* msg,float time)
{
	TickObjectData obj;
	obj.timePastFromLast = time;
	CCNotificationCenter::sharedNotificationCenter()->postNotification(msg,&obj);
}

void GlobalTickTrigle::update(float delta)
{
	const unsigned int currentTime = ServerTime::getTime();

	// 1.更新通知时间，找出需要通知的对象,移除不需要的对象
	// 不在这里通知，因为通知会触发mTrigles的修改，会崩溃
	vector<TrigleItem> needNotify;
	{
		vector<TrigleItem>::iterator it;
		for (it = mTrigles.begin(); it!= mTrigles.end();)
		{
			it->timeFromLastNotify += delta; // 更新时间

			switch (it->trigle.type)
			{
			case Trigle_Delay:
				// 间隔时间到了，通知.如果不loop，就移除
				if (it->timeFromLastNotify >= it->trigle.time)
				{
					needNotify.push_back((*it));

					it->timeFromLastNotify = 0; // 清空时间
					if (!it->trigle.loop)
					{
						it = mTrigles.erase(it); // 移除
						continue;
					}
				}

				break;
			case Trigle_AtTime:
				// 时间到了就通知，并移除
				if (it->trigle.time >= currentTime)
				{
					needNotify.push_back((*it));
					it->timeFromLastNotify = 0; // 清空,这里可以不需要
					it = mTrigles.erase(it); // 移除
					continue;
				}
				break;
			default:
				CCAssert(false,"");
				break;
			}

			 it++;
		}
	}
	

	postNotification(MSG_GLOBAL_TICK,delta);
	// 2.通知，通知期间可能有新的Item加入，有Item移除
	// 当然这里不用管，因为操作的数据都是mTrigles，而不是needNotify
	for (int i=0,count=needNotify.size(); i<count; i++)
	{
		postNotification(needNotify[i].trigle.notifyMsg.c_str(),needNotify[i].timeFromLastNotify);
	}
}
GlobalTickTrigle::~GlobalTickTrigle(void)
{
}
