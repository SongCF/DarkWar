#include "ShouHuZhiLingManager.h"
#include "TaskManager.h"
#include "MyselfManager.h"
#include "ServerTime.h"

ShouHuZhiLingManager* ShouHuZhiLingManager::mInstance = NULL;

ShouHuZhiLingManager::ShouHuZhiLingManager(void)
{
	{
		SHZHL season;
		season.seasonId = 1;
		season.minRquireLevel = 1;
		season.maxRquireLevel = 15;
		shouhuzhilingDatas.push_back(season);
	}
	{
		SHZHL season;
		season.seasonId = 2;
		season.minRquireLevel = 16;
		season.maxRquireLevel = 20;
		shouhuzhilingDatas.push_back(season);
	}
	{
		SHZHL season;
		season.seasonId = 3;
		season.minRquireLevel = 21;
		season.maxRquireLevel = 25;
		shouhuzhilingDatas.push_back(season);
	}
	{
		SHZHL season;
		season.seasonId = 4;
		season.minRquireLevel = 26;
		season.maxRquireLevel = 30;
		shouhuzhilingDatas.push_back(season);
	}
	
	CCAssert(shouhuzhilingDatas.size() == TaskManager::getManager()->getSeasonNum(),"");
}

ShouHuZhiLingManager* ShouHuZhiLingManager::getManager()
{
	if (mInstance == NULL)
	{
		mInstance = new ShouHuZhiLingManager();
	}
	return mInstance;
}

void ShouHuZhiLingManager::setRewards(ItemInfo reward)
{
	rewards = reward;
}

ItemInfo ShouHuZhiLingManager::getCurrentReword()
{
	return rewards;
}

void ShouHuZhiLingManager::clearReward()
{
	rewards.type = 0;
	rewards.count = 0;
}
void ShouHuZhiLingManager::updateSHZHL(int seasonId,CurrrentDefendHero curentHero)
{
	CCAssert(seasonId >= 1 && seasonId <= TaskManager::getManager()->getSeasonNum(),"");
	shouhuzhilingDatas[seasonId-1].currentDefendHero = curentHero;
}
void ShouHuZhiLingManager::updateSHZHL(int seasonId,HeroInfo firstHero)
{
	CCAssert(seasonId >= 1 && seasonId <= TaskManager::getManager()->getSeasonNum(),"");
	shouhuzhilingDatas[seasonId-1].firstDefendHero = firstHero;
}
void ShouHuZhiLingManager::setWorshiped(int seasonId,bool worshiped)
{
	CCAssert(seasonId >= 1 && seasonId <= TaskManager::getManager()->getSeasonNum(),"");
	shouhuzhilingDatas[seasonId-1].isWorshiped = worshiped;
}

void ShouHuZhiLingManager::setAmILoserInSeason(int seasonId,bool lose)
{
	CCAssert(seasonId >= 1 && seasonId <= TaskManager::getManager()->getSeasonNum(),"");
	shouhuzhilingDatas[seasonId-1].am_i_loser = lose;
}


// 获取守护之灵的完整数据
// 完整的包括了所有章节的数据
// 并且保证重第一章到最后一章顺序排列
vector<SHZHL> ShouHuZhiLingManager::getSHZHLData()
{
	// 简单校验下
	CCAssert(shouhuzhilingDatas.size() == TaskManager::getManager()->getSeasonNum(),"");
	return shouhuzhilingDatas;
}

SHZHL ShouHuZhiLingManager::getSHZHLData(int seasonId)
{
	// 简单校验下
	CCAssert(seasonId>=1 && seasonId <= TaskManager::getManager()->getSeasonNum(),"");
	CCAssert(shouhuzhilingDatas.size() == TaskManager::getManager()->getSeasonNum(),"");
	return shouhuzhilingDatas[seasonId-1];
}


enum ErrorCode
{
	EC_OK = 0,					// 没错，没问题
	EC_LEVEL_NOT_MATCH,			// 等级不匹配
	EC_LEVEL_DEFENDER_IS_SAFE,	// 守护者正处于安全期
	EC_CAN_NOT_WORSHIP_SELF,	// 不能膜拜自己
};

ShouHuZhiLingManager::ErrorCode ShouHuZhiLingManager::canIChlgSHZL(int seasonId)
{
	BaseActor* hero = MyselfManager::getManager()->getMyZhuJueData();
	for (int i=0,count = shouhuzhilingDatas.size();i<count;i++)
	{
		if (shouhuzhilingDatas[i].seasonId == seasonId)
		{
			if (hero->getLevel() < shouhuzhilingDatas[i].minRquireLevel || hero->getLevel() > shouhuzhilingDatas[i].maxRquireLevel)
			{
				return EC_LEVEL_NOT_MATCH;
			}


			if (shouhuzhilingDatas[i].currentDefendHero.heroId == hero->getId())
			{
				return EC_CAN_NOT_CHLG_SELF;
			}

			if (shouhuzhilingDatas[i].am_i_loser)
			{
				return EC_CANNOT_CHLG_AGAIN_AFTER_LOSE;
			}

			if (isCurrentDefenderSafe(seasonId))
			{
				return EC_DEFENDER_IS_SAFE;
			}
			return EC_OK;
		}
	}

	CCAssert(false,"异常");
	return EC_ERROR;
}
ShouHuZhiLingManager::ErrorCode ShouHuZhiLingManager::canIWorshipSHZL(int seasonId)
{
	BaseActor* hero = MyselfManager::getManager()->getMyZhuJueData();
	for (int i=0,count = shouhuzhilingDatas.size();i<count;i++)
	{
		if (shouhuzhilingDatas[i].seasonId == seasonId)
		{
			if (hero->getLevel() < shouhuzhilingDatas[i].minRquireLevel || hero->getLevel() > shouhuzhilingDatas[i].maxRquireLevel)
			{
				return EC_LEVEL_NOT_MATCH;
			}

			if (shouhuzhilingDatas[i].currentDefendHero.heroId == hero->getId())
			{
				return EC_CAN_NOT_WORSHIP_SELF;
			}
			if (shouhuzhilingDatas[i].isWorshiped)
			{
				return EC_ALREADY_WORSHIPED;
			}

			if (shouhuzhilingDatas[i].currentDefendHero.heroId != INVALID_ID && !shouhuzhilingDatas[i].currentDefendHero.isHero())
			{
				return EC_CAN_NOT_WORSHIP_GW;
			}

			if (shouhuzhilingDatas[i].currentDefendHero.heroId == INVALID_ID)
			{
				CCAssert(false,"不可能，currentDefendHero必须要有人");
				return EC_ERROR;
			}

			return EC_OK;
		}
	}

	CCAssert(false,"异常");
	return EC_ERROR;
}

ShouHuZhiLingManager::ErrorCode ShouHuZhiLingManager::canIGetReward(int seasonId)
{
	if (!amIDefendAtSeason(seasonId))
	{
		return EC_CAN_NOT_GET_REWARD_NOT_THIS_SEASON;
	}

	if (rewards.type == 0 || rewards.count == 0)
	{
		return EC_NO_REWARD;
	}

	return EC_OK;
}


unsigned int ShouHuZhiLingManager::getCurrentDefenderDay(int seasonId)
{
	SHZHL data = getSHZHLData(seasonId);
	if (data.currentDefendHero.heroId == INVALID_ID)
	{
		CCAssert(false,"异常");
		return 0;
	}

	unsigned int curentTime = ServerTime::getTime();
	if (curentTime < data.currentDefendHero.defendTime)
	{
		CCAssert(false,"异常");
		return 0;
	}

#define SystemDailyTask_Hour 4
/*
	//tm *p_defend = localtime(((time_t*)&data.currentDefendHero.defendTime));
	//tm *p_now = localtime(((time_t*)&curentTime));


	tm t_defend;
	tm *p_defend = &t_defend;
	memcpy(p_defend,ServerTime::getTime_tm(data.currentDefendHero.defendTime), sizeof(tm));
	tm *p_now = ServerTime::getTime_tm(curentTime);

	int days;
	if(p_defend->tm_hour+8 < SystemDailyTask_Hour && p_now->tm_hour+8 >= SystemDailyTask_Hour) {
		days = p_now->tm_mday - p_defend->tm_mday + 1;
	}
	else if(p_defend->tm_hour+8 >= SystemDailyTask_Hour && p_now->tm_hour+8 < SystemDailyTask_Hour) {
		days = p_now->tm_mday - p_defend->tm_mday - 1;
	}
	else {
		days = p_now->tm_mday - p_defend->tm_mday;
	}

	return days;
	//return (curentTime - data.currentDefendHero.defendTime)/(24*60*60);
*/

	unsigned int last_time = data.currentDefendHero.defendTime;
	unsigned int cur_time = ServerTime::getTime();
	if(last_time > cur_time)
		return -1;

	tm t_old;
	tm *p_old = &t_old;
	memcpy(p_old, ServerTime::getTime_tm(last_time), sizeof(tm));

	tm *p_now = ServerTime::getTime_tm(cur_time);
	//int days = p_now->tm_yday - p_old->tm_yday ;
	int days = ((unsigned int)cur_time - last_time) / (24 *3600);

	if(p_old->tm_hour > p_now->tm_hour) //过去时间的小时数比当前时间小时数大 时间加1天
		days ++;

	if(p_old->tm_hour < SystemDailyTask_Hour && p_now->tm_hour >= SystemDailyTask_Hour) {
		//过去时间小时数比系统刷新时间点小 而现在时间小时数比系统刷新时间点大 
		//则在计算的天数上加1天
		days ++;
	}
	else if(p_old->tm_hour >= SystemDailyTask_Hour && p_now->tm_hour < SystemDailyTask_Hour) {
		//过去时间小时数比系统刷新时间点大 而现在时间小时数比系统刷新时间点小 
		//则在计算的天数上减1天
		days --;
	}

	return days;

}

bool ShouHuZhiLingManager::isCurrentDefenderSafe(int seasonId)
{
	return false; // 不在有安全期的概念了

	return getCurrentDefenderDay(seasonId) == 0; // 一天24小时之内安全
}

bool ShouHuZhiLingManager::canGetReward(int seasonId)
{
	if (amIDefendAtSeason(seasonId) && rewards.type > 0 && rewards.count > 0)
	{
		return true;
	}
	return false;
}
bool ShouHuZhiLingManager::amIDefendAtSeason(int seasonId)
{
	SHZHL data = getSHZHLData(seasonId);
	return data.currentDefendHero.heroId == MyselfManager::getManager()->getMyZhuJueData()->getId();
}

ShouHuZhiLingManager::~ShouHuZhiLingManager(void)
{
}
