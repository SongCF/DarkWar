#include "SuiPianManager.h"
#include "ShenBingPool.h"
#include <map>
#include <cmath>
#include "cocos2d.h"
#include "CS_Common.h"

using namespace cocos2d;

SuiPianManager* SuiPianManager::instance = NULL;

SuiPianManager::SuiPianManager(void)
{
}

SuiPianManager* SuiPianManager::getManager()
{
	if (instance == NULL)
	{
		instance = new SuiPianManager();
	}
	return instance;
}

// 初始化
void SuiPianManager::initItemSuiPianData(vector<ItemSuipianInfo> itemSuipianVec)
{
	map<string,ItemSuipianInfo> unique_map;
	map<string,ItemSuipianInfo>::iterator it;
	// 校验重复
	{
		char tempKey[128];
		for (int i=0,count = itemSuipianVec.size(); i< count; i++)
		{
			memset(tempKey,0,sizeof(tempKey));
			sprintf(tempKey,"%d_%d",itemSuipianVec[i].item_id,itemSuipianVec[i].suipian_id);
			it = unique_map.find(tempKey);
			if (it != unique_map.end())
			{
				CCLog("Error : %s--> item is not unique!!! this is maybe a bug in the server!",__FUNCTION__);
				it->second.suipian_count += itemSuipianVec[i].suipian_count;
			}else
			{
				unique_map.insert(pair<string,ItemSuipianInfo>(tempKey,itemSuipianVec[i]));
			}
		}
	}


	this->itemSuipianVec.clear();

	for (it=unique_map.begin(); it != unique_map.end(); it++)
	{
		ItemSuipian data;

		data.item_type = (ItemsType)it->second.item_id;
		data.suipian_id = it->second.suipian_id;
		data.suipian_count = it->second.suipian_count;

		this->itemSuipianVec.push_back(data);
	}
}
void SuiPianManager::initShenBingSuiPianData(vector<ShenbingSuipianInfo> shenBingSuipianVec)
{
	map<string,ShenbingSuipianInfo> unique_map;
	map<string,ShenbingSuipianInfo>::iterator it;
	// 校验重复
	{
		char tempKey[128];
		for (int i=0,count = shenBingSuipianVec.size(); i< count; i++)
		{
			memset(tempKey,0,sizeof(tempKey));
			sprintf(tempKey,"%d_%d",shenBingSuipianVec[i].zb_group_id,shenBingSuipianVec[i].suipian_id);
			it = unique_map.find(tempKey);
			if (it != unique_map.end())
			{
				CCLog("Error : %s--> item is not unique!!! this is maybe a bug in the server!",__FUNCTION__);
				it->second.suipian_count += shenBingSuipianVec[i].suipian_count;
			}else
			{
				unique_map.insert(pair<string,ShenbingSuipianInfo>(tempKey,shenBingSuipianVec[i]));
			}
		}
	}


	this->shenBingSuipianVec.clear();

	for (it = unique_map.begin(); it != unique_map.end(); it++)
	{
		ShenbingSuipian data;

		data.zb_group_id = it->second.zb_group_id;
		data.suipian_id = it->second.suipian_id;
		data.suipian_count = it->second.suipian_count;

		int useMinLevel = CS::getShenBingLevels()[0];
		// 查询神兵数据
		bool found = ChuanqiPool::getPool()->getChuanQiData(data.zb_group_id,data.suiPianDetail,useMinLevel);
		CCAssert(found,"必须找到");

		this->shenBingSuipianVec.push_back(data);
	}
}

// 添加一个碎片数据：给指定类型和Id的碎片添加指定的个数
void SuiPianManager::addOneItemSuiPian(ItemSuipianInfo item)
{
	for (int i=0,count=itemSuipianVec.size(); i<count; i++)
	{
		// 已经有了，那么就更新
		if (itemSuipianVec[i].item_type == item.item_id && itemSuipianVec[i].suipian_id == item.suipian_id)
		{
			itemSuipianVec[i].suipian_count += item.suipian_count;
			return;
		}
	}

	ItemSuipian data;
	data.item_type = (ItemsType)item.item_id;
	data.suipian_id = item.suipian_id;
	data.suipian_count = item.suipian_count;
	itemSuipianVec.push_back(data);
}
void SuiPianManager::addOneShenBingSuiPian(ShenbingSuipianInfo item)
{
	for (int i=0,count=shenBingSuipianVec.size(); i<count; i++)
	{
		// 已经有了，那么就更新
		if (shenBingSuipianVec[i].zb_group_id == item.zb_group_id && shenBingSuipianVec[i].suipian_id == item.suipian_id)
		{
			shenBingSuipianVec[i].suipian_count += item.suipian_count;
			return;
		}
	}

	ShenbingSuipian data;
	data.zb_group_id = item.zb_group_id;
	data.suipian_id = item.suipian_id;
	data.suipian_count = item.suipian_count;
	int useMinLevel = CS::getShenBingLevels()[0];
	bool found = ChuanqiPool::getPool()->getChuanQiData(data.zb_group_id,data.suiPianDetail,useMinLevel);
	CCAssert(found,"必须找到！");
	shenBingSuipianVec.push_back(data);
}

// 移除一个碎片数据：给指定类型和Id的碎片移除指定的个数
void SuiPianManager::subOneItemSuiPian(ItemSuipianInfo item)
{
	for (int i=0,count=itemSuipianVec.size(); i<count; i++)
	{
		// 已经有了，那么就更新
		if (itemSuipianVec[i].item_type == item.item_id && itemSuipianVec[i].suipian_id == item.suipian_id)
		{
			if (itemSuipianVec[i].suipian_count >= item.suipian_count)
			{
				itemSuipianVec[i].suipian_count -= item.suipian_count;
			}else
			{
				CCLog("Error : runtime error ---> %s our item count is not enough to sub, recover--> clear 0!",__FUNCTION__);
				itemSuipianVec[i].suipian_count = 0;
				CCAssert(false,"错误");
			}

			return;
		}
	}

	CCAssert(false,"没有找到..");

}
void SuiPianManager::subOneShenBingSuiPian(ShenbingSuipianInfo item)
{
	for (int i=0,count=shenBingSuipianVec.size(); i<count; i++)
	{
		// 已经有了，那么就更新
		if (shenBingSuipianVec[i].zb_group_id == item.zb_group_id && shenBingSuipianVec[i].suipian_id == item.suipian_id)
		{
			if (shenBingSuipianVec[i].suipian_count >= item.suipian_count)
			{
				shenBingSuipianVec[i].suipian_count -= item.suipian_count;
			}else
			{
				CCLog("Error : runtime error ---> %s our item count is not enough to sub, recover--> clear 0!",__FUNCTION__);
				shenBingSuipianVec[i].suipian_count = 0;
				CCAssert(false,"错误");
			}

			return;
		}
	}

	CCAssert(false,"没有找到..");

}

// 添加type类型的Item碎片的个数，是这个类型的都会添加，从id = 1到id = MAX
// toAdd小于0就是减
void SuiPianManager::addItemSuiPianNumByItemType(ItemsType type,int toAdd)
{
	ItemSuipianInfo suipian;
	suipian.item_id = type;
	suipian.suipian_count = abs(toAdd);
	if (toAdd < 0) // 减
	{
		for (int i=0;i<MAX_SUIPIAN_ID_NUM;i++)
		{
			suipian.suipian_id = i+1;
			subOneItemSuiPian(suipian);
		}
	}
	else if(toAdd > 0) // 加
	{
		for (int i=0;i<MAX_SUIPIAN_ID_NUM;i++)
		{
			suipian.suipian_id = i+1;
			addOneItemSuiPian(suipian);
		}
	}
}
// 添加神兵碎片的个数，从id1到idMAX
void SuiPianManager::addShenBingSuiPianNumByZBGroupId(unsigned int zb_groupId,int toAdd)
{
	ShenbingSuipianInfo suipian;
	suipian.zb_group_id = zb_groupId;
	suipian.suipian_count = abs(toAdd);
	if (toAdd < 0) // 减
	{
		for (int i=0;i<MAX_SUIPIAN_ID_NUM;i++)
		{
			suipian.suipian_id = i+1;
			subOneShenBingSuiPian(suipian);
		}
	}
	else if(toAdd > 0) // 加
	{
		for (int i=0;i<MAX_SUIPIAN_ID_NUM;i++)
		{
			suipian.suipian_id = i+1;
			addOneShenBingSuiPian(suipian);
		}
	}
}


bool SuiPianManager::getShenBingSuiPianNumByGroupId(unsigned int groupId,ShenbingSuipianGroupData& outData)
{
	return getShenBingSuiPianNumByGroupId(groupId,getAllShenbingSuiPianWithGrouped(),outData);
}
bool SuiPianManager::getItemSuiPianNumByType(unsigned int itemType,ItemSuipianGroupData& outData)
{
	return getItemSuiPianNumByType(itemType,getAllItemSuiPianWithGrouped(),outData);
}

bool SuiPianManager::getShenBingSuiPianNumByGroupId(unsigned int groupId,const vector<ShenbingSuipianGroupData>& dataPool,ShenbingSuipianGroupData& outData)
{
	for (int i=0,count=dataPool.size(); i<count; i++)
	{
		if (dataPool[i].suiPianDetail.zb_group_id == groupId)
		{
			outData = dataPool[i];
			return true;
		}
	}
	return false;
}
bool SuiPianManager::getItemSuiPianNumByType(unsigned int itemType,const vector<ItemSuipianGroupData>& dataPool,ItemSuipianGroupData& outData)
{
	for (int i=0,count=dataPool.size(); i<count; i++)
	{
		if (dataPool[i].item_type == itemType)
		{
			outData = dataPool[i];
			return true;
		}
	}

	return false;
}



// 有组织的碎片数据
vector<ItemSuipianGroupData> SuiPianManager::getAllItemSuiPianWithGrouped()
{

	// 存放item_type到ItemSuipianGroupData的映射
	map<ItemsType,ItemSuipianGroupData> grouped_map;
	map<ItemsType,ItemSuipianGroupData>::iterator it;
	for (int i=0,count=itemSuipianVec.size(); i<count; i++)
	{
		it = grouped_map.find(itemSuipianVec[i].item_type);

		if (it != grouped_map.end())
		{// 找到就更新
			it->second.count_array[itemSuipianVec[i].suipian_id] += itemSuipianVec[i].suipian_count;
		}else
		{
			ItemSuipianGroupData newData;
			newData.item_type = itemSuipianVec[i].item_type;
			newData.count_array[itemSuipianVec[i].suipian_id] = itemSuipianVec[i].suipian_count;

			grouped_map.insert(pair<ItemsType,ItemSuipianGroupData>(itemSuipianVec[i].item_type,newData));
			
		}
	}

	vector<ItemSuipianGroupData> ret;
	for (it = grouped_map.begin(); it != grouped_map.end(); it++)
	{
		ret.push_back(it->second);
	}
	return ret;

}
vector<ShenbingSuipianGroupData> SuiPianManager::getAllShenbingSuiPianWithGrouped()
{
	// 存放group_id到ShenbingSuipianGroupData的映射
	map<unsigned int,ShenbingSuipianGroupData> grouped_map;
	map<unsigned int,ShenbingSuipianGroupData>::iterator it;
	for (int i=0,count=shenBingSuipianVec.size(); i<count; i++)
	{
		it = grouped_map.find(shenBingSuipianVec[i].zb_group_id);

		if (it != grouped_map.end())
		{// 找到就更新
			it->second.count_array[shenBingSuipianVec[i].suipian_id] += shenBingSuipianVec[i].suipian_count;
		}else
		{
			ShenbingSuipianGroupData newData;
			int useMinLevel = CS::getShenBingLevels()[0];
			bool mustTrue = ChuanqiPool::getPool()->getChuanQiData(shenBingSuipianVec[i].zb_group_id,newData.suiPianDetail,useMinLevel);
			CCAssert(mustTrue,"");
			newData.count_array[shenBingSuipianVec[i].suipian_id] = shenBingSuipianVec[i].suipian_count;

			grouped_map.insert(pair<unsigned int,ShenbingSuipianGroupData>(shenBingSuipianVec[i].zb_group_id,newData));
		}
	}


	vector<ShenbingSuipianGroupData> ret;
	for (it = grouped_map.begin(); it != grouped_map.end(); it++)
	{
		ret.push_back(it->second);
	}
	return ret;

}

SuiPianManager::~SuiPianManager(void)
{
}
