#include "DuanZaoManager.h"
#include "cocos2d.h"
#include "CS_Common.h"

using namespace cocos2d;

DuanZaoManager* DuanZaoManager::manager = new DuanZaoManager();

DuanZaoManager::DuanZaoManager(void)
{

}
DuanZaoManager* DuanZaoManager::getManager()
{
	return manager;
}
void DuanZaoManager::init(vector<DuanzaoGroup> group_arr)
{
	this->group_pool = group_arr;
	CCLOG("DuanZaoManager::init ---> ok");
}
// 获取可以锻造的列表
vector<DuanzaoGroup> DuanZaoManager::getDuanzaoGroupList()
{
	return group_pool;
}

bool DuanZaoManager::getDuanzaoGroup(unsigned int duanzao_group_id,DuanzaoGroup& data)
{
	int groupNum = group_pool.size();
	for (int i=0;i<groupNum;i++)
	{
		if (group_pool[i].duanzao_group_id == duanzao_group_id)
		{
			data = group_pool[i];
			return true;
		}
	}

	return false;
}

bool DuanZaoManager::getDuanzaoNeedGold(unsigned int duanzao_group_id,bool tenTimes,int& needCost)
{
	needCost = 0;
	DuanzaoGroup data;
	if (getDuanzaoGroup(duanzao_group_id,data))
	{
		needCost = data.gold_need;
		if (tenTimes)
		{
			needCost *= 10;
			needCost *= TEN_TIME_DISCOUNT;
		}

		return true;
	}

	return false;
}

bool DuanZaoManager::getDuanzaoNeedCaiLiao(unsigned int duanzao_group_id,bool tenTimes,int& item_type, int& needCost)
{
	needCost = 0;
	item_type = 0;

	DuanzaoGroup data;
	if (getDuanzaoGroup(duanzao_group_id,data))
	{
		if (data.item_id == INVALID_ID)
		{
			needCost = 0;
			return true;
		}

		item_type = data.item_id;
		needCost = data.item_count;

		if (tenTimes)
		{
			needCost *= 10;
			needCost *= TEN_TIME_DISCOUNT;
		}

		return true;
	}

	return false;
}

DuanZaoManager::~DuanZaoManager(void)
{

}