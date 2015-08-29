#pragma once
#include "protocol.h"

class DuanZaoManager
{
private:
	static DuanZaoManager* manager;
	vector<DuanzaoGroup> group_pool;
	DuanZaoManager(void);
public:
	static DuanZaoManager* getManager();
	void init(vector<DuanzaoGroup> group_arr);
	// 获取可以锻造的列表
	vector<DuanzaoGroup> getDuanzaoGroupList();
	bool getDuanzaoGroup(unsigned int duanzao_group_id,DuanzaoGroup& data);
	bool getDuanzaoNeedGold(unsigned int duanzao_group_id,bool tenTimes,int& needCost);
	bool getDuanzaoNeedCaiLiao(unsigned int duanzao_group_id,bool tenTimes,int& item_type, int& needCost);
	~DuanZaoManager(void);
};

