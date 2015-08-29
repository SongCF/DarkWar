#pragma once
#include <string>
#include <vector>
#include <map>
#include "protocol.h"
#include "CommonDefine.h"
using namespace std;

struct SuitPart
{
	// 如果该套装的这个装备会有N个分身，那么分身的group_id都会出现在这里
	// 至少都有一个
	vector<unsigned int> group_ids;

	ZhuangbeiType zhuanbei_type;
	string zhuangbei_name;

	bool containsGroupId(unsigned int groupId)
	{
		for (int i=0,count=group_ids.size(); i<count; i++)
		{
			if (group_ids[i] == groupId)
			{
				return true;
			}
		}
		return false;
	}

	bool containsAnyGroupIdFromSet(vector<unsigned int> groupIds)
	{
		for (int i=0,count=groupIds.size(); i<count; i++)
		{
			if (containsGroupId(groupIds[i]))
			{
				return true;
			}
		}
		return false;
	}

	// 合并GroupId
	void combine(vector<unsigned int> groupIds)
	{
		vector<unsigned int> newIds;

		for (int i=0,count=groupIds.size(); i<count; i++)
		{
			// 已有了就不用再push了
			if (containsGroupId(groupIds[i]))
				continue;

			newIds.push_back(groupIds[i]);
		}

		// 合并
		group_ids.insert(group_ids.end(),groupIds.begin(),groupIds.end());
	}
};

struct OneSuitInfo
{
	unsigned int suit_id;
	string suit_name;

	// 存放的全是当前套装的组成部分
	vector<SuitPart> parts_of_suit;
	ZBAttr suit_attr[5][2]; //5组属性，每组最多2个。attr_type为0表示无效
};

class SuitPool
{
private:
	static SuitPool* pool;

	SuitPool(void);
	vector<OneSuitInfo> allSuits;
	// groupId到套装数据的映射，用于加快查找
	map<unsigned int,OneSuitInfo*> groupSuitMapping;
	// suitId到套装的数据映射
	map<unsigned int,OneSuitInfo*> suitId_Suit_Mapping;

public:
	static SuitPool* getPool();
	void initSuit(vector<OneSuitInfo> all_suit);

	// 把groupIds绑定到一个suitId上面，
	// groupIds是同一件装备
	// 在initSuit之后设置
	void combineGroupIdsIntoSuitPart(vector<unsigned int> groupIds,unsigned int suitId);

	bool isSuit(unsigned int zb_group_id);

	// 获取套装Id到套装已经集齐了多少件的映射
	// 比如在equipGroupIdOnOneHero中有套装A，集齐了2件，套装B集齐了3件，那么返回的就是A--->2,B--->3的映射
	// equipTypes代表equipGroupIdOnOneHero中每一个装备的类型
	map<unsigned int,int> getSuitID2SuitNumMapping(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes);

	// 获取equipGroupIdOnOneHero中跟groupId触发套装效果的group
	// 多余的就会移除掉
	vector<unsigned int> getInSuitedGroupIdsFromTheGroupId(vector<unsigned int> equipGroupIdOnOneHero,unsigned int groupId);


	// 获取equipGroupIds中去掉了冗余的数据,是套装的数据
	// 就是把多余的分身的Id找到,去除掉
	// 比如equipGroupIds-->{1,2,2,3,4,5}
	// 而1,2,3是一件装备的3个分身，4,5是另一件装备的2个分身
	// 那么返回的就是1,4 或者其他的结果，但是这种情况下只会有2个数据返回。
	vector<unsigned int> getUniqueAndNoRedundancySuitGroupIds(vector<unsigned int> equipGroupIds);

	vector<OneSuitInfo> getAllSuits(); 

	// 这方法可行
	// 但是为了减少依赖，不想include
	// 就不用这个方法了
	//friend void ShenBingPool::call_SuitPool_combineGroupIdsIntoSuitPart(vector<unsigned int> groupIds,unsigned int suitId);

	//
	bool getSuitInforByZBGroupId(unsigned int zb_groud_id,OneSuitInfo& out_data);
	//
	bool getSuitPartByZBGroupId(unsigned int zb_groud_id,SuitPart& out_data);
	//
	bool getSuitInforBySuitId(unsigned int suitId,OneSuitInfo& out_data);

	// 根据角色身上的装备，返回可以拥有的套装加成
	// 返回的就是所有的数据加成。比如 攻击+20,护甲+2，攻击+2，等
	// 注意:可以有多个攻击的加成!!!!
	// 还可以指定需要得到的某个属性的加成，那么就传入相应的attrType，-1代表所有
	vector<ZBAttr> getSuitZBAttriByEquip(vector<EquipData> euipsOnOneHero,int attrType = -1);
	vector<ZBAttr> getSuitZBAttriByEquipGroupId(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes,int attrType = -1);
	// 根据一系列装备，获取某套装的属性
	// 如果有多个该属性的加成，那么就自动叠加到一个里面
	// 没有就是0
	ZBAttr getSuitZBAttriByEquipGroupIdAndAttrType(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes,int attrType);
	ZBAttr getSuitZBAttriByEquipAndAttrType(vector<EquipData> equipOnOneHero,int attrType);
	~SuitPool(void);
};

