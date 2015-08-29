#pragma once

#include "protocol.h"

// 物品
class WuPin
{
	ItemsType type;
	int count;
public:
	WuPin(ItemsType type,int count)
	{
		if (count < 0)
			CCAssert(false,"");

		this->type = type;
		this->count = count;
	}

	void setCount(int count)
	{
		this->count = count;
		if (this->count < 0)
		{
			this->count = 0;
			CCLog("Waring : WuPin[type = %d], count is < 0",type);
			CCAssert(false,"异常");
		}
	};
	ItemsType getType(){return type;};
	int getCount(){return count;};
};

// 
class WuPinManager
{
	vector<WuPin*> mWuPinPool;

	static WuPinManager* mManager;

	WuPinManager(void);

public:

	static WuPinManager* getManager();

	// 初始化
	void init(vector<ItemInfo> inforVec);
	// 给某种物品添加多少个
	// 如果是负值就减去，
	// 已经存在就更新，没有存在就添加
	// 返回刚才添加的
	WuPin* addWuPin(ItemsType type,int count);
	WuPin* setWuPinNum(ItemsType type,int count);
	// 获得某类型的物品
	// 肯定返回一个非NULL指针，如果没有，就是数量为0
	WuPin* getWuPin(ItemsType type);

	//获取所有宝石
	std::vector<ItemInfo> getAllBaoshi();

	//获取所有道具
	std::vector<ItemInfo> getAllItems();

	void clear();
	~WuPinManager(void);
};

