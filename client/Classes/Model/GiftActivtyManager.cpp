#include "GiftActivtyManager.h"
#include "ServerTime.h"

GiftActivtyManager* GiftActivtyManager::mInstance = NULL;

GiftActivtyManager::GiftActivtyManager(void)
{
}

GiftActivtyManager* GiftActivtyManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new GiftActivtyManager();
	}
	return mInstance;
}


ActivityInfo* GiftActivtyManager::getActivityByType(int type)
{
	for (int i=0,count=activties.size(); i<count; i++)
	{
		if (type == activties[i].id)
		{
			return &activties[i];
		}
	}

	return NULL;
}
void GiftActivtyManager::initActivityInfos(vector<ActivityInfo> datas)
{
	activties = datas;
}

GiftActivtyManager::ActivtyFilter GiftActivtyManager::getActivityState(const ActivityInfo* infor)
{
	if (infor == NULL)
	{
		CCLog("Error : %s ---> infor == NULL",__FUNCTION__);
		return AF_Getted;
	}
	ActivtyFilter ret = AF_CanGet;
	unsigned int curTime = ServerTime::getTime();
	// 开区间
	if (!infor->is_get && curTime < infor->close_time && curTime > infor->open_time )
	{
		ret = AF_CanGet;
	}
	else if (!infor->is_get && curTime <= infor->open_time)
	{
		ret = AF_CanGetLater;
	}
	else if (!infor->is_get && curTime >= infor->close_time)
	{
		ret = AF_TimeOver;
	}
	else if(infor->is_get)
	{
		ret = AF_Getted;
	}
	else
	{
		CCAssert(false,"未知情况");
	}

	CCAssert(ret != AF_All,"不会返回AF_All这个值");
	return ret;
}

GiftActivtyManager::ActivtyFilter GiftActivtyManager::getActivityState(const ActivityInfo& infor)
{
	return getActivityState(&infor);
}

// 获取所有的奖励
vector<ActivityInfo> GiftActivtyManager::getActivities(ActivtyFilter filter /* = AF_All */)
{
	if (filter == AF_All)
	{
		return activties;
	}

	vector<ActivityInfo> ret;
	unsigned int curTime = ServerTime::getTime();

	for (int i=0,count=activties.size(); i<count; i++)
	{
		if (filter == getActivityState(activties[i]))
		{
			ret.push_back(activties[i]);
		}
	}

	return ret;
}
// 没找到就返回NULL
const ActivityInfo* GiftActivtyManager::getActivity(int type)
{
	return getActivityByType(type);
}

GiftActivtyManager::~GiftActivtyManager(void)
{
}
