#include "MyselfManager.h"
#include "LFFileUtiles.h"
#include "include_json.h"
#include "WuPinManager.h"
#include "CS_Common.h"


MyselfManager* MyselfManager::manager = new MyselfManager();

MyselfManager::MyselfManager()
{
	jiaoTang.level = 0;
	next_vip_level = 0;
	next_need_charge = 0;
	b_first_charged = false;

	isDftRuntimeDataOk = false;
	isRuntimeDataOk = false;
}

MyselfManager::~MyselfManager(void)
{
	freeAllActor();
}

MyselfManager* MyselfManager::getManager()
{
	return manager;
}

void MyselfManager::reCreate()
{
	delete manager;
	EquipmentManager::getManager()->removeAll();
	WuPinManager::getManager()->clear();
	manager = new MyselfManager();
}

BaseActor* MyselfManager::getActorById(unsigned int _id)
{
	map<unsigned int , BaseActor* >::iterator l_it;; 
	l_it=allTeamActors.find(_id);
	if(l_it==allTeamActors.end())
	{
		return NULL;
	}
	return l_it->second;
}
bool MyselfManager::initManager(vector<BaseActorProperty> dataConfig,vector<ActorMapping> mappings)
{
	freeAllActor();
	mMyId = 0;
	isTeamDataInArenaOK = false;
	int num = dataConfig.size();
	for (int i=0;i<num;i++)
	{
		BaseActor* actor = new BaseActor(dataConfig[i]);
		allTeamActors.insert(pair<int,BaseActor*>(dataConfig[i].hero_id,actor));

		// 就是主英雄
		if (dataConfig[i].parent_hero_id == INVALID_ID)
		{
			CCAssert(mMyId == 0,"只能有一个主英雄");
			mMyId = dataConfig[i].hero_id;

			//// 初始化默认的runtime数据
			//VipPrivilege vip = CS::getVipPrivilege(dataConfig[i].vip_level);
			//RuntimeData runtimeData;
			//{
			//	runtimeData.buyTiLiTimeLeft = vip.MaxBuyTiLiTime;			//体力购买次数
			//	runtimeData.saoDangTimeLeft = vip.MaxSaoDangTime;			//副本扫荡次数
			//	runtimeData.buyArenaChallengeTimeLeft = vip.MaxBuyArenaChallengeTime;		//可购买竞技场挑战次数
			//	runtimeData.CanJumpGridNum = vip.JumpGridNum;				//关卡可跳过小怪数
			//	runtimeData.enableFastRongYuDuiHuan = vip.enableFastRongYuDuiHuan;			// 开启荣誉快速兑换功能
			//	runtimeData.freeRefreshTimeLeft = vip.freeRefreshTime;			// 免费刷新悬赏任务次数
			//}
			//setRuntionData(runtimeData);
		}
	}
	CCAssert(mMyId!=INVALID_ID,"");

	// 关联映射
	{
		BaseActor* zhenShen = NULL;
		BaseActor* yuanShen = NULL;
		for (int i=0,count = mappings.size(); i<count; i++)
		{
			zhenShen = getActorById(mappings[i].actorId);
			yuanShen = getActorById(mappings[i].yuanShenId);

			CCAssert(zhenShen!=NULL && yuanShen!=NULL,"");
			CCAssert(zhenShen != yuanShen,"");

			zhenShen->setYuanShen(yuanShen);
			yuanShen->setZhenShen(zhenShen);
		}
	}
	CCLog("MyselfManager::initManager ----> init over!");
	return true;
}

// 获取教堂的等级
unsigned int MyselfManager::getJiaoTangLevel()
{
	return jiaoTang.level;
}
// 获取教堂的数据
const JiaoTang& MyselfManager::getJiaoTangData()
{
	return jiaoTang;
}
// 是否可以洗礼actor
bool MyselfManager::canXiLiActor(const BaseActor* actor)
{
	// 15级之后才可以洗礼，教堂只能洗一次，洗了之后CurrentStage就上升了，那么当前等级的教堂不能洗了
	// 不能使元神
	if (actor->getCurrentStage() <= getJiaoTangLevel() && actor->getCurrentProperty().level >= 15 && !actor->isYuanShen())
	{
		return true;
	}
	return false;
}

void MyselfManager::setCurrentJiaoTangData(JiaoTang jiaotang)
{
	this->jiaoTang = jiaotang;
}

void MyselfManager::setRuntionData(RuntimeData data)
{
	runtimeData = data;
	isRuntimeDataOk = true;

	if (isTeamDataInArenaOK)
	{
		//runtimeData.buyArenaChallengeTimeLeft = teamDataInArena.buy_challenge_remainder_time;
		teamDataInArena.buy_challenge_remainder_time = runtimeData.buyArenaChallengeTimeLeft;
		CCLog("%s --> update buyArenaChallengeTimeLeft buy RuntimeData",__FUNCTION__);
	}
}
RuntimeData* MyselfManager::getRuntionData()
{
	CCAssert(isRuntimeDataOk,"");
	return &runtimeData;
}

void MyselfManager::setDefaultRuntimeData(RuntimeData data)
{
	isDftRuntimeDataOk = true;
	mDftRuntimeData = data;
}
RuntimeData* MyselfManager::getDefaultRuntimeData()
{
	CCAssert(isDftRuntimeDataOk,"");
	return &mDftRuntimeData;
}

unsigned int MyselfManager::getCurrentVipLevel()
{
	return MyselfManager::getManager()->getMyZhuJueData()->getCurrentProperty_R()->vip_level;
}

unsigned int MyselfManager::getNextVipLevel()
{
	return next_vip_level;
}
unsigned int MyselfManager::getNeedChargeToGetNextVip()
{
	return next_need_charge;
}
vector<ItemInfo> MyselfManager::getFirstChargePresent()
{
	return first_present_items;
}
vector<ChargePacketInfo> MyselfManager::getChargePacketInfo()
{
	return chargePakets;
}
bool MyselfManager::haveChargedBefore()
{
	return b_first_charged;
}

void MyselfManager::setNextVipLevel(unsigned int level)
{
	next_vip_level = level;
}
void MyselfManager::setNeedChargeToGetNextVip(unsigned int needCharge)
{
	next_need_charge = needCharge;
}
void MyselfManager::setFirstChargePresent(vector<ItemInfo> presents)
{
	first_present_items = presents;
}
void MyselfManager::setChargePacketInfo(vector<ChargePacketInfo> parkets)
{
	chargePakets = parkets;
}
void MyselfManager::setCharged(bool charged)
{
	b_first_charged = charged;
}



//void MyselfManager::refreshData(vector<BaseActorProperty> dataConfig)
//{
//	CCAssert(false,"先不要调用这个方法");
//	mMyId = 0;
//	int num = dataConfig.size();
//	for (int i=0;i<num;i++)
//	{
//		BaseActor* actor = getActorById(dataConfig[i].hero_id);
//		if (actor == NULL)
//		{
//			actor = new BaseActor(dataConfig[i]);
//			allTeamActors.insert(pair<int,BaseActor*>(dataConfig[i].hero_id,actor));
//		}else
//		{
//			actor->refreshBaseProPerty(dataConfig[i]);
//		}
//		// 就是主英雄
//		if (dataConfig[i].parent_hero_id == INVALID_ID)
//		{
//			mMyId = dataConfig[i].hero_id;
//		}
//	}
//
//	if (allTeamActors.size() != dataConfig.size())
//	{
//		CCAssert(false,"目前不能处理临时增加角色或减少角色的状况");
//	}
//
//	CCAssert(mMyId!=INVALID_ID,"");
//	CCLog("MyselfManager::initManager ----> init over!");
//}

void MyselfManager::addOneActor(BaseActorProperty config,unsigned int zhenshenIdIfIsYuanshen /* = 0 */)
{
	CCAssert(config.parent_hero_id != INVALID_ID,"只能是非主角");

	BaseActor* temp = getActorById(config.hero_id);
	if (temp != NULL)
	{
		// 已经存在了
		CCAssert(false,"");
		return;
	}

	BaseActor* actor = new BaseActor(config);
	allTeamActors.insert(pair<int,BaseActor*>(config.hero_id,actor));

	// 建立元神关联
	if (config.isYuanShen)
	{
		CCAssert(zhenshenIdIfIsYuanshen != 0,"");
		BaseActor* zhenshen = getActorById(zhenshenIdIfIsYuanshen);
		CCAssert(zhenshen!=NULL,"");
		CCAssert(zhenshen->getMyYuanShen()==NULL,"");
		zhenshen->setYuanShen(actor);
		actor->setZhenShen(zhenshen);
	}
}

void MyselfManager::freeAllActor()
{
	map<unsigned int ,BaseActor* >::iterator l_it;
	for (l_it = allTeamActors.begin();l_it!=allTeamActors.end();l_it++)
	{
		delete l_it->second;
	}
	allTeamActors.clear();
}

vector<BaseActor*> MyselfManager::getAllTeamActors(ShangZhenFilter_Actor filter,LockFilter_Actor filterLock,int actorType,bool expectZhuJue)
{
	vector<BaseActor*> tempRet;
	map<unsigned int,BaseActor*>::iterator it;
	for(it=allTeamActors.begin();it!=allTeamActors.end();++it)
	{
		BaseActor* actor = it->second;

		// 要锁住的，结果没有锁，就不要
		if (filterLock == LF_Locked && !actor->getCurrentProperty_R()->isLocked)
			continue;
		// 要没锁住，结果有锁，就不要
		if (filterLock == LF_UnLock && actor->getCurrentProperty_R()->isLocked)
			continue;

		// 要求是元神，那么不是元神就不要
		if(actorType == ACTOR_TYPE_YUAN_SHEN && !actor->isYuanShen())
			continue;
		// 要真身，如果是元神就不要
		if(actorType == ACTOR_TYPE_ZHEN_SHEN && actor->isYuanShen())
			continue;
		// 不要主角，那么是主角就放弃
		if (expectZhuJue && actor->getParId() == INVALID_ID)
			continue;
		// 需要上阵的，那么没有上阵就放弃
		if (filter == Actor_ShangZheng && actor->getChessBoardPos() == 0)
			continue;
		// 需要没上阵的，那么有上阵就放弃
		if (filter == Actor_WeiShangZheng && actor->getChessBoardPos() != 0)
			continue;

		tempRet.push_back(actor);
	}
	return tempRet;
}

vector<BaseActor*> MyselfManager::getTeamActors_ZhenShen(ShangZhenFilter_Actor filter /* = Actor_All */,LockFilter_Actor filterLock /* = LF_All */)
{
	return getAllTeamActors(filter,filterLock,ACTOR_TYPE_ZHEN_SHEN,false);
}

vector<BaseActor*> MyselfManager::getMyZhuShous_ZhenShen(ShangZhenFilter_Actor filter /* = Actor_All */,LockFilter_Actor filterLock /* = LF_All */)
{
	return getAllTeamActors(filter,filterLock,ACTOR_TYPE_ZHEN_SHEN,true);
}

int MyselfManager::getUnlockedHeroNumByProfession(int profession)
{
	int ret = 0;

	for (map<unsigned int,BaseActor*>::iterator it = allTeamActors.begin(); it!=allTeamActors.end(); it++)
	{
		if (!it->second->isLocked() || it->second->isYuanShen())
		{
			ret ++;
		}
	}
	return ret;
}

vector<BaseActor*> MyselfManager::getTeamActors_YuanShen(ShangZhenFilter_Actor filter /* = Actor_All */)
{
	//return getAllTeamActors(filter,ACTOR_TYPE_YUAN_SHEN,false);

	vector<BaseActor*> ret;
	vector<BaseActor*> zhenshen = getTeamActors_ZhenShen(filter);
	for (int i=0,count=zhenshen.size(); i<count; i++)
	{
		if (zhenshen[i]->getMyYuanShen() != NULL)
		{
			ret.push_back(zhenshen[i]->getMyYuanShen());
		}
	}

	return ret;

}
vector<BaseActor*> MyselfManager::getMyZhuShous_YuanShen(ShangZhenFilter_Actor filter /* = Actor_All */)
{
	vector<BaseActor*> ret;
	vector<BaseActor*> zhenshen = getMyZhuShous_ZhenShen(filter);
	for (int i=0,count=zhenshen.size(); i<count; i++)
	{
		if (zhenshen[i]->getMyYuanShen() != NULL)
		{
			ret.push_back(zhenshen[i]->getMyYuanShen());
		}
	}

	return ret;
}




vector<Equipment*> MyselfManager::getAllEquipmentsOnActor(unsigned int actorId, Filter_Equip filter,FilterEquipType filterType /* = Filter_Type_All */)
{
	return EquipmentManager::getManager()->getAllEquipmentsOnActor(actorId,filter,filterType);
}

vector<Equipment*> MyselfManager::equipmentFilter(vector<Equipment*> data,Filter_Equip filter,FilterEquipType filterType)
{
	return EquipmentManager::getManager()->equipmentFilter(data,filter,filterType);
}

vector<Equipment*> MyselfManager::getAllEquipments(Filter_Equip filter,FilterEquipType filterType /* = Type_All */)
{
	return EquipmentManager::getManager()->getAllEquipment(filter,filterType);
}

vector<Equipment*> MyselfManager::getAllEquipmentsWithOutEquipedOrChuanQi()
{
//	return EquipmentManager::getManager()->getAllEquipment(Filter_No_Equipped,Filter_Type_All);
	vector<Equipment*> temp = MyselfManager::getManager()->getAllEquipments(Filter_All);
	vector<Equipment*> vec;
	for (int i=0; i<temp.size(); ++i)
	{
		if (temp.at(i)->getActorId() == INVALID_ID && temp.at(i)->getEquipColor() != ZhuangbeiColour_Chuanqi)
			vec.push_back(temp.at(i));
	}
	return vec;
}


bool MyselfManager::getArenaTeamBasicInfo(ArenaTeamBasicInfo& outData)
{
	if (isTeamDataInArenaOK)
	{
		outData = teamDataInArena;
	}
	return isTeamDataInArenaOK;
}

ArenaTeamBasicInfo* MyselfManager::getEdiableArenaTeamBasicInfo()
{
	return isTeamDataInArenaOK?&teamDataInArena:NULL;
}
void MyselfManager::initArenaTeamBasicInfo(ArenaTeamBasicInfo data)
{
	teamDataInArena = data;
	isTeamDataInArenaOK = true;
	//CCAssert(isRuntimeDataOk,"这里是不确定的地方，如果先初始化的runtimeData，那么这里就要更新buyArenaChallengeTimeLeft，如果先初始化ArenaTeamBasicInfo，那么后面再修改剩余次数");
	// 直接用竞技场的数据
	if (isRuntimeDataOk)
	{
		runtimeData.buyArenaChallengeTimeLeft = data.buy_challenge_remainder_time;
		CCLog("%s --> update buyArenaChallengeTimeLeft buy teamDataInArena data",__FUNCTION__);
	}
}

BaseActor* MyselfManager::getMyZhuJueData()
{
	BaseActor* actor = getActorById(mMyId);
	if (actor != NULL)
	{
		CCAssert(actor->getParId() == INVALID_ID,"");
	}
	return actor;
}

void MyselfManager::removeEquipementFromOwner(Equipment* quip)
{
	if (quip->getActorId() == INVALID_ID)
	{
		CCLog("MyselfManager::removeEquipement ---> quip is not used so not need to remove");
		return;
	}
	BaseActor* actor = getMyTeamActor(quip->getActorId());
	if (actor)
	{
		CCLog("MyselfManager::removeEquipement ---> the equipment removed from owner!");
		actor->detachEuipment(quip);
	}else
	{
		CCAssert(false,"");
	}
}

void MyselfManager::attachEquipment(Equipment* quip,int forActorId)
{
	CCAssert(quip->getActorId() == INVALID_ID,"");

	BaseActor* actor = getMyTeamActor(forActorId);
	CCAssert(actor!=NULL,"");

	bool bNeedTakeOff = true;
	{// 是否必须脱掉一个装备
		vector<EquipRequirePlace> places = quip->getRequiredPlaces();
		for (int i=0;i<(int)places.size();i++)
		{
			vector<Equipment*> needTakeOff = actor->getNeedTakeOffIfWareOnEqui(quip->getType(),places[i]);
			if (needTakeOff.size() <= 0)
			{
				bNeedTakeOff = false;
				return;
			}
		}
	}
	CCAssert(!bNeedTakeOff,"不支持，必须单独调用脱掉函数，因为需要先联网脱掉！，所有这里就只支持添加和移除，不能包括换装！");

	// 添加新的装备
	actor->attachEquipment(quip);
	CCLog("MyselfManager::attachEquipment ---> OK!");
}

BaseActor* MyselfManager::getMyTeamActor(unsigned int actorId)
{
	return getActorById(actorId);
}

BaseActor* MyselfManager::getMyTeamZhenShenActorByPosIndex(unsigned int index)
{
	vector<BaseActor*> allData = getTeamActors_ZhenShen(Actor_ShangZheng);
	int num = allData.size();
	for (int i=0;i<num;i++)
	{
		if (allData[i]->getChessBoardPos() == index)
		{
			return allData[i];
		}
	}
	return NULL;
}
