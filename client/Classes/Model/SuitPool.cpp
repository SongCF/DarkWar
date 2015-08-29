#include "SuitPool.h"

SuitPool* SuitPool::pool = new SuitPool();

SuitPool::SuitPool(void)
{
}

SuitPool* SuitPool::getPool()
{
	return pool;
}
void SuitPool::initSuit(vector<OneSuitInfo> all_suit)
{
	allSuits = all_suit;

	// group_id到OneSuitInfo的映射建立
	// suitId到OneSuitInfo的映射建立
	groupSuitMapping.clear();
	suitId_Suit_Mapping.clear();
	int suitNum = allSuits.size();
	for (int i = 0;i<suitNum;i++)
	{
		int partsNum = allSuits[i].parts_of_suit.size();
		for (int j=0;j<partsNum;j++)
		{
			groupSuitMapping.insert(pair<unsigned int, OneSuitInfo*>(allSuits[i].parts_of_suit[j].group_ids[0],&allSuits[i]));
		}

		suitId_Suit_Mapping.insert(pair<unsigned int, OneSuitInfo*>(allSuits[i].suit_id,&allSuits[i]));
	}


	// 测试map的特性
	{
		map<unsigned int,int> suit_num_mapping;
		map<unsigned int, int>::iterator iter;

		suit_num_mapping.insert(pair<unsigned int,int>(1,1));
		iter = suit_num_mapping.find(1);
		if (iter != suit_num_mapping.end())
		{
			iter->second += 10; // 这种更新可以
		}

		// 这就插入不进去
		suit_num_mapping.insert(pair<unsigned int,int>(1,5));
		iter = suit_num_mapping.find(1);
		if (iter != suit_num_mapping.end())
		{
			int k = iter->second;
			CCAssert(k == 11,"");
		}
	}

	/*{
		vector<unsigned int> groups;
		groups.push_back(1426);
		groups.push_back(1427);
		groups.push_back(1428);

		groups.push_back(1424);
		groups.push_back(1425);

		groups.push_back(1419);
		groups.push_back(1420);

		groups.push_back(1399);
		groups.push_back(1400);


		vector<ZBAttr> att = getSuitZBAttriByEquipGroupId(groups);
		vector<ZBAttr> att_baoji = getSuitZBAttriByEquipGroupId(groups,AttrType_baoji);
		vector<ZBAttr> att_skill = getSuitZBAttriByEquipGroupId(groups,AttrType_skillratio);
		vector<ZBAttr> att_gjmin = getSuitZBAttriByEquipGroupId(groups,AttrType_gongji_min);
		CCLog("");
	}*/
}

void SuitPool::combineGroupIdsIntoSuitPart(vector<unsigned int> groupIds,unsigned int suitId)
{
	CCAssert(groupIds.size() > 0,"测试用，用以告诉外界调用这个函数之前确保initSuit已经调用过了");
	CCAssert(suitId > 0 && !groupIds.empty(),"");

	bool found = false;

	for (int i=0,count=allSuits.size(); i<count; i++)
	{
		if (allSuits[i].suit_id != suitId)
			continue;

		for (int partIndex=0,partCount=allSuits[i].parts_of_suit.size(); partIndex<partCount; partIndex++)
		{
			// 如果当前的groupIds是属于这个套装的这个装备类型里面的一个套装
			// 那么把groupIds全部合并进去
			if (allSuits[i].parts_of_suit[partIndex].containsAnyGroupIdFromSet(groupIds))
			{
				// 合并到SuitPart中
				allSuits[i].parts_of_suit[partIndex].combine(groupIds);

				// 把groupid分散到groupSuitMapping中
				map<unsigned int,OneSuitInfo*>::iterator it;
				for (int groupIndex=0,groupCount=groupIds.size(); groupIndex<groupCount; groupIndex++)
				{
					it = groupSuitMapping.find(groupIds[groupIndex]);
					if (it == groupSuitMapping.end())
					{
						groupSuitMapping.insert(pair<unsigned int,OneSuitInfo*>(groupIds[groupIndex],&allSuits[i]));
					}
				}
				
				found = true;
				break;
			}
		}

		// 直接不用再循环找下一个suitid了
		break;
	}

	CCAssert(found,"必须找到，测试用，没找到应该就是bug");
}

bool SuitPool::isSuit(unsigned int zb_group_id)
{
	OneSuitInfo not_use;
	return getSuitInforByZBGroupId(zb_group_id,not_use);
}
bool SuitPool::getSuitInforByZBGroupId(unsigned int zb_groud_id,OneSuitInfo& out_data)
{
	out_data.suit_id = 0;// 无效

	map<unsigned int, OneSuitInfo*>::iterator iter;
	//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
	iter = groupSuitMapping.find(zb_groud_id);
	if(iter != groupSuitMapping.end())
	{
		OneSuitInfo infor = *(iter->second); // 拷贝一份
		out_data = infor;
		return true;
	}
	return false;
}

bool SuitPool::getSuitPartByZBGroupId(unsigned int zb_groud_id,SuitPart& out_data)
{
	OneSuitInfo suitInfor;
	if (!getSuitInforByZBGroupId(zb_groud_id,suitInfor))
		return false;

	for (int i=0,count=suitInfor.parts_of_suit.size(); i<count; i++)
	{
		if (suitInfor.parts_of_suit[i].containsGroupId(zb_groud_id))
		{
			out_data = suitInfor.parts_of_suit[i];
			return true;
		}
	}

	CCAssert(false,"异常");
	return false;
}

bool SuitPool::getSuitInforBySuitId(unsigned int suitId,OneSuitInfo& out_data)
{
	out_data.suit_id = 0;// 无效

	map<unsigned int, OneSuitInfo*>::iterator iter;
	//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
	iter = suitId_Suit_Mapping.find(suitId);
	if(iter != suitId_Suit_Mapping.end())
	{
		OneSuitInfo infor = *(iter->second); // 拷贝一份
		out_data = infor;
		return true;
	}
	return false;
}

vector<OneSuitInfo> SuitPool::getAllSuits()
{
	return allSuits;
}
vector<ZBAttr> SuitPool::getSuitZBAttriByEquip(vector<EquipData> euipsOnOneHero,int attrType /*= -1*/)
{
	vector<unsigned int> groupIds;
	vector<unsigned int> types;
	int num = euipsOnOneHero.size();
	for (int i=0;i<num;i++)
	{
		groupIds.push_back(euipsOnOneHero[i].group_id);
		types.push_back(euipsOnOneHero[i].zhuangbei_type);
	}
	return getSuitZBAttriByEquipGroupId(groupIds,types,attrType);
}
vector<ZBAttr> SuitPool::getSuitZBAttriByEquipGroupId(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes,int attrType /* = -1 */)
{
	vector<ZBAttr> ret;

	//// 测试效率
	//return ret;

	// 存放suitId 和 集齐的数量的映射，可以有N个套装
	map<unsigned int,int> suit_num_mapping = getSuitID2SuitNumMapping(equipGroupIdOnOneHero,equipTypes);
	map<unsigned int, int>::iterator iter;
	OneSuitInfo tempSuit;

	// 根据套装的件数，来获取对应的加成
	for(iter = suit_num_mapping.begin(); iter != suit_num_mapping.end(); iter++)
	{
		if (getSuitInforBySuitId(iter->first,tempSuit))
		{
			CCAssert(iter->second <= tempSuit.parts_of_suit.size(),"得到该套装的套装部分件数不可能超过该套装的总件数");

			// 添加套装属性
			for (int i=1;i<iter->second;i++)
			{
				if (tempSuit.suit_attr[i-1][0].attr_type != 0 && (attrType == -1 || attrType == tempSuit.suit_attr[i-1][0].attr_type) )
				{// 属性一 有效

					ret.push_back(tempSuit.suit_attr[i-1][0]);
				}

				if (tempSuit.suit_attr[i-1][1].attr_type != 0 && (attrType == -1 || attrType == tempSuit.suit_attr[i-1][1].attr_type) )
				{// 属性二 有效
					ret.push_back(tempSuit.suit_attr[i-1][1]);
				}
			}
		}
		else
		{
			CCAssert(false,"肯定能找到，如果没有找到就是Bug");
		}
	}

	return ret;
}

ZBAttr SuitPool::getSuitZBAttriByEquipGroupIdAndAttrType(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes,int attrType)
{
	CCAssert(attrType>0,"不支持-1，因为需要指定一个属性，才可以合并");
	vector<ZBAttr> attrs = getSuitZBAttriByEquipGroupId(equipGroupIdOnOneHero,equipTypes,attrType);

	ZBAttr ret;
	ret.attr_type = attrType;
	ret.attr_value = 0;
	ret.cuiqu_suc_ratio = 0;

	int num = attrs.size();
	for (int i=0;i<num;i++)
	{
		ret.attr_value += attrs[i].attr_value;
	}

	return ret;
}
ZBAttr SuitPool::getSuitZBAttriByEquipAndAttrType(vector<EquipData> equipOnOneHero,int attrType)
{
	vector<unsigned int> groupIds;
	vector<unsigned int> types;
	int num = equipOnOneHero.size();
	for (int i=0;i<num;i++)
	{
		groupIds.push_back(equipOnOneHero[i].group_id);
		types.push_back(equipOnOneHero[i].zhuangbei_type);
	}
	return getSuitZBAttriByEquipGroupIdAndAttrType(groupIds,types,attrType);
}

vector<unsigned int> SuitPool::getUniqueAndNoRedundancySuitGroupIds(vector<unsigned int> equipGroupIds)
{
	vector<unsigned int> ret;

	map<unsigned int,bool> uniqueEquipGroupIdOnOneHero;
	map<unsigned int,bool>::iterator itGroupIds;
	for (int i=0,count=equipGroupIds.size(); i<count; i++)
	{
		SuitPart suitPart;
		if (getSuitPartByZBGroupId(equipGroupIds[i],suitPart))
		{
			// 就取这个part的分身0作为groupId当代表
			// 有重复也无所谓，insert就插不进去的
			// 即使插入进去，也无所谓
			uniqueEquipGroupIdOnOneHero.insert(pair<unsigned int,bool>(suitPart.group_ids[0],true));
		}
	}

	for (itGroupIds = uniqueEquipGroupIdOnOneHero.begin(); itGroupIds != uniqueEquipGroupIdOnOneHero.end(); itGroupIds++)
	{
		ret.push_back(itGroupIds->first);
	}

	return ret;
}

vector<unsigned int> SuitPool::getInSuitedGroupIdsFromTheGroupId(vector<unsigned int> equipGroupIdOnOneHero,unsigned int groupId)
{
	vector<unsigned int> ret;

	OneSuitInfo suit;
	if (!getSuitInforByZBGroupId(groupId,suit))
	{
		return ret;
	}

	vector<unsigned int> pureData = getUniqueAndNoRedundancySuitGroupIds(equipGroupIdOnOneHero);
	for (int i=0,count=pureData.size(); i<count; i++)
	{
		OneSuitInfo temp;
		if (!getSuitInforByZBGroupId(pureData[i],temp))
		{
			CCAssert(false,"Bug");
			continue;
		}

		// 是一套的，就保存
		if (temp.suit_id == suit.suit_id)
		{
			ret.push_back(pureData[i]);
		}
	}

	return ret;
}

map<unsigned int,int> SuitPool::getSuitID2SuitNumMapping(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes)
{
	// 存放suitId 和 集齐的数量的映射，可以有N个套装
	map<unsigned int,int> suit_num_mapping;
	map<unsigned int, int>::iterator iter;
	OneSuitInfo tempSuit;

	// 不用这个字段了
	equipTypes.clear();


	// 去掉多余的，没用的
	equipGroupIdOnOneHero = getUniqueAndNoRedundancySuitGroupIds(equipGroupIdOnOneHero);
	vector<unsigned int>::iterator itGroupIds;

	for (itGroupIds = equipGroupIdOnOneHero.begin(); itGroupIds != equipGroupIdOnOneHero.end(); itGroupIds++)
	{
		tempSuit.suit_id = 0;
		if (getSuitInforByZBGroupId(*itGroupIds,tempSuit))
		{// 是套装
			CCAssert(tempSuit.suit_id!=0,"");

			//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
			iter = suit_num_mapping.find(tempSuit.suit_id);
			if(iter != suit_num_mapping.end())
			{
				iter->second++; // 更新值
			}else
			{
				suit_num_mapping.insert(pair<unsigned int, int>(tempSuit.suit_id,1));
			}
		}
	}

	return suit_num_mapping;
}



//map<unsigned int,int> SuitPool::getSuitID2SuitNumMapping(vector<unsigned int> equipGroupIdOnOneHero,vector<unsigned int> equipTypes)
//{
//	// 存放suitId 和 集齐的数量的映射，可以有N个套装
//	map<unsigned int,int> suit_num_mapping;
//	map<unsigned int, int>::iterator iter;
//	OneSuitInfo tempSuit;
//	// 保存的是在一个套装里面，已经处理过的装备的类型,bool是没有用的
//	// suitid--->{equipType--->{false}}
//	map<unsigned int,map<unsigned int,bool> > processedEquipTypeInSuitMaping;
//	map<unsigned int,map<unsigned int,bool> >::iterator iter_pro;
//
//	int equiNum = equipGroupIdOnOneHero.size();
//	for (int i=0;i<equiNum;i++)
//	{
//		tempSuit.suit_id = 0;
//
//		if (getSuitInforByZBGroupId(equipGroupIdOnOneHero[i],tempSuit))
//		{// 是套装
//			CCAssert(tempSuit.suit_id!=0,"");
//
//			// 检查
//			{
//				iter_pro = processedEquipTypeInSuitMaping.find(tempSuit.suit_id);
//				// 检查是否处理过了该套装里面的装备没有
//				if (iter_pro != processedEquipTypeInSuitMaping.end())
//				{
//					if (iter_pro->second.find(equipTypes[i]) !=  iter_pro->second.end())
//					{
//						// 已经处理过同类型的了
//						continue;
//					}
//					else
//					{
//						iter_pro->second.insert(pair<unsigned int,bool>(equipTypes[i],true));
//					}
//
//				}else
//				{
//					map<unsigned int,bool> procedTypes;
//					procedTypes.insert(pair<unsigned int,bool>(equipTypes[i],true));
//					processedEquipTypeInSuitMaping.insert(pair<unsigned int,map<unsigned int,bool> >(tempSuit.suit_id,procedTypes));
//				}
//			}
//			
//			
//
//			//如果map中没有要查找的数据，它返回的迭代器等于end函数返回的迭代器;
//			iter = suit_num_mapping.find(tempSuit.suit_id);
//			if(iter != suit_num_mapping.end())
//			{
//				iter->second++; // 更新值
//			}else
//			{
//				suit_num_mapping.insert(pair<unsigned int, int>(tempSuit.suit_id,1));
//			}
//		}
//	}
//
//	return suit_num_mapping;
//}


SuitPool::~SuitPool(void)
{
}
