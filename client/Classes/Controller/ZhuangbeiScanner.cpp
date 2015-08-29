#include "ZhuangbeiScanner.h"
#include "MyselfManager.h"
#include "EquipmentManager.h"
#include "PropertyCalculator.h"
#include "ServerTime.h"
#include "CS_Common.h"
#include "TimeLog.h"
#include "SkillManager.h"
#include "ScriptDataProvider.h"

//#define CALU_WAY 1		// 每次都重新算3个值
//#define CALU_WAY 2		// actor的初始值不用再算了
#define CALU_WAY 3	// actor的初始值不用再算了，每次只算一个值

//#define 

ZhuangbeiScanner* ZhuangbeiScanner::mInstance = NULL;

ZhuangbeiScanner::ZhuangbeiScanner(void)
{
	ignoreHint = false;
	isEquipPoolModifiedFromLastGenReport = false;
	mLastReport.timestamp = 0;
}

ZhuangbeiScanner* ZhuangbeiScanner::getScanner()
{
	if (mInstance == NULL)
	{
		mInstance = new ZhuangbeiScanner();
	}
	return mInstance;
}


void ZhuangbeiScanner::addOneEquipment(Equipment* equip)
{
	CCAssert(equip!=NULL,"");
	for (int i=0,count=equipmentToCompare.size(); i<count; i++)
	{
		if (equip == equipmentToCompare[i])
		{
			return;
		}
	}

	equipmentToCompare.push_back(equip);
	isEquipPoolModifiedFromLastGenReport = true;
	CCLog("%s --> Modified,",__FUNCTION__);
	LFLog("%s --> equipment{id = %lld,name = %s, hero_id = %u,pic = %s,color = %d}",
		__FUNCTION__,
		equip->getEquipId(),
		equip->getName().c_str(),
		equip->getActorId(),
		equip->getPicFile().c_str(),
		equip->getEquipColor());
}

void ZhuangbeiScanner::equipmentChanged(Equipment* equip)
{
	addOneEquipment(equip);
	// 设置为true
	isEquipPoolModifiedFromLastGenReport = true;

	CCLog("%s --> Modified",__FUNCTION__);
}

void ZhuangbeiScanner::removeOneEquipment(const Equipment* equip)
{
	vector<Equipment*>::iterator it = equipmentToCompare.begin();
	for (;it != equipmentToCompare.end(); )
	{
		if ((*it) == equip)
		{
			it = equipmentToCompare.erase(it);
			isEquipPoolModifiedFromLastGenReport = true;
			CCLog("%s --> Modified",__FUNCTION__);
			break;
		}
		it++;
	}

	// 已读中移除
	{
		map<BaseActor*,ReadedZhuanbei>::iterator it2 = readedEquips.begin();
		for (;it2 != readedEquips.end(); it2++)
		{
			it2->second.remove(equip);
		}
	}
	
	// 本来是差装备的移除
	{
		map<BaseActor*,ReadedZhuanbei>::iterator it3 = notGoodEquips.begin();
		for (;it3 != notGoodEquips.end(); it3++)
		{
			it3->second.remove(equip);
		}
	}

	// 本来就是好装备移除
	{
		map<BaseActor*,AlreadyGoodZhuangbei>::iterator it3 = goodEquipsByPrevGenReport.begin();
		for (;it3 != goodEquipsByPrevGenReport.end(); it3++)
		{
			it3->second.removeAll(equip);
		}
	}
}

void ZhuangbeiScanner::setMarkRead(BaseActor* actor,Equipment* equip)
{
	// 这里存在一个bug
	// 就是虽然我看了actor的戒子1的一个装备
	// 但是这个装备针对戒子2也是好的话，那么结果
	// 这一步之后，戒子二的也被忽略了


	map<BaseActor*,ReadedZhuanbei>::iterator it = readedEquips.find(actor);
	if (it == readedEquips.end()) // 没有
	{
		ReadedZhuanbei readed;
		readed.actor = actor;
		readed.add(equip);
		//readed.equips.push_back(equip);
		readedEquips.insert(pair<BaseActor*,ReadedZhuanbei>(actor,readed));
	}
	else
	{
		it->second.add(equip); // 插入
	}

	// 设置标记，用于自动重新生成报表
	isEquipPoolModifiedFromLastGenReport = true;
	CCLog("%s --> Modified",__FUNCTION__);

}

TestReport ZhuangbeiScanner::getTestReport()
{
	if (ignoreHint)
	{
		TestReport null_ret;
		return null_ret;
	}

	// 没有修改过，就直接返回上次的
	if (!isEquipPoolModifiedFromLastGenReport)
	{
		return mLastReport;
	}

	return genTestReport();
}

TestReport ZhuangbeiScanner::getTestReportByActor(BaseActor* actor)
{
	TestReport allReport = getTestReport();

	TestReport ret;
	ret.metaData = allReport.metaData;
	ret.timestamp = allReport.timestamp;
	for (int i=0,count=allReport.reportList.size(); i<count; i++)
	{
		if (allReport.reportList[i].actor == actor)
		{
			ret.reportList.push_back(allReport.reportList[i]);
			break;
		}
	}

	return ret;

}

void ZhuangbeiScanner::fillEquipsByCurrentPos(BaseActor* actor,vector<Equipment*>& equips)
{
	//TimeLog timeLog(__FUNCTION__);

	// 清空，并都先初始化为NULL
	equips.clear();
	for (int i=0;i<10;i++)
		equips.push_back(NULL);

	vector<Equipment*> temp = actor->getEquipments();
	for (int i=0,count=temp.size(); i<count; i++)
	{
		equips[temp[i]->getCurrentPlace()] = temp[i];
	}
}

vector<Equipment*> ZhuangbeiScanner::getCurentNotUsedEquipFromComparePool()
{
	vector<Equipment*> ret;

	for (int i=0,count=equipmentToCompare.size(); i<count; i++)
	{
		if (equipmentToCompare[i]->getActorId() == INVALID_ID)
		{
			ret.push_back(equipmentToCompare[i]);
		}
	}

	return ret;
}


ZhuangbeiScanner::FastCompearState ZhuangbeiScanner::fastCompear(BaseActor* actor,Equipment* newEquip,Equipment* oldEquip)
{
	// 戒子，护身符，对于夹主属性的，就直接判断
	if ( newEquip->getType() == ZhuangbeiType_Jiezhi || newEquip->getType() == ZhuangbeiType_Hushenfu)
	{
		AttrType actorAttr;
		AttrType equipAttr = newEquip->getMainProType();
		switch (actor->getCurrentProperty_R()->profession)
		{
		case Profession_Yemanren:
		case Profession_WuSeng:
		case Profession_Qishi:
			actorAttr = AttrType_liliang;
			break;
		case Profession_Chike:
		case Profession_ShenJianShou:
		case Profession_YouXia:
		case Profession_LieShou:
			actorAttr = AttrType_mingjie;
			break;
		case Profession_Fashi:
		case Profession_MiShu:
		case Profession_WuShi:
			actorAttr = AttrType_zhili;
			break;
		default:
			CCLog("Error:%s ---> profression = %d", __FUNCTION__,actor->getCurrentProperty_R()->profession);
			CCAssert(false,"");
			break;
		}

		// 体能不管
		if ( equipAttr == AttrType_zhili && actorAttr != equipAttr)
		{
			return FCS_Bad;
		}
		if ( equipAttr == AttrType_mingjie && actorAttr != equipAttr)
		{
			return FCS_Bad;
		}
		if ( equipAttr == AttrType_liliang && actorAttr != equipAttr)
		{
			return FCS_Bad;
		}

		// 下面就在看成色
	}

	if (oldEquip == NULL)
	{
		return FCS_Good;
	}

	//	如果是传奇装，且角色的对应槽位不是传奇装（即使装备武器类型不同），则提示为好装备。
	if (newEquip->getEquipColor() >= ZhuangbeiColour_Chuanqi && oldEquip->getEquipColor() < ZhuangbeiColour_Chuanqi)
	{
		return FCS_Good;
	}
	//	如果新装备成色 优于 旧装备，且新装备等级 不低于 旧装备，则提示为好装备。
	if (newEquip->getEquipColor() > oldEquip->getEquipColor() && newEquip->getLevel() >= oldEquip->getLevel())
	{
		return FCS_Good;
	}
	//	如果新装备成色 等于 旧装备，且新装备等级 高于 旧装备，则提示为好装备。
	if (newEquip->getEquipColor() == oldEquip->getEquipColor() && newEquip->getLevel() > oldEquip->getLevel())
	{
		return FCS_Good;
	}
	//	如果新装备成色 低于 旧装备，且新装备等级 不高于 旧装备，则抛弃。
	if (newEquip->getEquipColor() < oldEquip->getEquipColor() && newEquip->getLevel() <= oldEquip->getLevel())
	{
		return FCS_Bad;
	}
	//	如果新装备等级 等于 旧装备，且新装备成色 低于 旧装备，则抛弃
	if (newEquip->getEquipColor() < oldEquip->getEquipColor() && newEquip->getLevel() == oldEquip->getLevel())
	{
		return FCS_Bad;
	}

	return FCS_NotSure;
}

inline AlreadyGoodZhuangbei ZhuangbeiScanner::getAlreadyGoodZhuangbeiFromMapByActor(map<BaseActor*,AlreadyGoodZhuangbei>& data,BaseActor* actor)
{
	AlreadyGoodZhuangbei good;
	{
		map<BaseActor*,AlreadyGoodZhuangbei>::iterator it = data.find(actor);
		if (it != data.end())
		{
			good = it->second;
		}else
		{
			good.actor = actor;
			data.insert(pair<BaseActor*,AlreadyGoodZhuangbei>(actor,good));
		}
	}

	return good;
}
inline ReadedZhuanbei ZhuangbeiScanner::getReadedZhuanbeiFromMapByActor(map<BaseActor*,ReadedZhuanbei>& data,BaseActor* actor)
{
	ReadedZhuanbei mark;
	{
		map<BaseActor*,ReadedZhuanbei>::iterator it = data.find(actor);
		if (it != data.end())
		{
			mark = it->second;
		}else
		{
			mark.actor = actor;
			data.insert(pair<BaseActor*,ReadedZhuanbei>(actor,mark));
		}
	}

	return mark;
}

TestReport ZhuangbeiScanner::genTestReport()
{
#define WANT_PRO_HP					1
#define WANT_PRO_ChengShouLi		2
#define WANT_PRO_GongJi				3
	CCLog("");
	CCLog("");
	TimeLog timeLog(__FUNCTION__);

	TestReport report;
	report.timestamp = ServerTime::getTime();

#if CALU_WAY == 1
	report.metaData = RM_AllDataIsOk;
#elif CALU_WAY == 2
	report.metaData = RM_AllDataIsOk;
#elif CALU_WAY == 3
	report.metaData = RM_OnlyByWhyIsOk;
#else
#error "report.metaData Error"
#endif

	vector<BaseActor*> allShangZhenZhenShen = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	vector<Equipment*> allNotUsedEquipment = getCurentNotUsedEquipFromComparePool();

	vector<Equipment*> equipsOnActorForFastAccess;

	for (int actorIndex = 0,actorNum = allShangZhenZhenShen.size(); actorIndex < actorNum; actorIndex++)
	{
		TimeLog timeLogFor("TimeLog:For One Actor Loop");

		ReadedZhuanbei readedMarkForThisActor = getReadedZhuanbeiFromMapByActor(readedEquips,allShangZhenZhenShen[actorIndex]);
		ReadedZhuanbei notGoodForThisActor = getReadedZhuanbeiFromMapByActor(notGoodEquips,allShangZhenZhenShen[actorIndex]);
		AlreadyGoodZhuangbei alreadyGoodEquipsForThisActor = getAlreadyGoodZhuangbeiFromMapByActor(goodEquipsByPrevGenReport,allShangZhenZhenShen[actorIndex]);

		LFLog("%s --> go to find equipment for actor{hero_id = %u,profression = %d}",
			__FUNCTION__,
			allShangZhenZhenShen[actorIndex]->getCurrentProperty().hero_id,
			allShangZhenZhenShen[actorIndex]->getCurrentProperty().profession);

		TestItem itemRecord;
		itemRecord.actor = allShangZhenZhenShen[actorIndex];

		// 一个角色只算一次
#if CALU_WAY == 2 || CALU_WAY == 3
		PropertyCalculator mCalulator;
		CalculateData data = allShangZhenZhenShen[actorIndex]->getCalculateData();
		data.includeCalDatas.clear(); // 不算元神
		int initHp = mCalulator.calculateHp(data);
		int initGongjili = mCalulator.calculateGongJiLi_MacroValue(data,Range_Average);
		int initHujia = mCalulator.calculateChengShouLi(data);
#endif
		// 
		fillEquipsByCurrentPos(allShangZhenZhenShen[actorIndex],equipsOnActorForFastAccess);

		for (int equipPoolIndex=0,poolCount=allNotUsedEquipment.size(); equipPoolIndex<poolCount; equipPoolIndex++)
		{
			LFLog("%s --> test equipment{id = %lld,name = %s,pic = %s,color = %d}",
				__FUNCTION__,
				allNotUsedEquipment[equipPoolIndex]->getEquipId(),
				allNotUsedEquipment[equipPoolIndex]->getName().c_str(),
				allNotUsedEquipment[equipPoolIndex]->getPicFile().c_str(),
				allNotUsedEquipment[equipPoolIndex]->getEquipColor());

			if (readedMarkForThisActor.contains(allNotUsedEquipment[equipPoolIndex]))
			{
				LFLog("%s --> euip is marked by this actor,throw!",
					__FUNCTION__);
				continue;
			}

			if (notGoodForThisActor.contains(allNotUsedEquipment[equipPoolIndex]))
			{
				LFLog("%s --> euip is not good by previous compear,throw!",
					__FUNCTION__);
				continue;
			}
			// 不需要以前的了，懒得以后添加时做无用功
			notGoodForThisActor.equips.clear();


			// 是不是根据以前的比较来看已经是好装备了
			// 要在readedMarkForThisActor之后判断
			if (allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_Jiezhi &&
				alreadyGoodEquipsForThisActor.contains(allNotUsedEquipment[equipPoolIndex],POS_JieZhi1))
			{
				LFLog("%s --> euip is alreadyGood good by previous compear,save!",__FUNCTION__);
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[POS_JieZhi1]);
				itemRecord.comparedPlace.push_back(POS_JieZhi1);
				itemRecord.hpChangeVec.push_back(0);
				itemRecord.chengShouLiChangeVec.push_back(0);
				itemRecord.gongJiChangeVec.push_back(0);
				itemRecord.reasonVec.push_back(Because_LastCompare);
				continue;
			}
			if (allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_Jiezhi &&
				alreadyGoodEquipsForThisActor.contains(allNotUsedEquipment[equipPoolIndex],POS_JieZhi2))
			{
				LFLog("%s --> euip is alreadyGood good by previous compear,save!",__FUNCTION__);
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[POS_JieZhi2]);
				itemRecord.comparedPlace.push_back(POS_JieZhi2);
				itemRecord.hpChangeVec.push_back(0);
				itemRecord.chengShouLiChangeVec.push_back(0);
				itemRecord.gongJiChangeVec.push_back(0);
				itemRecord.reasonVec.push_back(Because_LastCompare);
				continue;
			}
			if (alreadyGoodEquipsForThisActor.contains(allNotUsedEquipment[equipPoolIndex],
				allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]))
			{
				int place = allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0];
				LFLog("%s --> euip is alreadyGood good by previous compear,save!",__FUNCTION__);
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[place]);
				itemRecord.comparedPlace.push_back(place);
				itemRecord.hpChangeVec.push_back(0);
				itemRecord.chengShouLiChangeVec.push_back(0);
				itemRecord.gongJiChangeVec.push_back(0);
				itemRecord.reasonVec.push_back(Because_LastCompare);
				continue;
			}
			// 下面的不需要了，直接清空
			// 省得添加的时候做无用功
			alreadyGoodEquipsForThisActor.new_old_equips.clear();


			if (!isNecessaryToCompare(allShangZhenZhenShen[actorIndex],NULL,allNotUsedEquipment[equipPoolIndex]))
			{
				LFLog("%s --> is not NecessaryToCompare!",__FUNCTION__);
				continue;
			}

			if (!isNeccessaryToCompearByCurrentEquipsOnHero(allNotUsedEquipment[equipPoolIndex],equipsOnActorForFastAccess))
			{
				LFLog("%s --> is not NeccessaryToCompearByCurrentEquipsOnHero!",__FUNCTION__);
				continue;
			}

			// 快速比较
			//  如果是传奇装，且角色的对应槽位不是传奇装（即使装备武器类型不同），则提示为好装备。
			//	如果新装备成色 优于 旧装备，且新装备等级 不低于 旧装备，则提示为好装备。
			//	如果新装备成色 等于 旧装备，且新装备等级 高于 旧装备，则提示为好装备。
			//	如果新装备成色 低于 旧装备，且新装备等级 不高于 旧装备，则抛弃。
			//	如果新装备等级 等于 旧装备，且新装备成色 低于 旧装备，则抛弃
			if (allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_Jiezhi)
			{
				int bad_for_place_num = 0;
				ZhuangbeiScanner::FastCompearState state = fastCompear(allShangZhenZhenShen[actorIndex],allNotUsedEquipment[equipPoolIndex],equipsOnActorForFastAccess[POS_JieZhi1]);
				if (state == FCS_Good)
				{
					itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
					itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[POS_JieZhi1]);
					itemRecord.comparedPlace.push_back(POS_JieZhi1);
					itemRecord.hpChangeVec.push_back(0);
					itemRecord.chengShouLiChangeVec.push_back(0);
					itemRecord.gongJiChangeVec.push_back(0);
					itemRecord.reasonVec.push_back(Because_FastCompare);
					LFLog("%s --> good jie zi1 by fast compear!",__FUNCTION__);
				}
				else if(state == FCS_Bad)
				{
					LFLog("%s --> bad jie zi1 by fast compear!",__FUNCTION__);
					bad_for_place_num ++;
				}
				else
				{
					LFLog("%s --> not sure is jie zi1 good?? by fast compear!",__FUNCTION__);
				}

				state = fastCompear(allShangZhenZhenShen[actorIndex],allNotUsedEquipment[equipPoolIndex],equipsOnActorForFastAccess[POS_JieZhi2]);
				if (state == FCS_Good)
				{
					itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
					itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[POS_JieZhi2]);
					itemRecord.comparedPlace.push_back(POS_JieZhi2);
					itemRecord.hpChangeVec.push_back(0);
					itemRecord.chengShouLiChangeVec.push_back(0);
					itemRecord.gongJiChangeVec.push_back(0);
					itemRecord.reasonVec.push_back(Because_FastCompare);
					LFLog("%s --> good jie zi2 by fast compear!",__FUNCTION__);
				}
				else if(state == FCS_Bad)
				{
					LFLog("%s --> bad jie zi2 by fast compear!",__FUNCTION__);
					bad_for_place_num ++;
				}
				else
				{
					LFLog("%s --> not sure is jie zi2 good?? by fast compear!",__FUNCTION__);
				}

				// 对于两个戒子位置都是差的，那么就标记，下次不用比较了
				if (bad_for_place_num == 2)
				{
					notGoodForThisActor.equips.insert(pair<Equipment*,bool>(allNotUsedEquipment[equipPoolIndex],true));
				}

				continue;
			}
			else
			{
				ZhuangbeiScanner::FastCompearState state = fastCompear(allShangZhenZhenShen[actorIndex],allNotUsedEquipment[equipPoolIndex],equipsOnActorForFastAccess[allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]]);
				if (state == FCS_Good)
				{
					int place = allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0];
					itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
					itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]]);
					itemRecord.comparedPlace.push_back(place);
					itemRecord.hpChangeVec.push_back(0);
					itemRecord.chengShouLiChangeVec.push_back(0);
					itemRecord.gongJiChangeVec.push_back(0);
					itemRecord.reasonVec.push_back(Because_FastCompare);
					LFLog("%s --> good equip by fast compear!",__FUNCTION__);
					continue;
				}
				else if(state == FCS_Bad)
				{
					LFLog("%s --> bad equip by fast compear!",__FUNCTION__);
					notGoodForThisActor.equips.insert(pair<Equipment*,bool>(allNotUsedEquipment[equipPoolIndex],true));
					continue;
				}
				else
				{
					LFLog("%s --> not sure is equip good?? by fast compear!",__FUNCTION__);
				}
			}


			int hpChange;
			int huJiaChange;
			int gongJiLiChange;

			// 戒子真他妈特殊，必须先看有没有戒子了，然后指定到底是要换，还是要加
			if (allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_Jiezhi)
			{
				// 不管有没有带戒子，直接挨着脱1号位，2号位即可
				// 1. 先脱戒子1测试
#if CALU_WAY == 1
				getPropertyChangeData(allShangZhenZhenShen[actorIndex],
					allNotUsedEquipment[equipPoolIndex],
					equipsOnActorForFastAccess[POS_JieZhi1],
					hpChange,huJiaChange,gongJiLiChange);
#elif  CALU_WAY == 2
				getPropertyChangeDataByInitedData(allShangZhenZhenShen[actorIndex],initHp,initHujia,initGongjili,
					allNotUsedEquipment[equipPoolIndex],
					equipsOnActorForFastAccess[POS_JieZhi1],
					hpChange,huJiaChange,gongJiLiChange);
#elif  CALU_WAY == 3
				huJiaChange = 0;
				hpChange = 0;
				gongJiLiChange = 0;
				getPropertyChangeDataByInitedData2(allShangZhenZhenShen[actorIndex],VT_GONGJI,initGongjili,
					allNotUsedEquipment[equipPoolIndex],
					equipsOnActorForFastAccess[POS_JieZhi1],
					gongJiLiChange);
#else
#endif
				LFLog("%s -->we must take off one jiezi [jiezi1] to test ==> hpChange = %d,chengShouLiChange = %d,gongJiLiChange = %d",
					__FUNCTION__,
					hpChange,huJiaChange,gongJiLiChange);
				bool alreadyGood = false;
				// 只比攻击
				if (gongJiLiChange > 0)
				{
					itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
					itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[POS_JieZhi1]);
					itemRecord.comparedPlace.push_back(POS_JieZhi1);
					itemRecord.hpChangeVec.push_back(hpChange);
					itemRecord.chengShouLiChangeVec.push_back(huJiaChange);
					itemRecord.gongJiChangeVec.push_back(gongJiLiChange);
					itemRecord.reasonVec.push_back(Because_GongJi);
					alreadyGood = true;
					LFLog("%s --> good jie zi if wear at pos1 ! after compare!",__FUNCTION__);
				}


				// 2. 再脱戒子2测试
#if CALU_WAY == 1
				getPropertyChangeData(allShangZhenZhenShen[actorIndex],
					allNotUsedEquipment[equipPoolIndex],
					equipsOnActorForFastAccess[POS_JieZhi2],
					hpChange,huJiaChange,gongJiLiChange);
#elif  CALU_WAY == 2
				getPropertyChangeDataByInitedData(allShangZhenZhenShen[actorIndex],initHp,initHujia,initGongjili,
					allNotUsedEquipment[equipPoolIndex],
					equipsOnActorForFastAccess[POS_JieZhi2],
					hpChange,huJiaChange,gongJiLiChange);
#elif  CALU_WAY == 3
				huJiaChange = 0;
				hpChange = 0;
				gongJiLiChange = 0;
				getPropertyChangeDataByInitedData2(allShangZhenZhenShen[actorIndex],VT_GONGJI,initGongjili,
					allNotUsedEquipment[equipPoolIndex],
					equipsOnActorForFastAccess[POS_JieZhi2],
					gongJiLiChange);
#else
#endif
				LFLog("%s -->we must take off one jiezi [jiezi2] to test ==> hpChange = %d,chengShouLiChange = %d,gongJiLiChange = %d",
					__FUNCTION__,
					hpChange,huJiaChange,gongJiLiChange);

				// 只比攻击
				if (gongJiLiChange > 0)
				{
					itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
					itemRecord.oldEquipVec.push_back(equipsOnActorForFastAccess[POS_JieZhi2]);
					itemRecord.comparedPlace.push_back(POS_JieZhi2);
					itemRecord.hpChangeVec.push_back(hpChange);
					itemRecord.chengShouLiChangeVec.push_back(huJiaChange);
					itemRecord.gongJiChangeVec.push_back(gongJiLiChange);
					itemRecord.reasonVec.push_back(Because_GongJi);

					alreadyGood = true;
					LFLog("%s --> good jie zi if wear at pos2 ! after compare!",__FUNCTION__);
				}

				if (!alreadyGood)
				{
					notGoodForThisActor.equips.insert(pair<Equipment*,bool>(allNotUsedEquipment[equipPoolIndex],true));
					LFLog("%s : not a good jiezi!",__FUNCTION__);
				}
				continue;
			}




			// 下面是非戒子的情况
			int propertyWeWant = WANT_PRO_HP;
			if (allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_Dunpai ||
				allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_KuijiaQin ||
				allNotUsedEquipment[equipPoolIndex]->getType() == ZhuangbeiType_KuijiaZhong)
			{
				propertyWeWant = WANT_PRO_ChengShouLi;
			}
			else
			{
				propertyWeWant = WANT_PRO_GongJi;
			}

			Equipment* equipOld = NULL;
			switch (allNotUsedEquipment[equipPoolIndex]->getType())
			{
			case ZhuangbeiType_DanshouJian:
				equipOld = equipsOnActorForFastAccess[POS_ZhuWuQi];
				break;
			case ZhuangbeiType_ShuangshouBishou:
				equipOld = equipsOnActorForFastAccess[POS_ZhuWuQi];
				break;
			case ZhuangbeiType_ShuangshouFazhang:
				equipOld = equipsOnActorForFastAccess[POS_ZhuWuQi];
				break;
			case ZhuangbeiType_ShuangshouFu:
				equipOld = equipsOnActorForFastAccess[POS_ZhuWuQi];
				break;
			case ZhuangbeiType_DanshouFazhang:
				equipOld = equipsOnActorForFastAccess[POS_ZhuWuQi];
				break;
			case ZhuangbeiType_Gong:
				equipOld = equipsOnActorForFastAccess[POS_ZhuWuQi];
				break;
				// 对副手武器来说，来说如果主武器是双手装，那么也不能比较
			case ZhuangbeiType_Faqi:
			case ZhuangbeiType_Dunpai:
				equipOld = equipsOnActorForFastAccess[POS_FuShou];
				break;

			// 这些都是一一对应的，不用特殊处理
			case ZhuangbeiType_Hushenfu:
				equipOld = equipsOnActorForFastAccess[POS_Hushenfu];
				break;
			case ZhuangbeiType_KuijiaZhong:
			case ZhuangbeiType_KuijiaQin:
				equipOld = equipsOnActorForFastAccess[POS_XiongJia];
				break;
			default:
				CCLog("Error : %s ---> unsuported zhuangbeiType [%d]",__FUNCTION__,allNotUsedEquipment[equipPoolIndex]->getType());
				CCAssert(false,"不支持的装备类型");
				break;
			}


#if CALU_WAY == 1
			getPropertyChangeData(allShangZhenZhenShen[actorIndex],
				allNotUsedEquipment[equipPoolIndex],
				equipOld,
				hpChange,huJiaChange,gongJiLiChange);
#elif  CALU_WAY == 2
			getPropertyChangeDataByInitedData(allShangZhenZhenShen[actorIndex],initHp,initHujia,initGongjili,
				allNotUsedEquipment[equipPoolIndex],
				equipOld,
				hpChange,huJiaChange,gongJiLiChange);
#elif  CALU_WAY == 3
			// 传NULL就会自动脱装的
			if (propertyWeWant == WANT_PRO_GongJi)
			{
				huJiaChange = 0;
				hpChange = 0;
				gongJiLiChange = 0;
				getPropertyChangeDataByInitedData2(allShangZhenZhenShen[actorIndex],VT_GONGJI,initGongjili,
					allNotUsedEquipment[equipPoolIndex],
					equipOld,
					gongJiLiChange);
			}
			else if (propertyWeWant == WANT_PRO_ChengShouLi)
			{
				huJiaChange = 0;
				hpChange = 0;
				gongJiLiChange = 0;
				getPropertyChangeDataByInitedData2(allShangZhenZhenShen[actorIndex],VT_CHENGSHOULI,initHujia,
					allNotUsedEquipment[equipPoolIndex],
					equipOld,
					huJiaChange);
			}
			else if(propertyWeWant == WANT_PRO_HP)
			{
				huJiaChange = 0;
				hpChange = 0;
				gongJiLiChange = 0;
				getPropertyChangeDataByInitedData2(allShangZhenZhenShen[actorIndex],VT_HP,initHp,
					allNotUsedEquipment[equipPoolIndex],
					equipOld,
					hpChange);
			}
#else
#endif

			LFLog("%s --> hpChange = %d,chengShouLiChange = %d,gongJiLiChange = %d",
				__FUNCTION__,
				hpChange,huJiaChange,gongJiLiChange);


			// 还要比较类型
			if (propertyWeWant == WANT_PRO_GongJi && gongJiLiChange > 0)
			{
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipOld);
				itemRecord.comparedPlace.push_back(allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]);
				itemRecord.hpChangeVec.push_back(hpChange);
				itemRecord.chengShouLiChangeVec.push_back(huJiaChange);
				itemRecord.gongJiChangeVec.push_back(gongJiLiChange);
				itemRecord.reasonVec.push_back(Because_GongJi);

				LFLog("%s --> good equipment after compare gongjiLiChange!",__FUNCTION__);
			}
			else if(propertyWeWant == WANT_PRO_ChengShouLi && huJiaChange > 0)
			{
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipOld);
				itemRecord.comparedPlace.push_back(allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]);
				itemRecord.hpChangeVec.push_back(hpChange);
				itemRecord.chengShouLiChangeVec.push_back(huJiaChange);
				itemRecord.gongJiChangeVec.push_back(gongJiLiChange);
				itemRecord.reasonVec.push_back(Because_HuaJia);

				LFLog("%s --> good equipment after compare chenShouLiChange!",__FUNCTION__);
			}
			else if(propertyWeWant == WANT_PRO_HP && hpChange > 0)
			{
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipOld);
				itemRecord.comparedPlace.push_back(allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]);
				itemRecord.hpChangeVec.push_back(hpChange);
				itemRecord.chengShouLiChangeVec.push_back(huJiaChange);
				itemRecord.gongJiChangeVec.push_back(gongJiLiChange);
				itemRecord.reasonVec.push_back(Because_HP);

				LFLog("%s --> good equipment after compare hpChange!",__FUNCTION__);
			}
			else if (
				equipOld != NULL && 
				isNecessaryToCompare(allShangZhenZhenShen[actorIndex],equipOld,allNotUsedEquipment[equipPoolIndex]) &&
				equipOld->getEquipColor() < ZhuangbeiColour_Chuanqi && 
				allNotUsedEquipment[equipPoolIndex]->getEquipColor() >= ZhuangbeiColour_Chuanqi)
			{// 原来的地方不是传奇，现在有传奇了，必提示
				itemRecord.betterEquipVec.push_back(allNotUsedEquipment[equipPoolIndex]);
				itemRecord.oldEquipVec.push_back(equipOld);
				itemRecord.comparedPlace.push_back(allNotUsedEquipment[equipPoolIndex]->getRequiredPlaces()[0]);
				itemRecord.hpChangeVec.push_back(hpChange);
				itemRecord.chengShouLiChangeVec.push_back(huJiaChange);
				itemRecord.gongJiChangeVec.push_back(gongJiLiChange);
				itemRecord.reasonVec.push_back(Because_Color);

				LFLog("%s --> Good Color,keep It!",__FUNCTION__);
			}
			else
			{// 不算好装备
				notGoodForThisActor.equips.insert(pair<Equipment*,bool>(allNotUsedEquipment[equipPoolIndex],true));
				LFLog("%s --> not a good equipment after compare,throw!",__FUNCTION__);
			}
		}

		// 存放
		if (!itemRecord.betterEquipVec.empty())
		{
			LFLog("%s --> save one recored item!",__FUNCTION__);
			report.reportList.push_back(itemRecord);
		}
		// 记录好的
		{
			map<BaseActor*,AlreadyGoodZhuangbei>::iterator it = goodEquipsByPrevGenReport.find(allShangZhenZhenShen[actorIndex]);
			CCAssert(it != goodEquipsByPrevGenReport.end(),"前面一定是插入了的，这里肯定能找到");
			for (int goodIndex=0,goodCount=itemRecord.betterEquipVec.size(); goodIndex<goodCount; goodIndex++)
			{
				it->second.add(itemRecord.betterEquipVec[goodIndex],itemRecord.comparedPlace[goodIndex]); // 添加
			}
		}
		
		// 记录差的
		if (!notGoodForThisActor.equips.empty())
		{
			map<BaseActor*,ReadedZhuanbei>::iterator it = notGoodEquips.find(allShangZhenZhenShen[actorIndex]);
			CCAssert(it != notGoodEquips.end(),"前面一定是插入了的，这里肯定能找到");
			it->second.add(notGoodForThisActor.equips); // 添加
		}
	}

	// 
	isEquipPoolModifiedFromLastGenReport = false;

	// 不能清空
	// 因为设置了标记之后就会重新gen的，清空了就没有过滤了
	//// 标记已读的清空
	//readedEquips.clear();

	// 刷新值
	mLastReport = report;
	return report;
}

bool ZhuangbeiScanner::isNeccessaryToCompearByCurrentEquipsOnHero(Equipment* newEquip,vector<Equipment*> currentEquipOnHero)
{
	bool ret = true;

	// 传奇都有必要
	if (newEquip->getEquipColor() >= ZhuangbeiColour_Chuanqi)
	{
		return ret;
	}

	Equipment* equipOld = NULL;
	switch (newEquip->getType())
	{
	case ZhuangbeiType_DanshouJian:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && currentEquipOnHero[POS_ZhuWuQi]->getType() != ZhuangbeiType_DanshouJian)
		{//
			LFLog("%s --> ZhuangbeiType_DanshouJian, but there is one equip on POS_ZhuWuQi,and is not ZhuangbeiType_DanshouJian ,throw!",__FUNCTION__);
			ret = false;
		}
		break;
	case ZhuangbeiType_ShuangshouBishou:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && currentEquipOnHero[POS_ZhuWuQi]->getType() != ZhuangbeiType_ShuangshouBishou)
		{//
			LFLog("%s --> ZhuangbeiType_ShuangshouBishou, but there is one equip on POS_ZhuWuQi,and is not ZhuangbeiType_ShuangshouBishou ,throw!",__FUNCTION__);
			ret = false;
		}
		break;
	case ZhuangbeiType_ShuangshouFazhang:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && currentEquipOnHero[POS_ZhuWuQi]->getType() != ZhuangbeiType_ShuangshouFazhang)
		{//
			LFLog("%s --> ZhuangbeiType_ShuangshouFazhang, but there is one equip on POS_ZhuWuQi,and is not ZhuangbeiType_ShuangshouFazhang ,throw!",__FUNCTION__);
			ret = false;
		}
		break;
	case ZhuangbeiType_ShuangshouFu:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && currentEquipOnHero[POS_ZhuWuQi]->getType() != ZhuangbeiType_ShuangshouFu)
		{//
			LFLog("%s --> ZhuangbeiType_ShuangshouFu, but there is one equip on POS_ZhuWuQi,and is not ZhuangbeiType_ShuangshouFu ,throw!",__FUNCTION__);
			ret = false;
		}
		break;
	case ZhuangbeiType_DanshouFazhang:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && currentEquipOnHero[POS_ZhuWuQi]->getType() != ZhuangbeiType_DanshouFazhang)
		{//
			LFLog("%s --> ZhuangbeiType_DanshouFazhang, but there is one equip on POS_ZhuWuQi,and is not ZhuangbeiType_DanshouFazhang ,throw!",__FUNCTION__);
			ret = false;
		}
		break;
	case ZhuangbeiType_Gong:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && currentEquipOnHero[POS_ZhuWuQi]->getType() != ZhuangbeiType_Gong)
		{//
			LFLog("%s --> ZhuangbeiType_Gong, but there is one equip on POS_ZhuWuQi,and is not ZhuangbeiType_Gong ,throw!",__FUNCTION__);
			ret = false;
		}
		break;
		// 对副手武器来说，来说如果主武器是双手装，那么也不能比较
	case ZhuangbeiType_Faqi:
	case ZhuangbeiType_Dunpai:
		if (currentEquipOnHero[POS_ZhuWuQi] !=NULL && CS::isShuangShouWuQi(currentEquipOnHero[POS_ZhuWuQi]->getType()))
		{//
			LFLog("%s --> ZhuangbeiType_Faqi||ZhuangbeiType_Dunpai, but there is one equip on POS_ZhuWuQi,and is not danshou wuqi,throw!",__FUNCTION__);
			ret = false;
		}
		break;
		// 这些都是一一对应的，不用特殊处理
	case ZhuangbeiType_Hushenfu:
		break;
	case ZhuangbeiType_KuijiaZhong:
	case ZhuangbeiType_KuijiaQin:
		break;
	case ZhuangbeiType_Jiezhi:
		break;
	default:
		CCLog("Error : %s ---> unsuported zhuangbeiType [%d]",__FUNCTION__,equipOld->getType());
		CCAssert(false,"不支持的装备类型");
		break;
	}

	return ret;
}

bool ZhuangbeiScanner::isNecessaryToCompare(BaseActor* actor,Equipment* oldEquip,Equipment* newEquip)
{
	if (!CS::canEquipToHero(newEquip->getType(),actor->getCurrentProperty_R()->profession))
	{
		return false;
	}

	// 职业绑定
	if (newEquip->getEquipData().profession_id != 0 &&
		newEquip->getEquipData().profession_id != actor->getCurrentProperty_R()->profession)
	{
		return false;
	}

	if (newEquip == oldEquip ||// 相同装备直接返回
		newEquip->getActorId() == actor->getId() || //本来自己身上，直接返回
		newEquip->getActorId() != 0 || // 在别人身上，直接返回
		!EquipmentManager::getManager()->canUseEquip(newEquip->getType(),actor->getActorType()) //|| // 不能装这个装备直接返回
//		newEquip->isInFengYin() //||	// 还没解封
		//newEquip->getLevel() > actor->getCurrentProperty().level // 等级不足
		)
	{
		return false;
	}
	return true;
}

void ZhuangbeiScanner::getPropertyChangeData(BaseActor* actor,Equipment* testEquip,Equipment* oldEquip,int& out_hpChange,int& out_huJiaChange,int& out_gongJiLiChange)
{
	CCAssert(false,"不支持了");
	//TimeLog timeLog(__FUNCTION__);

	CCAssert(actor!=NULL && testEquip!=NULL,"");
	if (oldEquip != NULL)
		CCAssert(oldEquip->getActorId() == actor->getId(),"");

	out_hpChange = 0;
	out_huJiaChange = 0;
	out_gongJiLiChange = 0;

	
	EquipmentManager* equipManager = EquipmentManager::getManager();

	if (!isNecessaryToCompare(actor,oldEquip,testEquip))
	{
		return;
	}
	
	PropertyCalculator calculator;

	// 换上当前装备的数据
	CalculateData calculatedata = actor->getCalculateDataWhenEquipOne(testEquip,oldEquip);
	//
	int newHp = calculator.calculateHp(calculatedata);
	int newGongjili = calculator.calculateGongJiLi_MacroValue(calculatedata,Range_Average);
	int newHujia = calculator.calculateChengShouLi(calculatedata);

	//
	int curHp = actor->getXueliang();
	int curGongjili = actor->getGongji_MacroValue();
	//int curHujia = actor->getHujia();
	int curHujia = calculator.calculateChengShouLi(actor->getCalculateData());

	out_hpChange = newHp -curHp;
	out_huJiaChange = newHujia - curHujia;
	out_gongJiLiChange = newGongjili - curGongjili;
}

void ZhuangbeiScanner::getPropertyChangeDataByInitedData(BaseActor* actor,int initHp,int initHujia,int initGongji,Equipment* testEquip,Equipment* oldEquip,int& out_hpChange,int& out_huJiaChange,int& out_gongJiLiChange)
{
	//TimeLog timeLog(__FUNCTION__);

	CCAssert(actor!=NULL && testEquip!=NULL,"");
	if (oldEquip != NULL)
		CCAssert(oldEquip->getActorId() == actor->getId(),"");

	out_hpChange = 0;
	out_huJiaChange = 0;
	out_gongJiLiChange = 0;


	EquipmentManager* equipManager = EquipmentManager::getManager();

	if (!isNecessaryToCompare(actor,oldEquip,testEquip))
	{
		return;
	}

	PropertyCalculator calculator;

	// 换上当前装备的数据
	CalculateData calculatedata = actor->getCalculateDataWhenEquipOne(testEquip,oldEquip);

	calculatedata.includeCalDatas.clear(); // 不算元神

	//
	int newHp = calculator.calculateHp(calculatedata);
	int newGongjili = calculator.calculateGongJiLi_MacroValue(calculatedata,Range_Average);
	int newHujia = calculator.calculateChengShouLi(calculatedata);


	//
	int curHp = initHp;
	int curGongjili = initGongji;
	int curHujia = initHujia;

	out_hpChange = newHp -curHp;
	out_huJiaChange = newHujia - curHujia;
	out_gongJiLiChange = newGongjili - curGongjili;
}


void ZhuangbeiScanner::getPropertyChangeDataByInitedData2(BaseActor* actor,ValueType valueType,int initValue,Equipment* testEquip,Equipment* oldEquip,int& out_change)
{
	//TimeLog timeLog(__FUNCTION__);

	CCAssert(actor!=NULL && testEquip!=NULL,"");
	if (oldEquip != NULL)
		CCAssert(oldEquip->getActorId() == actor->getId(),"");

	out_change = 0;

	EquipmentManager* equipManager = EquipmentManager::getManager();

	if (!isNecessaryToCompare(actor,oldEquip,testEquip))
	{
		return;
	}

	PropertyCalculator calculator;

	// 换上当前装备的数据
	CalculateData calculatedata = actor->getCalculateDataWhenEquipOne(testEquip,oldEquip);
	//
	calculatedata.includeCalDatas.clear(); // 不算元神

	int newData = 0;
	switch (valueType)
	{
	case ZhuangbeiScanner::VT_HP:
		newData= calculator.calculateHp(calculatedata);
		break;
	case ZhuangbeiScanner::VT_CHENGSHOULI:
		newData = calculator.calculateChengShouLi(calculatedata);
		break;
	case ZhuangbeiScanner::VT_GONGJI:
		newData = calculator.calculateGongJiLi_MacroValue(calculatedata,Range_Average);
		break;
	default:
		break;
	}

	out_change = newData - initValue;
}


void ZhuangbeiScanner::setIgnoreHint(bool ignore)
{
	ignoreHint = ignore;
	if (ignoreHint)
	{
		equipmentToCompare.clear();
		isEquipPoolModifiedFromLastGenReport = true;
	}
}

void ZhuangbeiScanner::setIgnoreHint(bool ignore,BaseActor* actor,int pos)
{
	// 没有用了
}

bool ZhuangbeiScanner::isIgnoreHint(BaseActor* actor,int pos)
{
	// 没有用了
	return false;
}




ZhuangbeiScanner::~ZhuangbeiScanner(void)
{
}
