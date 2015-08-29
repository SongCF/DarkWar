#include "EquipmentManager.h"
#include "Equipment.h"
#include "LFFileUtiles.h"
#include "MyselfManager.h"
#include "CS_Common.h"
#include "EquipObserver.h"


EquipmentManager* EquipmentManager::manger = new EquipmentManager();
EquipmentManager::EquipmentManager(void)
{
	removeAll();
}



EquipmentManager* EquipmentManager::getManager()
{
	return manger;
}


bool EquipmentManager::initManager(vector<EquipData> configDatas)
{
	// 要保证MySelfManager先初始化，这样就可以给角色装上装备
	CCAssert(MyselfManager::getManager()->getMyZhuJueData()!=NULL,"");
	removeAll();

	int num = configDatas.size();
	for (int i=0;i<num;i++)
	{
		Equipment* temp = new Equipment(configDatas[i]);
		equipmentVec.push_back(temp);
	}

	// 初始化角色 装备
	{
		//vector<BaseActor*> allActors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All);
		vector<BaseActor*> allActors = MyselfManager::getManager()->getAllTeamActors(Actor_All,LF_All,ACTOR_TYPE_ALL,false);
		int actorNum = allActors.size();
		for (int i=0;i<actorNum;i++)
		{
			// 挨个初始化
			allActors[i]->initEquipment(getEquipOnActor(allActors[i]->getId()));
		}
	}

	CCLog("EquipmentManager::initManager ----> init over!");

	EquipObserver::getObserver()->inittedEquipManager();

	return true;
}

//void EquipmentManager::refreshData(vector<EquipData> configDatas)
//{
//	CCAssert(false,"先不要调用这个方法");
//
//	// 要保证MySelfManager先初始化，这样就可以给角色装上装备
//	CCAssert(MyselfManager::getManager()->getMyZhuJueData()!=NULL,"");
//
//	int num = configDatas.size();
//	for (int i=0;i<num;i++)
//	{
//		Equipment* temp = getOneEquipment(configDatas[i].zhuangbei_id);
//		if (temp == NULL)
//		{
//			temp = new Equipment(configDatas[i]);
//			equipmentVec.push_back(temp);
//		}
//		else
//		{
//			temp->refreshData(configDatas[i]);
//		}
//	}
//
//	// 检查一致性，合并
//	{
//		// 数据不一致，需要删减
//		if (configDatas.size() != equipmentVec.size())
//		{
//			CCAssert(false,"");
//
//			
//		}
//	}
//
//	// 初始化角色 装备
//	{
//		vector<BaseActor*> allActors = MyselfManager::getManager()->getAllTeamActors(Actor_All,ACTOR_TYPE_ALL,false);
//		//vector<BaseActor*> allActors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_All);
//		int actorNum = allActors.size();
//		for (int i=0;i<actorNum;i++)
//		{
//			// 挨个初始化
//			allActors[i]->initEquipment(getEquipOnActor(allActors[i]->getId()));
//		}
//	}
//
//	CCLog("EquipmentManager::initManager ----> init over!");
//}


bool EquipmentManager::isSpareEnoughToSaveEquip(unsigned int num)
{
	int not_use_num = 0;
	for (int i=0,count=equipmentVec.size(); i<count; i++)
	{
		if (equipmentVec[i]->getActorId() == INVALID_ID && 
			equipmentVec[i]->getEquipColor() < ZhuangbeiColour_Chuanqi)
		{
			not_use_num ++;
		}
	}

	return (not_use_num+num) <= MAX_COUNTS_IN_KNAPSACK;
	//return (not_use_num+num) <= 5;
}

bool EquipmentManager::isEquipmentExist(unsigned long long zhuangbei_id)
{
	int num = equipmentVec.size();
	for (int i=0;i<num;i++)
	{
		if (equipmentVec[i]->getEquipId() == zhuangbei_id)
		{
			return true;
		}
	}
	return false;
}

void EquipmentManager::addOneEquipment(EquipData data)
{
	if (data.zhuangbei_id <= 0)
	{
		CCAssert(false,"EquipmentManager::addOneEquipment ---> zhuangbei_id <= 0");
		return;
	}
	if (isEquipmentExist(data.zhuangbei_id))
	{
		CCLog("Waring : EquipmentManager::addOneEquipment --> Equipment Is Exist!");
		return;
	}

	//if (data.zhuangbei_hero_id > 0)
	//{
	//	CCAssert(false,"不支持已经装在别人身上的装备。这个方法就是掉装时用的");
	//	return;
	//}
	Equipment* equip = new Equipment(data);
	equipmentVec.push_back(equip);

	if (data.zhuangbei_hero_id > 0)
	{
		BaseActor* owner = MyselfManager::getManager()->getMyTeamActor(data.zhuangbei_hero_id);
		CCAssert(owner!=NULL,"actor必须已经存在--->是否忘了需要先将Actor的数据加到MyselfManager??");
		owner->attachEquipment(equip);
	}

	EquipObserver::getObserver()->addOneToEquipeManager(equip);
}

Equipment* EquipmentManager::getOneEquipment(unsigned long long eId)
{
	int num = equipmentVec.size();

	for (int i=0;i<num;i++)
	{
		if (equipmentVec[i]->getEquipId() == eId)
		{
			return equipmentVec[i];
		}
	}
	return NULL;
}

vector<Equipment*> EquipmentManager::getEquipmentsByGroupId(unsigned int group_id)
{
	vector<Equipment*> ret;
	int num = equipmentVec.size();

	for (int i=0;i<num;i++)
	{
		if (equipmentVec[i]->getGroupId() == group_id)
		{
			ret.push_back(equipmentVec[i]);
		}
	}
	return ret;
}

bool EquipmentManager::haveSuitPart(unsigned int group_id)
{
	return getEquipmentsByGroupId(group_id).size() > 0;
}




vector<Equipment*> EquipmentManager::getAllEquipmentsOnActor(unsigned int actorId, Filter_Equip filter,FilterEquipType filterType /* = Filter_Type_All */)
{
	BaseActor* actor = MyselfManager::getManager()->getMyTeamActor(actorId);
	vector<Equipment*> equipOn = actor->getEquipments();
	return equipmentFilter(equipOn,filter,filterType);
}

vector<Equipment*> EquipmentManager::equipmentFilter(vector<Equipment*> data,Filter_Equip filter,FilterEquipType filterType)
{
	vector<Equipment*> tempRet;

	vector<Equipment*> allEquipInTeam = data;
	int num = allEquipInTeam.size();
	for (int i=0;i<num;i++)
	{
		// 需要装备了的， 于是对于没有装备就放弃
		if (filter == Filter_Equipped && allEquipInTeam[i]->getActorId() == 0)
			continue;
		// 需要没有装备的， 于是对于有装备就放弃
		if (filter == Filter_No_Equipped && allEquipInTeam[i]->getActorId() != 0)
			continue;
		// 要求是防具，那么不是防具就放弃
		if(filterType == Filter_Type_FangJu && allEquipInTeam[i]->getEquipType() != EquipType_FangJu)
			continue;
		// 要求是饰品，那么不是饰品就放弃
		if(filterType == Filter_Type_ShiPin && allEquipInTeam[i]->getEquipType() != EquipType_ShiPin)
			continue;
		// 要求是武器，那么不是武器就放弃
		if(filterType == Filter_Type_WuQi && allEquipInTeam[i]->getEquipType() != EquipType_WuQi)
			continue;

		tempRet.push_back(allEquipInTeam[i]);
	}

	return tempRet;
}

vector<Equipment*> EquipmentManager::getAllEquipment(Filter_Equip filter,FilterEquipType filterType /* = Type_All */)
{
	return equipmentFilter(getAllEquipment(),filter,filterType);
}



//
void EquipmentManager::destoryOneEquip(unsigned long long zhuangBeiId)
{
	vector<Equipment*>::iterator it;

	for (it = equipmentVec.begin();it!=equipmentVec.end();it++)
	{
		if ( (*it)->getEquipId() == zhuangBeiId)
		{
			Equipment* toRemove = (*it);

			// 通知移除
			EquipObserver::getObserver()->removeOneFromEquipManager(toRemove);

			// 从使用者上移除
			MyselfManager::getManager()->removeEquipementFromOwner(toRemove);
			it = equipmentVec.erase(it);
			CCLog("destoryOneEquip->id = %lld",zhuangBeiId);
			delete toRemove;

			break;
		}
	}
}

void EquipmentManager::removeAll()
{
	int num = equipmentVec.size();
	for (int i=0;i<num;i++)
	{
		delete equipmentVec[i];
	}
	equipmentVec.clear();
}

EquipType EquipmentManager::getEquipType(ZhuangbeiType type)
{
	return (EquipType)CS::getEquipType(type);
}

bool EquipmentManager::canEquipToPlace(ZhuangbeiType type,EquipRequirePlace place)
{
	vector<EquipRequirePlace> requirePlaces = getRequirdPlace(type);
	int num = requirePlaces.size();
	for (int i=0;i<num;i++)
	{
		if (requirePlaces[i] == place)
		{
			return true;
		}
	}
	return false;
}

vector<EquipRequirePlace> EquipmentManager::getRequirdPlace(ZhuangbeiType type)
{
	vector<EquipRequirePlace> requirePlace;

	if (type == ZhuangbeiType_DanshouJian ||
		type == ZhuangbeiType_ShuangshouBishou ||
		type == ZhuangbeiType_ShuangshouFu ||
		type == ZhuangbeiType_DanshouFazhang ||
		type == ZhuangbeiType_ShuangshouFazhang ||
		type == ZhuangbeiType_Gong)
	{
		requirePlace.push_back(POS_ZhuWuQi);
	}
	else if (type == ZhuangbeiType_Dunpai
		|| type == ZhuangbeiType_Faqi)
	{
		requirePlace.push_back(POS_FuShou);
	}
	else if (type == ZhuangbeiType_Hushenfu)
	{
		requirePlace.push_back(POS_Hushenfu);
	}
	else if (
		type == ZhuangbeiType_KuijiaZhong ||
		type == ZhuangbeiType_KuijiaQin
		)
	{
		requirePlace.push_back(POS_XiongJia);
	}
	else if (type == ZhuangbeiType_Jiezhi)
	{
		requirePlace.push_back(POS_JieZhi1);
		requirePlace.push_back(POS_JieZhi2);
	}else
	{
		CCLog("Error : EquipmentManager::getRequirdPlace ---> type = %d",type);
		CCAssert(false,"不支持！");
	}

	return requirePlace;
}

bool EquipmentManager::canUseEquip(ZhuangbeiType zhuangbei_type,Profession profession_id)
{
	return CS::canEquipToHero(zhuangbei_type,profession_id);
}

std::vector<std::string> EquipmentManager::getZBShuXingString(const vector<ZBAttr>& attr_vec,bool group_common_attr_type /* = false */)
{
	vector<ZBAttr> attr_vec_op;
	if (group_common_attr_type)
	{
		// 归并同类型的数据
		// type-->value的映射
		map<unsigned int,unsigned int> mapTypeToValue;
		map<unsigned int,unsigned int>::iterator it;
		for (int i=0,count = attr_vec.size();i< count;i++)
		{
			it = mapTypeToValue.find(attr_vec[i].attr_type);
			if (it == mapTypeToValue.end())
			{
				mapTypeToValue.insert(pair<unsigned int,unsigned int>(attr_vec[i].attr_type,attr_vec[i].attr_value));
			}
			else
			{
				it->second += attr_vec[i].attr_value; // 更新
			}
		}
		for (it = mapTypeToValue.begin();it != mapTypeToValue.end();it++)
		{
			ZBAttr one;
			one.attr_type = it->first;
			one.attr_value = it->second;
			one.cuiqu_suc_ratio = 0;
			attr_vec_op.push_back(one);
		}
	}
	else
	{
		attr_vec_op = attr_vec;
	}

	// 遍历拼接
	std::vector<std::string> vec;
	char str[128];
	ZBAttr item;
	for (int i = 0,count = attr_vec_op.size(); i< count; i++)
	{
		item = attr_vec_op[i];
		memset(str,0,sizeof(str));

		switch (item.attr_type)
		{
		case AttrType_mingjie:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("minjie").c_str());
			vec.push_back(str);
			break;
		case AttrType_liliang:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("liliang").c_str());
			vec.push_back(str);
			break;
		case AttrType_zhili:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("zhili").c_str());
			vec.push_back(str);
			break;
		case AttrType_tineng:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("tineng").c_str());
			vec.push_back(str);
			break;
		case AttrType_gongji_min:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("zuixiaoshanghai").c_str());
			vec.push_back(str);
			break;
		case AttrType_gongji_max:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("zuidashanghai").c_str());
			vec.push_back(str);
			break;
		case AttrType_baoji:
			sprintf(str, "+%u%% %s", item.attr_value, LFStrings::getValue("baojilv").c_str());
			vec.push_back(str);
			break;
		case AttrType_baoshang:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("baojishanghai").c_str());
			vec.push_back(str);
			break;
		case AttrType_hujia_add:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("huJia").c_str());
			vec.push_back(str);
			break;
		case AttrType_hp_percent:
			sprintf(str, "+%u%% %s", item.attr_value, LFStrings::getValue("shengmingjiacheng").c_str());
			vec.push_back(str);
			break;
		case AttrType_kangxing:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("moKang").c_str());
			vec.push_back(str);
			break;
		case AttrType_gedanglv:
			sprintf(str, "+%u%% %s", item.attr_value, LFStrings::getValue("gedanglv").c_str());
			vec.push_back(str);
			break;
		case AttrType_gedangzhi:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("gedangzhi").c_str());
			vec.push_back(str);
			break;
		case AttrType_skillzhouqi:
			sprintf(str, "%s%u%s", LFStrings::getValue("jiNengZhouQi").c_str(), item.attr_value, LFStrings::getValue("lun").c_str());
			vec.push_back(str);
			break;
		case AttrType_skillturn:
			sprintf(str, "%s%u%s", LFStrings::getValue("jiNengChuFaDian").c_str(),item.attr_value, LFStrings::getValue("lun").c_str());
			vec.push_back(str);
			break;
		case AttrType_skillresult:
			sprintf(str, "+%u %s", item.attr_value, LFStrings::getValue("jiNengXiaoGuoJiaCheng").c_str());
			vec.push_back(str);
			break;
		default:
			CCLog("Error : %s unknown AttrType [%d]",__FUNCTION__,item.attr_type);
			break;
		}
	}
	return vec;
}


// 从已装备的装备中，寻找需要被替换下来的装备
vector<Equipment*> EquipmentManager::getNeedTakeOffIfWareOnEqui(ZhuangbeiType type,EquipRequirePlace needPlace,vector<Equipment*> currentEquiped)
{
	CCAssert(needPlace != POS_NONE,"必须指定确定的位置，不然很麻烦");

	vector<Equipment*> ret;

	// 只有2个特殊情况：
	//1. 要穿上的是副武器，但是目前穿了一个双手主武器
	//2. 要穿上的是双手主武器，目前穿了主武器或者副武器

	// 情况1
	if (needPlace == POS_FuShou)
	{
		// 已经穿了一个主武器是双手的，那么戴副手就需要移除主武器
		Equipment* zhu = getEquipmentWhichEquipedAtPos(POS_ZhuWuQi,currentEquiped);
		if (zhu!=NULL &&CS::isShuangShouWuQi(zhu->getType()) )
		{
			ret.push_back(zhu);

			return ret;
		}
		else
		{ // 主武器没有，或者是单手的，那么不用管，不是特殊情况
		}
	}
	// 情况2
	else if ( CS::isShuangShouWuQi(type))
	{
		Equipment* zhu = getEquipmentWhichEquipedAtPos(POS_ZhuWuQi,currentEquiped);
		Equipment* fu = getEquipmentWhichEquipedAtPos(POS_FuShou,currentEquiped);

		if(zhu)
			ret.push_back(zhu);
		if(fu)
			ret.push_back(fu);
		return ret;
	}
	//else if (type == ZhuangbeiType_Jiezhi)
	//{
	//	Equipment* jiezhi1 = getEquipmentWhichEquipedAtPos(POS_JieZhi1,currentEquiped);
	//	Equipment* jiezhi2 = getEquipmentWhichEquipedAtPos(POS_JieZhi2,currentEquiped);
	//	// 如果已经装了2个戒子，再装，就脱掉戒子2
	//	if(jiezhi1 != NULL && jiezhi2 !=NULL)
	//	{
	//		ret.push_back(jiezhi2);
	//	}
	//	return ret;
	//}

	// 平时的处理，直接1对1的替换
	Equipment* temp = getEquipmentWhichEquipedAtPos(needPlace,currentEquiped);

	if (temp)
		ret.push_back(temp);

	return ret;










	//vector<Equipment*> ret;

	//vector<EquipRequirePlace> place = getRequirdPlace(type);

	//if (type != ZhuangbeiType_Jiezhi)
	//{
	//	CCAssert(place.size() == 1,"不是戒子的目前只会有一个目标卡槽");
	//}else
	//{
	//	CCAssert(place.size() == 2,"戒子就会有2个卡槽");
	//}

	//// 只有2个特殊情况：
	////1. 要穿上的是副武器，但是目前穿了一个双手主武器
	////2. 要穿上的是双手主武器，目前穿了主武器或者副武器

	//// 情况1
	//if (place[0] == POS_FuShou)
	//{
	//	// 已经穿了一个主武器是双手的，那么戴副手就需要移除主武器
	//	Equipment* zhu = getEquipmentWhichEquipedAtPos(POS_ZhuWuQi,currentEquiped);
	//	if (zhu!=NULL &&CS::isShuangShouWuQi(zhu->getType()) )
	//	{
	//		ret.push_back(zhu);

	//		return ret;
	//	}
	//	else
	//	{ // 主武器没有，或者是单手的，那么不用管，不是特殊情况
	//	}
	//}
	//// 情况2
	//else if ( CS::isShuangShouWuQi(type))
	//{
	//	Equipment* zhu = getEquipmentWhichEquipedAtPos(POS_ZhuWuQi,currentEquiped);
	//	Equipment* fu = getEquipmentWhichEquipedAtPos(POS_FuShou,currentEquiped);

	//	if(zhu)
	//		ret.push_back(zhu);
	//	if(fu)
	//		ret.push_back(fu);
	//	return ret;
	//}
	//else if (type == ZhuangbeiType_Jiezhi)
	//{
	//	Equipment* jiezhi1 = getEquipmentWhichEquipedAtPos(POS_JieZhi1,currentEquiped);
	//	Equipment* jiezhi2 = getEquipmentWhichEquipedAtPos(POS_JieZhi2,currentEquiped);

	//	// 如果已经装了2个戒子，再装，就脱掉戒子2
	//	if(jiezhi1 != NULL && jiezhi2 !=NULL)
	//	{
	//		ret.push_back(jiezhi2);
	//	}
	//	return ret;
	//}

	//// 平时的处理，直接1对1的替换
	//Equipment* temp = getEquipmentWhichEquipedAtPos(place[0],currentEquiped);

	//if (temp)
	//	ret.push_back(temp);

	//return ret;
}

Equipment* EquipmentManager::getEquipmentWhichEquipedAtPos(EquipRequirePlace place,vector<Equipment*> items)
{
	int num = items.size();

	for (int i=0;i<num;i++)
	{
		if ( items[i]->getActorId() > 0 && items[i]->getCurrentPlace() == place)
		{
			return items[i];
		}
	}
	return NULL;
}

vector<Equipment*> EquipmentManager::getEquipOnActor(unsigned int actorId)
{
	vector<Equipment*> mRet;

	int num = equipmentVec.size();

	for (int i=0;i<num;i++)
	{
		if (equipmentVec[i] -> getActorId() == actorId)
		{
			mRet.push_back(equipmentVec[i]);
		}
	}
	return mRet;
}


EquipmentManager::~EquipmentManager(void)
{
	removeAll();
}
