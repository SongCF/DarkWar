#pragma once
#include <string>
#include <vector>
#include <map>
#include "protocol.h"
#include "CommonDefine.h"
using namespace std;

struct SuitPart
{
	// �������װ�����װ������N��������ô�����group_id�������������
	// ���ٶ���һ��
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

	// �ϲ�GroupId
	void combine(vector<unsigned int> groupIds)
	{
		vector<unsigned int> newIds;

		for (int i=0,count=groupIds.size(); i<count; i++)
		{
			// �����˾Ͳ�����push��
			if (containsGroupId(groupIds[i]))
				continue;

			newIds.push_back(groupIds[i]);
		}

		// �ϲ�
		group_ids.insert(group_ids.end(),groupIds.begin(),groupIds.end());
	}
};

struct OneSuitInfo
{
	unsigned int suit_id;
	string suit_name;

	// ��ŵ�ȫ�ǵ�ǰ��װ����ɲ���
	vector<SuitPart> parts_of_suit;
	ZBAttr suit_attr[5][2]; //5�����ԣ�ÿ�����2����attr_typeΪ0��ʾ��Ч
};

class SuitPool
{
private:
	static SuitPool* pool;

	SuitPool(void);
	vector<OneSuitInfo> allSuits;
	// groupId����װ���ݵ�ӳ�䣬���ڼӿ����
	map<unsigned int,OneSuitInfo*> groupSuitMapping;
	// suitId����װ������ӳ��
	map<unsigned int,OneSuitInfo*> suitId_Suit_Mapping;

public:
	static SuitPool* getPool();
	void initSuit(vector<OneSuitInfo> all_suit);

	// ��groupIds�󶨵�һ��suitId���棬
	// groupIds��ͬһ��װ��
	// ��initSuit֮������
	void combineGroupIdsIntoSuitPart(vector<unsigned int> groupIds,unsigned int suitId);

	bool isSuit(unsigned int zb_group_id);

	// ��ȡ��װId����װ�Ѿ������˶��ټ���ӳ��
	// ������equipGroupIdOnOneHero������װA��������2������װB������3������ô���صľ���A--->2,B--->3��ӳ��
	// equipTypes����equipGroupIdOnOneHero��ÿһ��װ��������
	map<unsigned int,int> getSuitID2SuitNumMapping(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes);

	// ��ȡequipGroupIdOnOneHero�и�groupId������װЧ����group
	// ����ľͻ��Ƴ���
	vector<unsigned int> getInSuitedGroupIdsFromTheGroupId(vector<unsigned int> equipGroupIdOnOneHero,unsigned int groupId);


	// ��ȡequipGroupIds��ȥ�������������,����װ������
	// ���ǰѶ���ķ����Id�ҵ�,ȥ����
	// ����equipGroupIds-->{1,2,2,3,4,5}
	// ��1,2,3��һ��װ����3������4,5����һ��װ����2������
	// ��ô���صľ���1,4 ���������Ľ�����������������ֻ����2�����ݷ��ء�
	vector<unsigned int> getUniqueAndNoRedundancySuitGroupIds(vector<unsigned int> equipGroupIds);

	vector<OneSuitInfo> getAllSuits(); 

	// �ⷽ������
	// ����Ϊ�˼�������������include
	// �Ͳ������������
	//friend void ShenBingPool::call_SuitPool_combineGroupIdsIntoSuitPart(vector<unsigned int> groupIds,unsigned int suitId);

	//
	bool getSuitInforByZBGroupId(unsigned int zb_groud_id,OneSuitInfo& out_data);
	//
	bool getSuitPartByZBGroupId(unsigned int zb_groud_id,SuitPart& out_data);
	//
	bool getSuitInforBySuitId(unsigned int suitId,OneSuitInfo& out_data);

	// ���ݽ�ɫ���ϵ�װ�������ؿ���ӵ�е���װ�ӳ�
	// ���صľ������е����ݼӳɡ����� ����+20,����+2������+2����
	// ע��:�����ж�������ļӳ�!!!!
	// ������ָ����Ҫ�õ���ĳ�����Եļӳɣ���ô�ʹ�����Ӧ��attrType��-1��������
	vector<ZBAttr> getSuitZBAttriByEquip(vector<EquipData> euipsOnOneHero,int attrType = -1);
	vector<ZBAttr> getSuitZBAttriByEquipGroupId(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes,int attrType = -1);
	// ����һϵ��װ������ȡĳ��װ������
	// ����ж�������Եļӳɣ���ô���Զ����ӵ�һ������
	// û�о���0
	ZBAttr getSuitZBAttriByEquipGroupIdAndAttrType(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes,int attrType);
	ZBAttr getSuitZBAttriByEquipAndAttrType(vector<EquipData> equipOnOneHero,int attrType);
	~SuitPool(void);
};

