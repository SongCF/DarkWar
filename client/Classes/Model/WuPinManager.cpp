#include "WuPinManager.h"

WuPinManager* WuPinManager::mManager = new WuPinManager();

WuPinManager::WuPinManager(void)
{

}

WuPinManager* WuPinManager::getManager()
{
	return mManager;
}

void WuPinManager::init(vector<ItemInfo> inforVec)
{
	clear();
	int num = inforVec.size();
	for (int i=0;i<num;i++)
	{
		WuPin* item = new WuPin((ItemsType)inforVec[i].type,inforVec[i].count);
		mWuPinPool.push_back(item);
	}

	{
#define MAX_ITEM_TYPE 1024
		// 检测项目的唯一性
		unsigned int apearedTime[MAX_ITEM_TYPE+1] = {0};
		int num = mWuPinPool.size();

		vector<WuPin*>::iterator it = mWuPinPool.begin();
		// 重复的
		vector<ItemInfo> tempRepeatVec;

		for (;it!=mWuPinPool.end();)
		{
			WuPin* temp = (*it);

			if (temp->getType() > MAX_ITEM_TYPE)
			{
				CCLog("Error : %s --> item type[=%d] is biger than [%d] , the array can not hold this value,must fix this!",
					__FUNCTION__,temp->getType(),MAX_ITEM_TYPE);
				CCAssert(false,"Error,type超出了数组的长度");

				it ++; // 下一个
				continue;
			}

			// 出现次数递加
			apearedTime[temp->getType()]++;

			// 第二次出现
			if (apearedTime[temp->getType()] >= 2)
			{
				CCLog("Waring : WuPinManager::init --> Item Repeated! [type = %d,count = %d]",temp->getType(),temp->getCount());
				// 这里直接报错，以后可以直接更新就是了
				CCAssert(false,"");
				
				// 保存数据
				ItemInfo infor;
				infor.count = temp->getCount();
				infor.type = temp->getType();
				tempRepeatVec.push_back(infor);

				// 移除
				it = mWuPinPool.erase(it);
				continue;
			}
			it++;
		}

		// 把重复的更新进去
		int repeatNum = tempRepeatVec.size();
		for (int i=0;i<repeatNum;i++)
		{
			addWuPin((ItemsType)tempRepeatVec[i].type,tempRepeatVec[i].count);
		}
	}

	CCLog("WuPinManager::init ---> over!");
}
WuPin* WuPinManager::addWuPin(ItemsType type,int count)
{
	int num = mWuPinPool.size();
	// 尝试更新
	for (int i=0;i<num;i++)
	{
		// 同种类型，直接修改数值即可
		if (mWuPinPool[i]->getType() == type)
		{
			int countBefore = mWuPinPool[i]->getCount();
			mWuPinPool[i]->setCount(countBefore + count);

			return mWuPinPool[i];
		}
	}
	// 直接添加
	WuPin* t = new WuPin(type,count);
	mWuPinPool.push_back(t);
	return t;
}

WuPin* WuPinManager::setWuPinNum(ItemsType type,int count)
{
	WuPin* wuping = getWuPin(type);
	wuping->setCount(count);
	return wuping;
}

WuPin* WuPinManager::getWuPin(ItemsType type)
{
	int num = mWuPinPool.size();

	for (int i=0;i<num;i++)
	{
		if (mWuPinPool[i]->getType() == type && mWuPinPool[i]->getCount() >= 0)
		{
			return mWuPinPool[i];
		}
	}

	// 没有 就加一个空的进去
	return addWuPin(type,0);
}

std::vector<ItemInfo> WuPinManager::getAllBaoshi()
{
	int num = mWuPinPool.size();

	std::vector<ItemInfo> vec;

	for (int i=0;i<num;i++)
	{
		if (mWuPinPool[i]->getType() > ItemsType_Baoshi_min && mWuPinPool[i]->getType() < ItemsType_Baoshi_max)
		{
			if(mWuPinPool[i]->getCount() > 0)
			{
				ItemInfo item;
				item.type = mWuPinPool[i]->getType();
				item.count = mWuPinPool[i]->getCount();
				vec.push_back(item);
			}
			
		}
	}

	return vec;
}

std::vector<ItemInfo> WuPinManager::getAllItems()
{
	int num = mWuPinPool.size();

	std::vector<ItemInfo> vec;

	for (int i=0;i<num;i++)
	{
		if(mWuPinPool[i]->getCount() > 0)
		{
			ItemInfo item;
			item.type = mWuPinPool[i]->getType();
			item.count = mWuPinPool[i]->getCount();
			vec.push_back(item);
		}
	}

	return vec;
}

void WuPinManager::clear()
{
	int num = mWuPinPool.size();

	for (int i=0;i<num;i++)
	{
		delete mWuPinPool[i];
	}

	mWuPinPool.clear();
}
WuPinManager::~WuPinManager(void)
{
}
