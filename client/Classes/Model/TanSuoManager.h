#pragma once
#include <stdlib.h>
#include <string>

struct TanSuoData
{
	unsigned int currentTimeCanUse; // 当前的探索点数
	unsigned int defaultTimeCanUse;	// 总数的点数
	unsigned int destoriedNum;		// 破坏，击开石头的个数

	unsigned int unlocked_seal;		//此轮已解封装备数, 每轮点亮6个石碑后解封重新刷新为0
	unsigned int last_modify_timestamp;	//最后一次回复探索次数的时间戳 如果次数已经满了 则不管

	TanSuoData()
	{
		memset(this,0,sizeof(TanSuoData));
	}
};

class TanSuoManager
{
private:
	static TanSuoManager* mInstance;

	TanSuoData mTansuoData;

	TanSuoManager(void);
public:
	static TanSuoManager* getManager();

	//
	void setTanSuoData(TanSuoData data);
	// 
	void subCanTanSuoTime(unsigned int toSub);
	//
	void setCanTanSuoTime(unsigned int time);
	//
	void addDistoriedNum(unsigned int toAdd);
	//
	void setDistoriedNum(unsigned int num);
	//
	void addUnlockedSealNum(unsigned int num);
	//
	void setLastModifyTansuoTime(unsigned int time);
	

	// 获取当前还剩下的壳探索次数
	unsigned int getCurrentTimeLeft();
	// 获取探索总数
	unsigned int getTotalTanSuoTime();
	// 获取击碎，破坏的个数
	unsigned int getDistoriedNum();
	// 当前是否可以解封了，大于6个就可以了
	bool canJieFeng();

	//获取上一次刷新次数的时间
	unsigned int getLastRefreshTime();

	~TanSuoManager(void);
};

