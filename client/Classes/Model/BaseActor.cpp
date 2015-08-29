#include "BaseActor.h"
#include "LFUtile.h"
#include "MsgDefine.h"
#include "GameDirector.h"
#include "Music.h"
#include "ImageManager.h"
#include "EquipmentManager.h"
#include "SkillManager.h"
#include "MyselfManager.h"
#include "ScriptDataProvider.h"
#include "PropertyCalculator.h"
#include "ClientErrorCode.h"
#include "WuPinManager.h"
#include "CS_Common.h"
#include "BaseActorPropertyParser.h"


inline CCPoint randomPosRelativeNode(CCNode* relativeNode)
{
	return ccpSub(relativeNode->getPosition(),ccp(0,0));
}

#define TAG_JIA_FANG_YU	43210
#define TAG_BASE_ACTOR_SPRITE 34567
#define TAG_BE_ATTACKED   34568

#define SHAKE_OFFSET 30.0f

BaseActor::BaseActor(BaseActorProperty p)
{
	this->mappingBody = NULL;
	this->mBaseProperty = p;
	mCalulator = new PropertyCalculator();
	mCurrentUsedSkill = NULL;
	mResMgr = ResourcePathManager::getManager();
}
//
Profession BaseActor::getActorType()
{
	return mBaseProperty.profession;
}
bool BaseActor::isHelper()
{
	return mBaseProperty.profession >=Profession_Qishi;
}

bool BaseActor::canUpdateLevel(int &clientErrorCode)
{
	if (!isYuanShen())
	{
		if (mBaseProperty.level >= HERO_MAX_LEVEL)
		{
			clientErrorCode = CEC_HeroIsMaxLevel;
			return false;
		}

		// 经验还不够，不能升级
		if (mBaseProperty.exp_current < mBaseProperty.exp_need)
		{
			clientErrorCode = CEC_ExpNotEnough;
			return false;
		}

		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < CS::heroLevelUpNeedSilver(mBaseProperty.level+1))
		{
			clientErrorCode = CEC_SilverNotEnough;
			return false;
		}

		if (WuPinManager::getManager()->getWuPin(ItemsType_Hero_LevelUp)->getCount() < CS::heroLevelUpNeedItemCount(mBaseProperty.level+1))
		{
			clientErrorCode = CEC_HeroUpdateLevelItemNotEnough;
			return false;
		}

		return true;
	}
	else
	{
		if (mBaseProperty.level >= YUANSHEN_MAX_LEVEL)
		{
			clientErrorCode = CEC_HeroIsMaxLevel;
			return false;
		}

		// 经验还不够，不能升级
		if (mBaseProperty.exp_current < mBaseProperty.exp_need)
		{
			clientErrorCode = CEC_ExpNotEnough;
			return false;
		}

		if (mBaseProperty.level % 5 == 0) // 5,10,15这些砍不能升级，只能通过升阶
		{
			clientErrorCode = CEC_NeedUpStageFirst;
			return false;
		}

		return true;
	}
}
bool BaseActor::canUpdateStage(int &clientErrorCode)
{
	if (!isYuanShen())
	{
		if (mBaseProperty.currentStage >= CEC_HeroIsMaxStage)
		{
			clientErrorCode = CEC_HeroIsMaxStage;
			return false;
		}

		if (mBaseProperty.currentStage >= MyselfManager::getManager()->getJiaoTangLevel())
		{
			clientErrorCode = CEC_NeedUpLevelJiangTang;
			return false;
		}


		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < CS::heroUpgradeNeedSilver(mBaseProperty.currentStage+1))
		{
			clientErrorCode = CEC_SilverNotEnough;
			return false;
		}

		if (WuPinManager::getManager()->getWuPin(ItemsType_Hero_Upgrade)->getCount() < CS::heroUpgradeNeedItemCount(mBaseProperty.currentStage+1))
		{
			clientErrorCode = CEC_HeroUpdateStageItemNotEnough;
			return false;
		}

		return true;
	}
	else
	{
		if (mBaseProperty.currentStage >= YUANSHEN_MAX_STAGE)
		{
			clientErrorCode = CEC_HeroIsMaxStage;
			return false;
		}

		if (WuPinManager::getManager()->getWuPin(ItemsType_Silver)->getCount() < CS::yuanshenUpgradeNeedSilver(mBaseProperty.currentStage+1))
		{
			clientErrorCode = CEC_SilverNotEnough;
			return false;
		}

		if (mBaseProperty.level % 5 != 0) // 只能在5,10** 
		{
			clientErrorCode = CEC_YuanshenUpstageNeed5Mode;
			return false;
		}

		if (mBaseProperty.exp_need > mBaseProperty.exp_current)
		{
			clientErrorCode = CEC_ExpNotEnough;
			return false;
		}

		return true;
	}
}

BaseActorProperty BaseActor::getCurrentProperty() const
{
	//BaseActorProperty tempP = mBaseProperty;
	//// 基础属性影响的，以后就这样装饰下去。都在这里改。fightSysytem的是计算值
	//tempP.mBaoJiLv = mBuffProc->getStateBaoJiLv(tempP.mBaoJiLv);
	return mBaseProperty;
} 

const BaseActorProperty* BaseActor::getCurrentProperty_R() const
{
	return &mBaseProperty;
}

int BaseActor::getLevel()
{
	return	mBaseProperty.level;
}
bool BaseActor::canUnlock()
{
	return	mBaseProperty.canUnlock;
}
bool BaseActor::isLocked()
{
	return	mBaseProperty.isLocked;
}
void BaseActor::setLocked(bool isLocked)
{
	mBaseProperty.isLocked = isLocked;

}
void BaseActor::setCanUnlock(bool canUnlock)
{
	mBaseProperty.canUnlock = canUnlock;
}

void BaseActor::setActorBuZhengPos(unsigned int posIndex)
{
	if (posIndex > FormationHeroAcountMax)
	{
		CCAssert(false,"");
		return;
	}
	mBaseProperty.postion = posIndex;
}

void BaseActor::refreshBaseProPerty(HeroDetail data)
{
	mBaseProperty.refeshPro(&data);
}

void BaseActor::refreshBaseProPerty(BaseActorProperty data)
{
	mBaseProperty = data;
}

void BaseActor::setVitality(int value)
{
	if (value < 0)
		value = 0;
	// 可以超出
	// 不做限制
	//if (value > Vitality_Total)
	//	value = Vitality_Total;

	mBaseProperty.vit_current = value;
}

void BaseActor::setExpInfor(unsigned int cur_exp)
{
	if (cur_exp >= mBaseProperty.exp_need)
	{
		string name = BaseActorPropertyParser::getParser()->getExtraProerty(mBaseProperty.profession).nameInChinese;
		CCLog("Infor : %s -->{id = %u,profression = %s, level = %d} cur_exp[%d] is bigger than level up need[%d]! you can upgrade your level",
			isYuanShen()?"YuanShen":"ZhenShen",
			getId(),name.c_str(),
			mBaseProperty.level,
			cur_exp,
			mBaseProperty.exp_need);
	}
	mBaseProperty.exp_current = cur_exp;
}

void BaseActor::actorLevelAdd(int toAdd)
{
	CCAssert(false,"废除了:不可能直接修改等级，修改等级需要刷HeroDetail");
}

void BaseActor::initSkill(vector<SkillData*> skills)
{
	if (isYuanShen())
	{
		CCLog("Waring : %s-->Actor is YuanShen, not need to set skill,is this a bug??",__FUNCTION__);
	}

	mSkillVec.clear();
	int num = skills.size();
	for (int i=0;i<num;i++)
	{
		// 不是我的，不能初始化
		if (skills[i]->getInfor()->owner_hero_id != mBaseProperty.hero_id)
		{
			CCAssert(false,"");
			continue;
		}
		mSkillVec.push_back(skills[i]);

		// 设定当前的技能
		if (skills[i]->getInfor()->is_equiped)
		{
			setCurrentUseSkill(skills[i]);
		}
	}
}
void BaseActor::initEquipment(vector<Equipment*> items)
{
	mEquipVec.clear();
	int num = items.size();

	for (int i=0;i<num;i++)
	{
		// 不是我的，不能初始化
		if (items[i]->getActorId() != mBaseProperty.hero_id)
		{
			CCAssert(false,"");
			continue;
		}
		mEquipVec.push_back(items[i]);
		items[i]->setCurrentEquipedPlace(getPlaceToEquip(items[i]));
	}
}

void BaseActor::attatchSkill(SkillData* skill)
{
	if (isYuanShen())
	{
		CCLog("Waring : %s-->Actor is YuanShen, not need to set skill,is this a bug??",__FUNCTION__);
	}

	// 不能添加不是我学会的技能
	CCAssert(skill->getInfor()->owner_hero_id == mBaseProperty.hero_id,"");
	CCAssert(SkillManager::getManager()->getSkillData(skill->getInfor()->skill_id) != NULL,"必须先添加到Manager");

	int num = mSkillVec.size();
	for (int i=0;i<num;i++)
	{
		if (mSkillVec[i]->getInfor()->skill_id == skill->getInfor()->skill_id)
		{
			CCLog("Waring : %s ---> is already attached!",__FUNCTION__);
			return;
		}
	}

	mSkillVec.push_back(skill);

	if (skill->getInfor()->is_equiped)
	{
		setCurrentUseSkill(skill); // 设置成当前的技能
	}
}


Equipment* BaseActor::getEquipmentAt(EquipRequirePlace place)
{
	int num = mEquipVec.size();

	for (int i=0;i<num;i++)
	{
		if (mEquipVec[i]->getCurrentPlace() == place)
		{
			return mEquipVec[i];
		}
	}
	return NULL;
}

vector<Equipment*> BaseActor::getNeedTakeOffIfWareOnEqui(ZhuangbeiType type,EquipRequirePlace needPlace)
{
	return EquipmentManager::getManager()->getNeedTakeOffIfWareOnEqui(type,needPlace,mEquipVec);
}

EquipRequirePlace BaseActor::getPlaceToEquip(Equipment* equip)
{
	// 可以装到的位置
	vector<EquipRequirePlace> reqPlace = equip->getRequiredPlaces();
	EquipRequirePlace placeCanUsed; // 目前还是空位的地方
	bool found = false;
	for (int i=0;i<(int)reqPlace.size();i++)
	{
		if (getEquipmentAt(reqPlace[i]) == NULL )
		{
			placeCanUsed = reqPlace[i];
			found = true;
			break;
		}
	}

	if (!found)
	{
		CCAssert(false,"已经没有空位的卡槽存放当前武器了");
		placeCanUsed = reqPlace[0]; // 全部堆到第一个卡槽
	}

	return placeCanUsed;
}

void BaseActor::attachEquipment(Equipment* equip)
{
	// 不能添加别人已经使用的
	// 除非是自己
	CCAssert(equip->getActorId() == 0 || equip->getActorId() == mBaseProperty.hero_id,"");
	CCAssert(EquipmentManager::getManager()->getOneEquipment(equip->getEquipId()) != NULL,"必须先添加到Manager");

	int num = mEquipVec.size();
	for (int i=0;i<num;i++)
	{
		if (mEquipVec[i]->getEquipId() == equip->getEquipId())
		{
			CCLog("Waring : BaseActor::attachEquipment ---> is already attached!");
			return;
		}
	}
	mEquipVec.push_back(equip);
	equip->setActorId(mBaseProperty.hero_id);
	equip->setCurrentEquipedPlace(getPlaceToEquip(equip));
}

void BaseActor::detatchAllEquipment()
{
	int equipNum = mEquipVec.size();
	for (int i=0;i<equipNum;i++)
	{
		mEquipVec[i]->setActorId(INVALID_ID);
		mEquipVec[i]->setCurrentEquipedPlace(POS_NONE);
	}
	mEquipVec.clear();
}

void BaseActor::detachEuipment(Equipment* equip)
{
	int num = mEquipVec.size();

	// 我有装备
	if (equip->getActorId() == mBaseProperty.hero_id)
	{
		vector<Equipment*>::iterator it = mEquipVec.begin();
		for (it;it!=mEquipVec.end();it++)
		{
			// 同一个物品
			if ( (*it)->getEquipId() == equip->getEquipId())
			{
				mEquipVec.erase(it);
				break;
			}
		}
		// 设置成无效的
		equip->setActorId(INVALID_ID);
		equip->setCurrentEquipedPlace(POS_NONE);
	}else
	{
		CCLog("Waring : BaseActor::detachEuipment ---> I have not Equip this one!");
	}
}

void BaseActor::setCurrentUseSkill(SkillData* skill)
{
	if (isYuanShen())
	{
		CCLog("Waring : %s-->Actor is YuanShen, not need to set skill,is this a bug??",__FUNCTION__);
	}

	CCAssert(skill->getInfor()->owner_hero_id == mBaseProperty.hero_id,"只能使用自己的技能");

	if (skill == mCurrentUsedSkill)
	{
		CCLog("Waring : %s this skill is already on actor,don't set again!");
		return;
	}

	if (mCurrentUsedSkill != NULL)
	{
		mCurrentUsedSkill->getInfor()->is_equiped = false;
		delete mCurrentUsedSkill;
		mCurrentUsedSkill = NULL;
	}
	mCurrentUsedSkill = new SkillData(*skill->getInfor());
	mCurrentUsedSkill->getInfor()->is_equiped = true;
}
SkillData* BaseActor::getCurrentUseSkill()
{
	return mCurrentUsedSkill;
}

unsigned int BaseActor::getCurrentSkillZhouQi()
{
	CCAssert(mCurrentUsedSkill != NULL,"必须要有技能才能掉这个函数");
	CalculateData data = getCalculateData();
	return mCalulator->calculateJiNengZhouQi(data);
}
unsigned int BaseActor::getCurrentSkillTurn()
{
	CCAssert(mCurrentUsedSkill != NULL,"必须要有技能才能掉这个函数");
	CalculateData data = getCalculateData();
	return mCalulator->calculateJiNengTurn(data);
}
unsigned int BaseActor::getCurrentSkillResultPercent()
{
	CCAssert(mCurrentUsedSkill != NULL,"必须要有技能才能掉这个函数");
	CalculateData data = getCalculateData();
	return mCalulator->calculateJiNengResult(data);
}

HeroPropertySummery BaseActor::getSummeryProperty() const
{
	HeroPropertySummery ret;
	{
		ret.is_yuanshen = mBaseProperty.isYuanShen;
		ret.profession = mBaseProperty.profession;
		ret.stage = mBaseProperty.currentStage;
		ret.level = mBaseProperty.level;
		ret.base_hp = mBaseProperty.mHP;
		ret.liliang = mBaseProperty.mLiLiang;
		ret.mingjie = mBaseProperty.mMinJie;
		ret.zhili = mBaseProperty.mZhiLi;
		ret.tineng = mBaseProperty.mTiNeng;
		ret.baojilv = mBaseProperty.mBaoJiLv;
		ret.baoshang_yinzi = mBaseProperty.mBaoJiShangHaiYinZi;
		ret.effect_convert_per = mBaseProperty.currentEffectPercent;

		if (mCurrentUsedSkill != NULL)
		{
			ret.skill_type = mCurrentUsedSkill->getInfor()->skill_type;
			ret.skill_level = mCurrentUsedSkill->getInfor()->skill_level;
		}
	}

	return ret;
}

// 获得当前状态的CalculateData
CalculateData BaseActor::getCalculateData() const
{
	CalculateData data;
	int e_num = mEquipVec.size();
	vector<EquipData> equip_vec;
	for (int i=0;i<e_num;i++)
	{
		equip_vec.push_back(mEquipVec[i]->getEquipData());
	}
	data.allEquips = equip_vec;
	data.mainPro = getMainPropertyValue();
	//data.basePro = &mBaseProperty;
	data.base_pro = getSummeryProperty();
	// 附加元神的数据
	if (getMyYuanShen() != NULL)
	{
		data.includeCalDatas.push_back(getMyYuanShen()->getCalculateData());
		CCAssert(getMyYuanShen()->getCalculateData().allBuffs.empty(),"元神没有Buff【必须这样，因为后台数据计算不支持】");
		CCAssert(getMyYuanShen()->getCalculateData().includeCalDatas.empty(),"为了安全，元神不在带有include数据");
	}

	return data;
}
CalculateData BaseActor::getCalculateDataWithMainProZero() const
{
	CalculateData data;
	data.allBuffs.clear();
	int e_num = mEquipVec.size();
	vector<EquipData> equip_vec;
	for (int i=0;i<e_num;i++)
	{
		equip_vec.push_back(mEquipVec[i]->getEquipData());
	}
	data.allEquips = equip_vec;
	data.mainPro = 0;				// 这里填0
	//data.basePro = &mBaseProperty;
	data.base_pro = getSummeryProperty();
	// 附加元神的数据
	if (getMyYuanShen() != NULL)
	{
		data.includeCalDatas.push_back(getMyYuanShen()->getCalculateData());
	}

	return data;
}
// 计算如果装上了toEquip这个装备之后的CalculateData
CalculateData BaseActor::getCalculateDataWhenEquipOne(Equipment* toEquip,Equipment* willBeRepalced /* = NULL */)
{
	CalculateData data;
	PropertyCalculator calculator;

	{
		// 需要脱下的
		vector<EquipRequirePlace> places;
		if (willBeRepalced != NULL)
		{
			places.push_back(willBeRepalced->getCurrentPlace());
		}else
		{
			places = toEquip->getRequiredPlaces();
		}
		 

		// 寻找需要装最少的方案，当然还有其他的方式，比如看脱哪个更好。
		vector<Equipment*> needTakeOff = getNeedTakeOffIfWareOnEqui(toEquip->getType(),places[0]);
		int takeOffNum = needTakeOff.size();
		int placeNum = places.size();
		for (int i=1;i<placeNum;i++)
		{
			vector<Equipment*> needTakeOffTemp =  getNeedTakeOffIfWareOnEqui(toEquip->getType(),places[i]);
			if (needTakeOffTemp.size() < takeOffNum)
			{
				takeOffNum = needTakeOffTemp.size();
				needTakeOff = needTakeOffTemp;
			}
		}

		// 暂时脱下需要脱掉的，然后装上即将装上的，待会还原
		for (int i=0;i<takeOffNum;i++)
			detachEuipment(needTakeOff[i]);
		int lastId = toEquip->getActorId();
		EquipRequirePlace lastPlace = toEquip->getCurrentPlace();
		toEquip->setActorId(0);
		attachEquipment(toEquip);


		int e_num = getEquipments().size();
		vector<EquipData> equip_vec;
		for (int i=0;i<e_num;i++)
		{
			bool add = true;
			for (int j=0;j<takeOffNum;j++)
			{
				// 需要脱下
				if (getEquipments()[i]->getEquipId() == needTakeOff[j]->getEquipId())
				{
					add = false;
					break;
				}
			}

			if(add)
				equip_vec.push_back(getEquipments()[i]->getEquipData());
		}

		data.allEquips = equip_vec;
		data.mainPro = getMainPropertyValue();
		//data.basePro = &mBaseProperty;
		data.base_pro = getSummeryProperty();
		// 附加元神的数据
		if (getMyYuanShen() != NULL)
		{
			data.includeCalDatas.push_back(getMyYuanShen()->getCalculateData());
		}

		//还原
		detachEuipment(toEquip);
		toEquip->setActorId(lastId);
		toEquip->setCurrentEquipedPlace(lastPlace);

		for (int i=0;i<takeOffNum;i++)
		{
			attachEquipment(needTakeOff[i]);
		}
	}
	return data;
}

unsigned int BaseActor::getMainPropertyValue() const
{
	unsigned int ret = 0;
	switch (mBaseProperty.profession)
	{
	case Profession_Yemanren:
		ret = getLiliang();
		break;
	case Profession_Chike:
		ret = getMinjie();
		break;
	case Profession_Fashi:
		ret = getZhili();
		break;
	case Profession_Qishi:
		ret = getLiliang();
		break;
	case Profession_ShenJianShou:
		ret = getMinjie();
		break;
	case Profession_MiShu:
		ret = getZhili();
		break;
	case Profession_WuSeng:
		ret = getLiliang();
		break;
	case Profession_YouXia:
		ret = getMinjie();
		break;
	case Profession_LieShou:
		ret = getMinjie();
		break;
	case Profession_WuShi:
		ret = getZhili();
		break;
	default:
		ret = 0;
		CCLog("Error : BaseActor::getMainPropertyValue --> mBaseProperty.mActorType = %d.",mBaseProperty.profession);
		CCAssert(false,"");
		break;
	}
	return ret;
}

float BaseActor::getXueliang() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateHp(data);;
}
float BaseActor::getGongji() const
{
	CalculateData data = getCalculateData();
	unsigned int ret = mCalulator->calculateGongJiLi(data,Range_Average);
	return ret;
}

float BaseActor::getMyZhanDouLi(int range) const
{
	float ret = 0.0f;
	CalculateData data = getCalculateData();
	data.base_pro.level = 1;
	ret = mCalulator->calculateZhanDouLi(data,(ValueRange)range);
	return ret;
}
float BaseActor::getGongji_MacroValue() const
{
	return mCalulator->calculateGongJiLi_MacroValue(getCalculateData(),Range_Average);
}

float BaseActor::getHujia_MacroValue() const
{
	return mCalulator->calculateHuJiaZhi_MacroValue(getCalculateData());
}
float BaseActor::getMokang_MacroValue() const
{
	return mCalulator->calculateKuangXingZhi_MacroValue(getCalculateData());
}

float BaseActor::getHujia() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateHuJiaZhi(data);
}
float BaseActor::getMokang() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateKuangXingZhi(data);
}
unsigned int BaseActor::getBaoji() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateBaoJiLv(data);
}
unsigned int BaseActor::getBaoshang() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateBaoJiShangHaiYinzi(data);
}
float BaseActor::getMinjie() const
{
	return mCalulator->calculateMinJie(getCalculateDataWithMainProZero());
}
float BaseActor::getLiliang() const
{
	return mCalulator->calculateLiliang(getCalculateDataWithMainProZero());
}
float BaseActor::getZhili() const
{
	return mCalulator->calculateZhili(getCalculateDataWithMainProZero());
}
float BaseActor::getTineng() const
{
	return mCalulator->calculateTineng(getCalculateDataWithMainProZero());
}
void BaseActor::getExpInfor(unsigned int& out_cur_exp,unsigned int & out_exp_need) const
{
	out_cur_exp = mBaseProperty.exp_current;
	out_exp_need = mBaseProperty.exp_need;
}
unsigned int BaseActor::getJinengChufaLv() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateJiNengChuFaLv(data);
}
float BaseActor::getGedangZhi() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateWuLiGeDangZhi(data);
}
unsigned int BaseActor::getGedangLv() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateGeDangLv(data);
}
unsigned int BaseActor::getShanbiLv() const
{
	CalculateData data = getCalculateData();
	return mCalulator->calculateShanBiLv(data);
}
unsigned int BaseActor::getCurrentStage()const
{
	return mBaseProperty.currentStage;
}
void BaseActor::setCurrentStage(unsigned int stage)
{
	CCAssert(false,"不会需要这个函数，因为升阶之后，属性就变了，所有应该到时用refresh方法耍数据");
	//CCAssert(stage && stage <= 9,"最大就是9阶");
	//mBaseProperty.currentStage = stage;
}
bool BaseActor::canBeXiLiByCurrentJiaoTang()const
{
	return MyselfManager::getManager()->canXiLiActor(this);
}
bool BaseActor::isYuanShen()const
{
	return mBaseProperty.isYuanShen;
}

void BaseActor::setYuanShen(BaseActor* yuanshen)
{
	mappingBody = yuanshen;

	if (yuanshen != NULL)
	{
		if (yuanshen->getMyZhenShen() != this)
		{
			yuanshen->setZhenShen(this);
		}
	}
}
void BaseActor::setZhenShen(BaseActor* zhenshen)
{
	mappingBody = zhenshen;

	if (zhenshen != NULL)
	{
		if (zhenshen->getMyYuanShen() != this)
		{
			zhenshen->setYuanShen(this);
		}
	}
}

// 取得元神，如果本来就是元神，返回NULL
BaseActor* BaseActor::getMyYuanShen()const
{
	if (isYuanShen())
	{
		return NULL;
	}
	return mappingBody;
}
// 取得真身，如果本来是真身就返回NULL
BaseActor* BaseActor::getMyZhenShen()const
{
	if (!isYuanShen())
	{
		return NULL;
	}
	return mappingBody;
}

BaseActor::~BaseActor(void)
{
	if (mCalulator)
	{
		delete mCurrentUsedSkill;
		mCurrentUsedSkill = NULL;
	}

	delete mCalulator;
}

