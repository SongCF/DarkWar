#pragma once
#include <stdlib.h>
#include "protocol.h"
#include <vector>

using namespace std;


struct CurrrentDefendHero
{
	unsigned int heroId;	// 如果是怪物就是怪物Id
	unsigned int level;
	string name;
	unsigned int profession; // 职业大于10就是怪物，见isHero函数

	unsigned int defendTime;	// 守护的开始时间，比如2014-04-01开始守护

	CurrrentDefendHero()
	{
		heroId = 0;
		level = 0;
		profession = 0;
		defendTime = 0;
		name = "";
	}

	// 是英雄就返回true
	bool isHero()
	{
		return (profession >= Profession_Yemanren && profession <= Profession_WuShi);
	}

	//bool can
};

struct SHZHL
{
	unsigned int seasonId;		// 章节的Id:1~4

	unsigned int minRquireLevel;
	unsigned int maxRquireLevel;

	HeroInfo firstDefendHero;	// 初始守护的角色:没有就是0

	bool isWorshiped;	// 是否已经膜拜了
	CurrrentDefendHero currentDefendHero; // 总是会有的

	bool am_i_loser;	// 我是否刚24小时之类输掉了

	SHZHL()
	{
		seasonId = 0;
		minRquireLevel = 0;
		maxRquireLevel = 0;
		memset(&(this->firstDefendHero),0,sizeof(HeroInfo));
	}
};

class ShouHuZhiLingManager
{
private:
	// 完整的包括了所有章节的数据
	// 并且保证重第一章到最后一章顺序排列
	vector<SHZHL> shouhuzhilingDatas;
	// 奖励
	ItemInfo rewards;

	static ShouHuZhiLingManager* mInstance;
	ShouHuZhiLingManager(void);

	// 获取守护之灵的完整数据
	// 完整的包括了所有章节的数据
	// 并且保证重第一章到最后一章顺序排列
	vector<SHZHL> getSHZHLData();

public:
	static ShouHuZhiLingManager* getManager();

	void setRewards(ItemInfo reward);
	// 没有返回的就是空的type：0，count:0
	ItemInfo getCurrentReword();
	void clearReward();
	void updateSHZHL(int seasonId,CurrrentDefendHero curentHero);
	void updateSHZHL(int seasonId,HeroInfo firstHero);
	//
	void setWorshiped(int seasonId,bool worshiped);
	void setAmILoserInSeason(int seasonId,bool lose);

	// 获取守护之灵的数据
	// 每次UI获取的时候，都需要先请求网络的最新的数据
	// seasonId 1~4
	SHZHL getSHZHLData(int seasonId);

	enum ErrorCode
	{
		EC_OK = 0,								// 没错，没问题
		EC_ERROR,								// 有问题
		EC_LEVEL_NOT_MATCH,						// 等级不匹配
		EC_DEFENDER_IS_SAFE,					// 守护者正处于安全期
		EC_CANNOT_CHLG_AGAIN_AFTER_LOSE,		// 刚输了，不能再挑战
		EC_CAN_NOT_WORSHIP_SELF,				// 不能膜拜自己
		EC_CAN_NOT_WORSHIP_GW,					// 不能膜拜怪物
		EC_ALREADY_WORSHIPED,					// 不能膜拜,已经膜拜了
		EC_CAN_NOT_CHLG_SELF,					// 不能挑战自己
		EC_CAN_NOT_GET_REWARD_NOT_THIS_SEASON,	// 守护的不是这章
		EC_NO_REWARD,							// 没有奖励，或者已经领过了
	};

	// 是否可以挑战某个章节的守护之灵，可以就返回EC_OK
	// seasonId：1~4
	ErrorCode canIChlgSHZL(int seasonId);
	// 是否可以膜拜，可以就返回EC_OK
	// seasonId：1~4
	ErrorCode canIWorshipSHZL(int seasonId);
	// 
	ErrorCode canIGetReward(int seasonId);


	// 获取当前守护者已经守护的天数
	// 刚开始是0,24小时之后就是1，依次递增
	unsigned int getCurrentDefenderDay(int seasonId);
	// 当前守护者是否安全期
	bool isCurrentDefenderSafe(int seasonId);
	// 是否可以领取奖励了
	bool canGetReward(int seasonId);
	// 我是否守在某个season的
	// 应该这个接口没多大用
	bool amIDefendAtSeason(int seasonId);

	~ShouHuZhiLingManager(void);
};

