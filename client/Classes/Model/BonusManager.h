#pragma once
#include <stdlib.h>
#include <map>
#include <vector>

using namespace std;

#include "flat.h"
//struct Flat_BonusInfo;

// enum BonusType
// {
// 	BT_System = 0,//所有(不要使用这个值，即将删除)
// 
// 	BT_DailyLogin,
// 	BT_Level,
// 	BT_DayWelfare,
// 	BT_RewardTask,
// };


//每日登陆奖励、等级奖励、今日福利(日常任务)、悬赏任务(周常任务)
//
//

class BonusManager
{
	friend class CmdHelper;
private:
	static BonusManager* mInstance;

	// 所有的奖励
	map<int,vector<Flat_BonusInfo> > bonusPool;

	BonusManager(void);

	// 设置奖励
	void setBonus(BonusType _type,const vector<Flat_BonusInfo>& jiangli);
	void setBonus(const vector<Flat_BonusInfo>& vec);
	// 设置某个奖可以领取了 （系统推送）
	void setOneBonusEnable(BonusType type, unsigned int id);
	// 设置某个类型的某个奖励已经领取了
	void setIsGet(BonusType _type,unsigned int _id);

public:
	static BonusManager* getManager();

	//查看是否还有能领取的奖励
	bool haveNewBonus(BonusType type);

	//拿到所有的奖励（4种系统奖励）
	vector<Flat_BonusInfo> getAllBonus();

	// 获取奖励,里面包括领取的和还没领取的
	vector<Flat_BonusInfo> getBonus(BonusType _type);

	// 获取指定的Id的奖励,没找到就是返回false
	bool getBonus(BonusType _type,unsigned int _id,Flat_BonusInfo& outData);

	~BonusManager(void);
};

