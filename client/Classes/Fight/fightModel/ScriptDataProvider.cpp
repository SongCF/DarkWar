#include "ScriptDataProvider.h"
#include "CS_Common.h"
#include "CalculateData.h"
#ifdef _CLIENT_
#include "MyselfManager.h"
#include "FightScriptUserData.h"
#endif // _CLIENT_


extern int randomWithin(int start,int end);
extern float lfRandom0_1();

//////////////////////////////////////////////////////////////////////////
// AbstractItem
//////////////////////////////////////////////////////////////////////////

AbstractItem::AbstractItem()
{
	mDefaultFullHp = 0;
	mCdTime = 0;
	mFightTurn = 1;
}

bool AbstractItem::isEquipedSkill(SkillType type)
{
	for(int i=0,count = skilVec.size(); i<count; i++)
	{
		if (skilVec[i].skillType == type)
		{
			return true;
		}
	}
	return false;
}
bool AbstractItem::attachBuff(GameBuff buff)
{
	if (!buff.cumulative && haveBuffWithStateIs(buff.state))
		return false;

	mAttachedBuffVec.push_back(buff);
	return true;
}
bool AbstractItem::updateBuff(GameBuff buff)
{
	for (list<GameBuff>::iterator it = mAttachedBuffVec.begin(); 
		it!= mAttachedBuffVec.end() ; it++)
	{
		if (it->state == buff.state)
		{
			it->initWithBuff(buff);
			return true;
		}
	}
	return false;
}

void AbstractItem::removeBuff(int _id)
{
	for (list<GameBuff>::iterator it = mAttachedBuffVec.begin(); 
		it!= mAttachedBuffVec.end() ;)
	{
		// 有效果
		if (it->effect_vec.size() > 0)
		{
			// 移除
			for (list<GameBuffEffect>::iterator it_eff = it->effect_vec.begin();it_eff!=it->effect_vec.end();)
			{
				if (it_eff->effect_id == _id)
				{
					it_eff = it->effect_vec.erase(it_eff);
					continue;
				}
				it_eff ++;
			}

		}else
		{
			it = mAttachedBuffVec.erase(it);
			continue;
		}
		it++ ;
	}
}

bool AbstractItem::haveBuff(int _id)
{
	for (list<GameBuff>::iterator it_buff = mAttachedBuffVec.begin();it_buff != mAttachedBuffVec.end();it_buff++)
	{
		for (list<GameBuffEffect>::iterator it_eff = it_buff->effect_vec.begin();it_eff != it_buff->effect_vec.end();it_eff++)
		{
			if (it_eff->effect_id == _id)
			{
				return true;
			}
		}
	}
	return false;
}

bool AbstractItem::haveBuffWithStateIs(BuffStateId state)
{
	for (list<GameBuff>::iterator it_buff = mAttachedBuffVec.begin();it_buff != mAttachedBuffVec.end();it_buff++)
	{
		if (it_buff->state == state)
		{
			return true;
		}
	}
	return false;
}

bool AbstractItem::canUseThisInCurrentFightTurn()
{
	if (skilVec.size() == 0)
	{
		return false;
	}

	int zhouqi = getJiNengChuFaZhouQi();	// 例如5
	int turn = getJiNengChuFaTurn();		// 例如1   ---> (1-1)%5 = (1-1)
	CCAssert(turn >= 1 && turn <= zhouqi,"不能越界，到时自动压制一下就是了");

	// 刚好是点上，那么就可以触发
	return (mFightTurn-1)%zhouqi == (turn-1);
}

void AbstractItem::subHp(int toSub)				// 加血
{
	if (toSub < 0)
	{
		addHp(-toSub);
		return;
	}
	if (getHp() < toSub)
	{
		setCurrentHp(0);
		CCLog("Item Dead Clear All Buffs: %d",mAttachedBuffVec.size());
		mAttachedBuffVec.clear();
	}else
	{
		setCurrentHp(getHp() - toSub);
	}
}
void AbstractItem::addHp(int toAdd)
{
	if (toAdd < 0)
	{
		subHp(-toAdd);
		return;
	}
	setCurrentHp(getHp() + toAdd);

	if (getHp() > mDefaultFullHp)
	{
		setCurrentHp(mDefaultFullHp);
	}
}
int AbstractItem::getDefaultHp() const
{
	return mDefaultFullHp;
}
unsigned int AbstractItem::getMinJie() const
{
	return 0;
}
unsigned int AbstractItem::getZhiLi() const
{
	return 0;
}



//////////////////////////////////////////////////////////////////////////
// TeamHero
//////////////////////////////////////////////////////////////////////////
TeamHero::TeamHero()
{
}
TeamHero::~TeamHero()
{
}
void TeamHero::initWithTeamBaseProperty(TeamBaseProperty pro)
{
	mPro = pro;
	mDefaultFullHp = mPro.mHP;
}
Profession TeamHero::getProfession() const
{
	return mPro.leaderProfression;
}
void TeamHero::setCurrentHp(unsigned int hp)
{
	mPro.mHP = hp;
}
TeamBaseProperty TeamHero::getTeamProperty() const
{
	return mPro;
}

CalculateData TeamHero::getCalculateData() const
{
	CCAssert(false,"TeamHero不提供CalculateData，计算的数值是直接PropertyCalculator提供接口，直接用TeamHero*计算");
	CalculateData data;
	return data;
}
struct HeroPropertySummery TeamHero::getSummeryProperty() const
{
	CCAssert(false,"TeamHero不提供CalculateData，计算的数值是直接PropertyCalculator提供接口，直接用TeamHero*计算");
	HeroPropertySummery data;
	return data;
}

HeroBaseType TeamHero::getHeroType() const
{
	return Type_Hero_Team;
}
int TeamHero::getXSize() const
{
	return 1;// 都是单格子的
}
int TeamHero::getLevel() const
{
	return mPro.mLevel;
}
int TeamHero::getStage() const
{
	return mPro.mStage;
}
float TeamHero::getGongJiJuLi()const
{
	return mPro.mGongJiJuLi;
}
float TeamHero::getGongJiLi(ValueRange range) const
{
	float gongJiLi =  0;
	switch (range)
	{
	case Range_Ramdom:
		gongJiLi = randomWithin(mPro.mGongJiLi_MIN,mPro.mGongJiLi_MAX);
		break;
	case Range_Max:
		gongJiLi = max(mPro.mGongJiLi_MIN,mPro.mGongJiLi_MAX);
		break;
	case Range_Average:
		gongJiLi = (mPro.mGongJiLi_MIN+mPro.mGongJiLi_MAX)/2.0f;
		break;
	case Range_Min:
		gongJiLi = min(mPro.mGongJiLi_MIN,mPro.mGongJiLi_MAX);
		break;
	default:
		CCAssert(false,"不支持的值域类型");
		break;
	}
	return gongJiLi;
}
int TeamHero::getHp() const
{
	return mPro.mHP;
}
unsigned int TeamHero::getBaoJiLv() const
{
	BuffCalculator bCalculator;
	return mPro.mBaoJiLv + bCalculator.getBuffBaoJiLv(mAttachedBuffVec);
}
unsigned int TeamHero::getHuJiaJianMian() const
{
	CCAssert(false,"不会调用，只有对Guaiwu有用,因为TeamHero里面的攻击伤害已经在getShangHaiValue里面处理了");
	return 0;
}
unsigned int TeamHero::getKangXingJianMian() const
{
	CCAssert(false,"不会调用，只有对Guaiwu有用,因为TeamHero里面的攻击伤害已经在getShangHaiValue里面处理了");
	return 0;
}
unsigned int TeamHero::getBaoJiShangHaiYinZi() const
{
	return mPro.mBaoJiShangHaiYinZi;
}
unsigned int TeamHero::getJiNengChuFaLv() const
{
	return mPro.mJiNengFaChuLv;
}
unsigned int TeamHero::getJiNengChuFaZhouQi() const
{
	return mPro.mJiNengZhouQi;
}
unsigned int TeamHero::getJiNengChuFaTurn() const
{
	return mPro.mJiNengTurn;
}
unsigned int TeamHero::getJiNengResult() const
{
	return mPro.mJiNengResultPercent;
}


unsigned int TeamHero::getShanBiLv() const
{
	BuffCalculator bCalculator;
	return mPro.mShanBiLv + bCalculator.getBuffShanBiLv(mAttachedBuffVec);
}
int TeamHero::getShangHaiLeiXing()const
{
	return mPro.mShangHaiLeiXing;
}	
float TeamHero::getShangHaiValue(AbstractItem* target,ValueRange range /* = Range_Ramdom */) const
{
	// 英雄打怪物
	if (dynamic_cast<Guaiwu*>(target)!=NULL)
	{
		PropertyCalculator mCalulator;
		mCalulator.calculateAttackShangHaiValue(this,(Guaiwu*)target);
		return mCalulator.calculateAttackShangHaiValue(this,(Guaiwu*)target,range);
	}
	else
	{
		CCAssert(false,"不支持");
	}
	return 0;
}


bool TeamHero::getSpecialTrigger() const
{
	CCAssert(false, "不支持");
    return true;
}

void TeamHero::setSpecialTrigger()
{
	CCAssert(false, "不支持");
}





//////////////////////////////////////////////////////////////////////////
// BaseHero
//////////////////////////////////////////////////////////////////////////
// 战斗时用的角色的数据
BaseHero::BaseHero()
{
	mDefaultBloodCdTime = 0;
}

BaseHero::~BaseHero()
{
}
int BaseHero::getLevel() const
{
	CCAssert(false,"");
	return 0;
}

int BaseHero::getStage() const
{
	CCAssert(false,"");
	return 1;
}
float BaseHero::getGongJiLi(ValueRange range) const
{
	CCAssert(false,"");
	return 1;
}

int BaseHero::getHp() const
{
	CCAssert(false,"");
	return 0;
}

void BaseHero::setCurrentHp(unsigned int hp)
{
	CCAssert(false,"");
}

Profession BaseHero::getProfession() const
{
	CCAssert(false,"");
	return Profession_Yemanren;
}

CalculateData BaseHero::getCalculateData()const
{
	CCAssert(false,"没用实现，子类需要自己实现");
	CalculateData data;
	data.mainPro = 0;
	return data;
}
struct HeroPropertySummery BaseHero::getSummeryProperty() const
{
	CCAssert(false,"没用实现，子类需要自己实现");
	HeroPropertySummery data;
	return data;
}

HeroBaseType BaseHero::getHeroType() const
{
	CCAssert(false,"");
	return Type_Hero_Normal;
}
int BaseHero::getXSize() const
{
	CCAssert(false,"应该有具体的子类来完成");
	return 1;
}

float BaseHero::getGongJiJuLi()const			// 攻击距离
{
	CCAssert(false,"");
	return 1;
}
unsigned BaseHero::getMinJie() const
{
	CCAssert(false,"");
	return 0;	// 默认没有敏捷
}
unsigned int BaseHero::getBaoJiLv() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getZhiLi() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getLiLiang() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getHuJiaJianMian() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getKangXingJianMian() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getBaoJiShangHaiYinZi() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getJiNengChuFaLv() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getJiNengChuFaZhouQi() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getJiNengChuFaTurn() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getJiNengResult() const
{
	CCAssert(false,"");
	return 0;
}
unsigned int BaseHero::getShanBiLv() const
{
	CCAssert(false,"");
	return 0;
}
int BaseHero::getShangHaiLeiXing() const
{
	CCAssert(false,"");
	return SHANG_HAI_WU_LI;
}

bool BaseHero::getSpecialTrigger() const
{
	CCAssert(false,"应该有具体的子类来完成");
    return true;
}

void BaseHero::setSpecialTrigger()
{
	CCAssert(false,"应该有具体的子类来完成");
}


//////////////////////////////////////////////////////////////////////////
// GuaiWu
//////////////////////////////////////////////////////////////////////////
void Guaiwu::initWithGuaiwuData(GuaiWuData data)
{
	mData = data;
	mDefaultFullHp = data.xueLiang;
    
    mSpecialTriggerFlag = false;
}

int Guaiwu::getLevel() const
{
	return mData.level;
}

int Guaiwu::getStage() const
{
	return 1; // 没有阶
}

float Guaiwu::getGongJiJuLi()const			// 攻击距离
{
	return mData.gongJiJuLi;
}
Profession Guaiwu::getProfession() const
{
	return mData.profession;
}
HeroBaseType Guaiwu::getHeroType() const
{
	return mData.type;
}
int Guaiwu::getXSize() const
{
	return mData.x_size;
}
int Guaiwu::getGongJiLi() const
{
	return mData.gongJiLi;
}

float Guaiwu::getGongJiLi(ValueRange range) const
{
	return getGongJiLi();
}

unsigned int Guaiwu::getBaoJiLv() const
{
	return mData.baoJiLv;
}
unsigned int Guaiwu::getHuJiaJianMian() const
{
	return mData.huJiaJianMianLv;
}
unsigned int Guaiwu::getKangXingJianMian() const
{
	return mData.kangXingJianMianLv;
}
unsigned int Guaiwu::getBaoJiShangHaiYinZi() const
{
	return mData.baoShangYinZi;
}
unsigned int Guaiwu::getJiNengChuFaLv() const
{
	return mData.jiNengChuFaLv;
}

CalculateData Guaiwu::getCalculateDataForSkill() const
{
	// 直接把MainPro填写0.base_pro不管
	// 因为技能只会用到装备，buff数据
	CalculateData a_dta;
	{
		a_dta.allBuffs = mAttachedBuffVec;
		a_dta.allEquips.clear();
		// 技能的数据
		a_dta.base_pro.skill_type = skilVec[0].skillType;
		a_dta.base_pro.skill_level = skilVec[0].skillLevel;
		a_dta.mainPro = 0;
	}
	return a_dta;
}

unsigned int Guaiwu::getJiNengChuFaZhouQi() const
{
	CCAssert(skilVec.size() > 0,"");
	CalculateData a_dta = getCalculateDataForSkill();
	return mCalulator.calculateJiNengZhouQi(a_dta);
}
unsigned int Guaiwu::getJiNengChuFaTurn() const
{
	CCAssert(skilVec.size() > 0,"");
	CalculateData a_dta = getCalculateDataForSkill();
	return mCalulator.calculateJiNengTurn(a_dta);
}
unsigned int Guaiwu::getJiNengResult() const
{
	CCAssert(skilVec.size() > 0,"");
	CalculateData a_dta = getCalculateDataForSkill();
	return mCalulator.calculateJiNengResult(a_dta);
}
unsigned int Guaiwu::getShanBiLv() const
{
	return 0;	// 怪物不闪避
}
int Guaiwu::getHp() const
{
	return mData.xueLiang;
}
unsigned int Guaiwu::getMinJie() const
{
	return 0;		// 怪物没有敏捷
}
void Guaiwu::setCurrentHp(unsigned int hp)
{
	mData.xueLiang = hp;
}
float Guaiwu::getShangHaiValue(AbstractItem* target,ValueRange range /* = Range_Ramdom */)const
{
	// 怪物打怪物
	if (dynamic_cast<Guaiwu*>(target)!=NULL)
	{
		return mCalulator.calculateAttackShangHaiValue(this,(Guaiwu*)target,range);
	}
	// 怪物打团队
	else if (dynamic_cast<TeamHero*>(target)!=NULL)
	{
		return mCalulator.calculateAttackShangHaiValue(this,(TeamHero*)target,range);
	}
	// 怪物打英雄
	else if (dynamic_cast<Hero*>(target)!=NULL)
	{
		CalculateData a_dta = target->getCalculateData();
		return mCalulator.calculateAttackShangHaiValue(this,a_dta,range);
	}
	else
	{
		CCAssert(false,"不支持");
	}
	return 0;
}

int Guaiwu::getShangHaiLeiXing() const
{
	return mData.shangHaiLeiXing;
}


bool Guaiwu::getSpecialTrigger() const
{
    return mSpecialTriggerFlag;
}

void Guaiwu::setSpecialTrigger()
{
    if(mData.x_size < 2) {
		CCAssert(false, "boss怪物才有技能特殊触发");
    }
    
    if(!mSpecialTriggerFlag)
        mSpecialTriggerFlag = true;
    else {
		CCAssert(false, "已经触发过一次");
    }
}



//////////////////////////////////////////////////////////////////////////
// Hero
//////////////////////////////////////////////////////////////////////////

Hero::Hero()
{
	yuanShenBody = NULL;
}
Hero::~Hero()
{
	if(yuanShenBody != NULL)
		delete yuanShenBody;
}
void Hero::init(HeroBaseProperty basePro)
{
	this->yuanShenBody = NULL;
	this->pro = basePro;
	this->currentHp = basePro.mHP;
	this->mDefaultFullHp = currentHp;
	this->equipOpEnd = false;
}
// 装备的操作完毕了，需要调用这个方法，用于刷新血量
void Hero::zhuangbeiOpEnd(int curHp /* = -1 */)
{
	equipOpEnd = true;
	CalculateData data = getCalculateData();
	// 更新血值
	mDefaultFullHp = mCalulator.calculateHp(data);
	//mDefaultFullHp
	currentHp = curHp < 0?mDefaultFullHp : curHp;

	if (currentHp > mDefaultFullHp)
	{
		currentHp = mDefaultFullHp;
	}
}

// 设置元神/真身
void Hero::setYuanShen_AutoDeleted(Hero* yuanshen)
{
	CCAssert(!equipOpEnd,"应该在调用zhuangbeiOpEnd函数之前设置元神");
	CCAssert(!pro.isYuanShen,"不是元神的Hero才可以设置元神");
	if (yuanShenBody != NULL)
	{
		delete yuanShenBody;
	}
	yuanShenBody = yuanshen;
}

unsigned int Hero::getLiLiang() const
{
	// 不能再获取主属性，不然就是死递归了
	CalculateData data = getCalculateData(false);
	return mCalulator.calculateLiliang(data);
}
int Hero::getTiNeng() const
{
	// 不能再获取主属性，不然就是死递归了
	CalculateData data = getCalculateData(false);
	return mCalulator.calculateTineng(data);
}

unsigned int Hero::getZhiLi() const
{
	// 不能再获取主属性，不然就是死递归了
	CalculateData data = getCalculateData(false);
	return mCalulator.calculateZhili(data);
}
unsigned int Hero::getMinJie() const
{
	// 不能再获取主属性，不然就是死递归了
	CalculateData data = getCalculateData(false);
	return mCalulator.calculateMinJie(data);
}
float Hero::getGongJiJuLi() const
{
	// 这个不会变
	return pro.mGongJiJuLi;
}

float Hero::getGongJiLi(ValueRange range) const
{
	return mCalulator.calculateGongJiLi(getCalculateData(),range);
}



unsigned short Hero::get_resonance_star() const
{
	return pro.resonance_star;
}

Profession Hero::getProfession() const
{
	return pro.profession;
}
HeroBaseType Hero::getHeroType() const
{
	return Type_Hero_Normal;
}
int Hero::getXSize() const
{
	// 英雄都是占一格
	return 1;
}
int Hero::getHp() const
{
	// 有装备，那么必须是调用了zhuangbeiOpEnd才可以
	CCAssert(equipVec.size() ==0 || equipOpEnd==true,"");

	return currentHp;
}
unsigned int Hero::getBaoJiLv() const
{
	CalculateData data = getCalculateData();
	return mCalulator.calculateBaoJiLv(data);
}
unsigned int Hero::getBaoJiShangHaiYinZi() const
{
	CalculateData data = getCalculateData();
	return mCalulator.calculateBaoJiShangHaiYinzi(data);
}
unsigned int Hero::getJiNengChuFaLv() const
{
	CalculateData data = getCalculateData();
	return mCalulator.calculateJiNengChuFaLv(data);
}
unsigned int Hero::getJiNengChuFaZhouQi() const
{
	CCAssert(skilVec.size() > 0,"");
	return mCalulator.calculateJiNengZhouQi(getCalculateData());
}
unsigned int Hero::getJiNengChuFaTurn() const
{
	CCAssert(skilVec.size() > 0,"");
	return mCalulator.calculateJiNengTurn(getCalculateData());
}
unsigned int Hero::getJiNengResult() const
{
	CCAssert(skilVec.size() > 0,""); 
	return mCalulator.calculateJiNengResult(getCalculateData());
}


unsigned int Hero::getShanBiLv() const
{
	CalculateData data = getCalculateData();
	return mCalulator.calculateShanBiLv(data);
}
int Hero::getLevel() const
{
	return pro.level;
}
int Hero::getStage() const
{
	return pro.currentStage;
}
unsigned int Hero::getHuJiaJianMian() const
{
	CCAssert(false,"因为直接用计算伤害的函数，所以这里不需要写。");
	return 0;
}
unsigned int Hero::getKangXingJianMian() const
{
	CCAssert(false,"因为直接用计算伤害的函数，所以这里不需要写。");
	return 0;
}
CalculateData Hero::getCalculateData() const
{
	return getCalculateData(true);
}

struct HeroPropertySummery Hero::getSummeryProperty() const
{
	HeroPropertySummery ret;
	{
		ret.is_yuanshen = pro.isYuanShen;
		ret.profession = pro.profession;
		ret.stage = pro.currentStage;
		ret.level = pro.level;
		ret.base_hp = pro.mHP;
		ret.liliang = pro.mLiLiang;
		ret.mingjie = pro.mMinJie;
		ret.zhili = pro.mZhiLi;
		ret.tineng = pro.mTiNeng;
		ret.baojilv = pro.mBaoJiLv;
		ret.baoshang_yinzi = pro.mBaoJiShangHaiYinZi;
		ret.effect_convert_per = pro.currentEffectPercent;

		if (!skilVec.empty())
		{
			ret.skill_type = skilVec[0].skillType;
			ret.skill_level = skilVec[0].skillLevel;
		}
	}


	return ret;
}

CalculateData Hero::getCalculateData(bool needMainPro) const
{
	// 有装备，那么必须是调用了zhuangbeiOpEnd才可以
	CCAssert(equipVec.size() ==0 || equipOpEnd==true,"");

	CalculateData a_dta;
	{
		a_dta.allBuffs = mAttachedBuffVec;
		a_dta.allEquips = equipVec;
		//a_dta.basePro = &pro;
		a_dta.base_pro = getSummeryProperty();
		a_dta.mainPro = needMainPro?getMainPro():0;
		// 元神的数据
		if (yuanShenBody != NULL)
		{
			CCAssert(yuanShenBody->getCalculateData().allBuffs.empty(),"元神没有Buff【必须这样，因为后台数据计算不支持】");
			CCAssert(yuanShenBody->getCalculateData().includeCalDatas.empty(),"为了安全，元神不在带有include数据");
			a_dta.includeCalDatas.push_back(yuanShenBody->getCalculateData());
		}
	}
	return a_dta;
}
void Hero::setCurrentHp(unsigned int hp)
{
	currentHp = hp;
}
float Hero::getShangHaiValue(AbstractItem* target,ValueRange range /* = Range_Ramdom */) const
{
	// 有装备，那么必须是调用了zhuangbeiOpEnd才可以
	CCAssert(equipVec.size() ==0 || equipOpEnd==true,"");

	// 英雄打怪物
	if (dynamic_cast<Guaiwu*>(target)!=NULL)
	{
		return mCalulator.calculateAttackShangHaiValue(getCalculateData(),(Guaiwu*)target,range);
	}
	// 英雄打英雄
	else if (dynamic_cast<Hero*>(target)!=NULL)
	{
		CalculateData a_dta = ((Hero*)target)->getCalculateData();
		return mCalulator.calculateAttackShangHaiValue(getCalculateData(),a_dta,range);
	}
	else
	{
		CCAssert(false,"不支持");
	}
	return 0;
}

int Hero::getShangHaiLeiXing() const
{
	return pro.mShangHaiLeiXing;
}
int Hero::getMainPro() const
{
	int ret = 0;

	switch (pro.profession)
	{
	case Profession_Yemanren:
		ret = getLiLiang();
		//ret = pro.mLiLiang;
		break;
	case Profession_Chike:
		ret = getMinJie();
		//ret = pro.mMinJie;
		break;
	case Profession_Fashi:
		ret = getZhiLi();
		//ret = pro.mZhiLi;
		break;
	case Profession_Qishi:
		ret = getLiLiang();
		//ret = pro.mLiLiang;
		break;
	case Profession_ShenJianShou:
		ret = getMinJie();
		//ret = pro.mMinJie;
		break;
	case Profession_MiShu:
		ret = getZhiLi();
		break;
	case Profession_WuSeng:
		ret = getLiLiang();
		break;
	case Profession_YouXia:
		ret = getMinJie();
		break;
	case Profession_LieShou:
		ret = getMinJie();
		break;
	case Profession_WuShi:
		ret = getZhiLi();
		break;
	default:
		ret = 0;
		CCLog("Error : ScriptDataProvider--> Hero::getMainPropertyValue --> profession_id = %d.",pro.profession);
		CCAssert(false,"");
		break;
	}
	return ret;
}



//////////////////////////////////////////////////////////////////////////
//ScriptDataProvider
//////////////////////////////////////////////////////////////////////////

ScriptDataProvider::ScriptDataProvider(void)
{
}
ScriptDataProvider::~ScriptDataProvider(void)
{
}


#ifdef _CLIENT_

//#define _HERO_FIGHT_SELF_
#define _HERO_FIGHT_HERO_
//#define _HERO_FIGHT_GUAIWU_

#ifdef _HERO_FIGHT_SELF_
// 自己打自己的实现
BaseHero* ClientTestProvider::getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data)
{
	short cdTime;
	getHeroCDTime(cdTime,_heroId,user_data);
	// 直接取本地的英雄的数据
	vector<BaseActor*> actors = MyselfManager::getManager()->getTeamActors_ZhenShen(Actor_ShangZheng);
	int actorNum = actors.size();
	BaseActor* actor = actors[(_heroId-1)%actorNum];

	
	Hero* ret = new Hero();
	ret->init(actor->getCurrentProperty());
	// 不用装备
	ret->equipVec.clear();
	ret->skilVec.clear();

	switch (actor->getActorType())
	{
	case Profession_Yemanren:
		ret->skilVec.push_back(SkillDataForFight(SkillType_BaoLieZhan));
		break;
	case Profession_Chike:
		ret->skilVec.push_back(SkillDataForFight(SkillType_ShaLu));
		break;
	case Profession_Fashi:
		ret->skilVec.push_back(SkillDataForFight(SkillType_ShanDian));
		break;
	case Profession_Qishi:
		ret->skilVec.push_back(SkillDataForFight(SkillType_JianShou));
		break;
	case Profession_ShenJianShou:
		ret->skilVec.push_back(SkillDataForFight(SkillType_LieGong));
		break;
	case Profession_MiShu:
		ret->skilVec.push_back(SkillDataForFight(SkillType_HuoQiuShu));
		break;
	case Profession_WuSeng:
		ret->skilVec.push_back(SkillDataForFight(SkillType_ShenZhiXingNian));
		break;
	case Profession_YouXia:
		ret->skilVec.push_back(SkillDataForFight(SkillType_ShunYinTuXi));
		break;
	case Profession_LieShou:
		ret->skilVec.push_back(SkillDataForFight(SkillType_JianYu));
		break;
	case Profession_WuShi:
		ret->skilVec.push_back(SkillDataForFight(SkillType_WenYi));
		break;
	default:
		break;
	}

	vector<Equipment*> equips = actor->getEquipments();
	int equipsNum = equips.size();
	for (int i=0;i<equipsNum;i++)
	{
		ret->equipVec.push_back(equips[i]->getEquipData());
	}
	if (user_data.teamTag > 0)
	{
		// 回血
		if (((FightSUD*)user_data.data)->fightIndex > 0)
		{
			for (int i=0;i<FormationHeroAcountMax;i++)
			{
				if (((FightSUD*)user_data.data)->heroHpAfterLastFight.A_teamHeroId[i] == _heroId)
				{
					ret->zhuangbeiOpEnd();
					//修改血
					float hp = ret->getHp();
					hp *= 0.15f;
					hp += ((FightSUD*)user_data.data)->heroHpAfterLastFight.currentHp_A[i];
					if(hp > ret->getHp())
					{
						hp = ret->getHp();
					}
					ret->subHp(ret->getHp() - hp);
				}
			}
		}
		else
		{
			ret->zhuangbeiOpEnd();
		}
	}

	return ret;

}
#endif

#ifdef _HERO_FIGHT_HERO_
BaseHero* ClientTestProvider::getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data)
{
	
	short cdTime;
	getHeroCDTime(cdTime,_heroId,user_data);

	//const int index_offset = 0;
	//const int index_offset = 0;
	//Profession profession_pool[] = {
	//	Profession_Yemanren,Profession_Chike,Profession_Fashi,Profession_YouXia,Profession_Qishi,
	//	Profession_LieShou,Profession_ShenJianShou,Profession_MiShu,Profession_WuShi,Profession_WuSeng
	//};
	//SkillType skill_pool[] = {
	//	SkillType_BaoLieZhan,SkillType_ShaLu,SkillType_ShanDian,SkillType_LiRenHuiXuan,SkillType_JianShou,
	//	SkillType_JianYu,SkillType_LieGong,SkillType_HuoQiuShu,SkillType_WenYi,SkillType_ShenZhiXingNian
	//};
	//Profession proA[] = {profession_pool[index_offset+0],profession_pool[index_offset+1],profession_pool[index_offset+2],profession_pool[index_offset+3]};
	//Profession proB[] = {profession_pool[index_offset+4],profession_pool[index_offset+5],profession_pool[index_offset+6],profession_pool[index_offset+7]};
	//SkillType skill_a[] = {skill_pool[index_offset+0],skill_pool[index_offset+1],skill_pool[index_offset+2],skill_pool[index_offset+3]};
	//SkillType skill_b[] = {skill_pool[index_offset+4],skill_pool[index_offset+5],skill_pool[index_offset+6],skill_pool[index_offset+7]};

	Profession proA[] = {Profession_WuShi,Profession_WuSeng,Profession_ShenJianShou,Profession_Yemanren};
	Profession proB[] = {Profession_WuShi,Profession_WuSeng,Profession_ShenJianShou,Profession_Chike};
	SkillType skill_a[] = {SkillType_WenYi,SkillType_WenYi,SkillType_HuoQiuShu,SkillType_HuoQiuShu};
	SkillType skill_b[] = {SkillType_WenYi,SkillType_WenYi,SkillType_HuoQiuShu,SkillType_HuoQiuShu};

	SkillType* skill_A = skill_a;
	SkillType* skill_B = skill_b;
	float gongJiJuLi_A[] = {1,1,2,2};// 距离写出10，绝度全屏，只要大于对角线的长度就是了4.5f
	float gongJiJuLi_B[] = {1,10.0f,2,2};
	int guaiwu_x_size[] = {1,1,1,1};

	// A 队
	if (user_data.teamTag > 0)
	{
		//
		Hero* ret = new Hero();
		// test
		{
			// 不用装备
			ret->equipVec.clear();
			ret->skilVec.clear();

			HeroBaseProperty data;
			data.hero_id= _heroId;
			data.profession = proA[(_heroId-1) % 4];
			data.level = 10;

			data.exp_current = 20;
			data.exp_need = 2000;
			data.parent_hero_id = 0;
			data.postion = 1;
			data.vit_current = 100;

			data.mLiLiang = 4 + 4*lfRandom0_1();
			data.mMinJie = 500+6*lfRandom0_1();
			data.mZhiLi = 6 + 10*lfRandom0_1();
			data.mTiNeng = 0;
			data.vit_current = 50;
			data.mHP = 30;
			data.mGongJiJuLi = CS::getHeroGongjiJuLiByProfession(data.profession);
			data.mBaoJiLv = 100;
			data.mBaoJiShangHaiYinZi = 500;
			data.mJiNengFaChuLv = 0;
			data.mShangHaiLeiXing = SHANG_HAI_WU_LI;
			data.currentStage = 1;
			data.currentEffectPercent = 300;
			data.isYuanShen = false;

			ret->init(data);
			ret->setDefaultCdTime(cdTime);
			ret->skilVec.push_back(SkillDataForFight(skill_A[(_heroId-1) % 4]));

			// 加装备,测试
			{
				EquipData zhuangbei;
				zhuangbei.gongji_max = 2;
				zhuangbei.gongji_min = 1;
				zhuangbei.hole = 3;
				zhuangbei.hujia = 2;
				zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
				zhuangbei.equipDicrib = "equipDicrib";
				zhuangbei.zhuangbei_hero_id = 1;
				zhuangbei.zhuangbei_id = 2;
				zhuangbei.zhuangbei_level = 3;
				zhuangbei.equipName = "equipName";

				ZBAttr atti;
				atti.attr_type = AttrType_mingjie;
				atti.cuiqu_suc_ratio = 0;
				atti.attr_value = 1;
				zhuangbei.attr_vec.push_back(atti);

				if (data.profession == Profession_Yemanren)
				{
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_dsj_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_DanshouJian;
					ret->equipVec.push_back(zhuangbei);

					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_dunp_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Dunpai;
					ret->equipVec.push_back(zhuangbei);

					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_jz_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Jiezhi;
					ret->equipVec.push_back(zhuangbei);
					ret->equipVec.push_back(zhuangbei);


					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_hsf_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Hushenfu;
					ret->equipVec.push_back(zhuangbei);

// 					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
// 					zhuangbei.equipPic = "zb_kj_z_2";
// 					zhuangbei.zhuangbei_type = ZhuangbeiType_KuijiaZhong;
// 					ret->equipVec.push_back(zhuangbei);
				}
				else if(data.profession == Profession_Qishi)
				{
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_fu_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Dunpai;
					ret->equipVec.push_back(zhuangbei);
				}
				else if(data.profession == Profession_MiShu)
				{
					/*BaseActor* misu = NULL;
					{
					vector<BaseActor*> actors = MyselfManager::getManager()->getMyZhuShous_ZhenShen(Actor_ShangZheng);
					int size = actors.size();
					for (int i=0;i<size;i++)
					{
					if (actors[i]->getActorType() == Profession_MiShu)
					{
					misu = actors[i];
					break;
					}
					}
					}*/
					//// 找到就用外面的配置
					//if (misu)
					//{
					//	vector<Equipment*> equips = misu->getEquipments();
					//	int equipsNum = equips.size();
					//	for (int i=0;i<equipsNum;i++)
					//	{
					//		ret->equipVec.push_back(equips[i]->getEquipData());
					//	}
					//}
					//else
					{
						zhuangbei.zhuangbei_colour = ZhuangbeiColour_Baizhuang;
						zhuangbei.equipPic = "zb_fz_ss_1";
						zhuangbei.zhuangbei_type = ZhuangbeiType_ShuangshouFazhang;
						ret->equipVec.push_back(zhuangbei);
					}


				}
				else if(data.profession == Profession_ShenJianShou)
				{
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_gong_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Gong;
					ret->equipVec.push_back(zhuangbei);
				}
			}

			// 回血
			if ( ( (FightSUD*)user_data.data )->fightIndex > 0)
			{
				for (int i=0;i<FormationHeroAcountMax;i++)
				{
					if (( (FightSUD*)user_data.data )->heroHpAfterLastFight.A_teamHeroId[i] == _heroId)
					{
						ret->zhuangbeiOpEnd();
						//修改血
						float hp = ret->getHp();
						hp *= 0.15f;
						hp += ( (FightSUD*)user_data.data )->heroHpAfterLastFight.currentHp_A[i];
						if(hp > ret->getHp())
						{
							hp = ret->getHp();
						}
						ret->subHp(ret->getHp() - hp);
					}
				}
			}
			else
			{
				ret->zhuangbeiOpEnd();
			}
		}

		return ret;
	}
	else
	{
		//
		Hero* ret = new Hero();
		{
			// 不用装备
			ret->equipVec.clear();
			ret->skilVec.clear();

			HeroBaseProperty data;
			data.hero_id= _heroId;
			data.profession = proB[(_heroId-1) % 4];
			data.level = 1;

			data.exp_current = 20;
			data.exp_need = 2000;
			data.parent_hero_id = 0;
			data.postion = 1;
			data.vit_current = 100;

			data.mLiLiang = 4 + 4*lfRandom0_1();
			data.mMinJie = 5+6*lfRandom0_1();
			data.mZhiLi = 6 + 10*lfRandom0_1();
			data.mTiNeng = 0;
			data.vit_current = 200;
			data.mHP = 300;
			data.mGongJiJuLi = gongJiJuLi_B[(_heroId-1) % 4];
			data.mBaoJiLv = 20;
			data.mBaoJiShangHaiYinZi = 150;
			data.mJiNengFaChuLv = 0;
			data.mShangHaiLeiXing = SHANG_HAI_WU_LI;
			data.currentStage = 1;
			data.currentEffectPercent = 100;
			data.isYuanShen = false;

			ret->init(data);

			ret->setDefaultCdTime(cdTime);
			ret->skilVec.push_back(SkillDataForFight(skill_B[(_heroId-1) % 4]));
			ret->zhuangbeiOpEnd();


			if (((FightSUD*)user_data.data)->boss_freezy)
			{
				// boss不动的做法
				GameBuffEffect effect(-1);
				effect.xuanyun = true;
				effect.remove_pos = Pos_None;
				GameBuff buff(BuffState_Controller,0,0); 
				buff.effect_vec.push_back(effect);
				ret->attachBuff(buff);
			}
		}

		return ret;
	}
}
#endif

#ifdef _HERO_FIGHT_GUAIWU_
BaseHero* ClientTestProvider::getBaseHero(unsigned int _heroId,ScriptBuilderUserData user_data)
{
	short cdTime;
	getHeroCDTime(cdTime,_heroId,user_data);

	Profession proA[] = {Profession_Yemanren,Profession_Yemanren,Profession_Qishi,Profession_WuSeng};
	SkillType skill_A[] = {SkillType_BaoLieZhan,SkillType_BaoLieZhan,SkillType_JianShou,SkillType_ShenZhiXingNian};

	int proB[] = {23,24,40,40};
	float gongJiJuLi_B[] = {1,10.0f,2,2};
	int jinengchufalv[] = {100,100,100,100};
	int jinengchufalv_B[] = {0,0,0,0};
	int guaiwu_x_size[] = {1,1,1,1};

	// A 队
	if (user_data.teamTag > 0)
	{
		//
		Hero* ret = new Hero();
		// test
		{
			// 不用装备
			ret->equipVec.clear();
			ret->skilVec.clear();

			HeroBaseProperty data;
			data.hero_id= _heroId;
			data.profession = proA[(_heroId-1) % 4];
			data.level = 10;

			data.exp_current = 20;
			data.exp_need = 2000;
			data.parent_hero_id = 0;
			data.postion = 1;
			data.vit_current = 100;

			data.mLiLiang = 4 + 4*lfRandom0_1();
			data.mMinJie = 500+6*lfRandom0_1();
			data.mZhiLi = 6 + 10*lfRandom0_1();
			data.mTiNeng = 0;
			data.vit_current = 200;
			data.mHP = 200;
			data.mGongJiJuLi = CS::getHeroGongjiJuLiByProfession(data.profession);
			data.mBaoJiLv = 100;
			data.mBaoJiShangHaiYinZi = 500;
			data.mJiNengFaChuLv = jinengchufalv[(_heroId-1) % 4];
			data.mShangHaiLeiXing = SHANG_HAI_WU_LI;
			data.currentStage = 1;
			data.currentEffectPercent = 300;
			data.isYuanShen = false;

			ret->init(data);
			ret->setDefaultCdTime(cdTime);
			ret->skilVec.push_back(SkillDataForFight(skill_A[(_heroId-1) % 4]));

			// 加装备,测试
			{
				EquipData zhuangbei;
				zhuangbei.gongji_max = 2;
				zhuangbei.gongji_min = 1;
				zhuangbei.hole = 3;
				zhuangbei.hujia = 2;
				zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
				zhuangbei.equipDicrib = "equipDicrib";
				zhuangbei.zhuangbei_hero_id = 1;
				zhuangbei.zhuangbei_id = 2;
				zhuangbei.zhuangbei_level = 3;
				zhuangbei.equipName = "equipName";

				ZBAttr atti;
				atti.attr_type = AttrType_mingjie;
				atti.cuiqu_suc_ratio = 0;
				atti.attr_value = 1;
				zhuangbei.attr_vec.push_back(atti);

				if (data.profession == Profession_Yemanren)
				{
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_dsj_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_DanshouJian;
					ret->equipVec.push_back(zhuangbei);

					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_dunp_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Dunpai;
					ret->equipVec.push_back(zhuangbei);

					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_jz_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Jiezhi;
					ret->equipVec.push_back(zhuangbei);
					ret->equipVec.push_back(zhuangbei);


					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_hsf_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Hushenfu;
					ret->equipVec.push_back(zhuangbei);

					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_hsf_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_KuijiaZhong;
					ret->equipVec.push_back(zhuangbei);
				}
				else if(data.profession == Profession_Qishi)
				{
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_fu_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Dunpai;
					ret->equipVec.push_back(zhuangbei);
				}
				else if(data.profession == Profession_MiShu)
				{
					BaseActor* misu = NULL;
					{
						vector<BaseActor*> actors = MyselfManager::getManager()->getMyZhuShous_ZhenShen(Actor_ShangZheng);
						int size = actors.size();
						for (int i=0;i<size;i++)
						{
							if (actors[i]->getActorType() == Profession_MiShu)
							{
								misu = actors[i];
								break;
							}
						}
					}

					// 找到就用外面的配置
					if (misu)
					{
						vector<Equipment*> equips = misu->getEquipments();
						int equipsNum = equips.size();
						for (int i=0;i<equipsNum;i++)
						{
							ret->equipVec.push_back(equips[i]->getEquipData());
						}
					}
					else
					{
						zhuangbei.zhuangbei_colour = ZhuangbeiColour_Baizhuang;
						zhuangbei.equipPic = "zb_fz_ss_1";
						zhuangbei.zhuangbei_type = ZhuangbeiType_ShuangshouFazhang;
						ret->equipVec.push_back(zhuangbei);
					}

					
				}
				else if(data.profession == Profession_ShenJianShou)
				{
					zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
					zhuangbei.equipPic = "zb_gong_1";
					zhuangbei.zhuangbei_type = ZhuangbeiType_Gong;
					ret->equipVec.push_back(zhuangbei);
				}
			}

			// 回血
			if ( ((FightSUD*)user_data.data)->fightIndex > 0)
			{
				for (int i=0;i<FormationHeroAcountMax;i++)
				{
					if (((FightSUD*)user_data.data)->heroHpAfterLastFight.A_teamHeroId[i] == _heroId)
					{
						ret->zhuangbeiOpEnd();
						//修改血
						float hp = ret->getHp();
						hp *= 0.15f;
						hp += ((FightSUD*)user_data.data)->heroHpAfterLastFight.currentHp_A[i];
						if(hp > ret->getHp())
						{
							hp = ret->getHp();
						}
						ret->subHp(ret->getHp() - hp);
					}
				}
			}
			else
			{
				ret->zhuangbeiOpEnd();
			}
		}

		return ret;
	}
	else
	{
		
		//
		Guaiwu* ret = new Guaiwu();
		GuaiWuData g_Data;
		g_Data.level = 1;
		g_Data.gongJiLi = 18 + 20*lfRandom0_1();
		//g_Data.xueLiang = 4000+ 300*lfRandom0_1()+_heroId;
		g_Data.xueLiang = 10+ 30*lfRandom0_1()+_heroId;
		g_Data.baoJiLv = 35;
		g_Data.baoShangYinZi = 120;
		g_Data.jiNengChuFaLv = jinengchufalv_B[(_heroId-1) % 4];
		g_Data.huJiaJianMianLv = 20;
		g_Data.kangXingJianMianLv = 10 + 5*lfRandom0_1();
		g_Data.gongJiJuLi = gongJiJuLi_B[(_heroId-1) % 4];;
		g_Data.shangHaiLeiXing = SHANG_HAI_WU_LI;
		g_Data.profession = (Profession)proB[(_heroId-1) % 4];
		g_Data.x_size = guaiwu_x_size[(_heroId-1) % 4];
		g_Data.type = ((_heroId-1) % 4)==1?Type_Guaiwu_jingying:Type_Guaiwu_putong;
		ret->initWithGuaiwuData(g_Data);
		ret->setDefaultCdTime(cdTime);
		ret->skilVec.push_back(SkillType_LieGong);
		
		if (((FightSUD*)user_data.data)->boss_freezy)
		{
			// boss不动的做法
			GameBuffEffect effect(-1);
			effect.xuanyun = true;
			effect.remove_pos = Pos_None;
			GameBuff buff(BuffState_Controller,0,0); 
			buff.effect_vec.push_back(effect);
			ret->attachBuff(buff);
		}

		return ret;
	}
}
#endif

void ClientTestProvider::getTeamBloodNum(short& currentNum,short& defaultNum,int& bloodAddPercent,ScriptBuilderUserData data)
{
	currentNum = 0;
	defaultNum = 0;
	bloodAddPercent = 30;		// 30%

	if (data.teamTag == TEAM_TAG_A)
	{
		if (( (FightSUD*)data.data )->fightIndex == 0)
		{
			currentNum = 4;
		}
		else // 沿用上次剩下的血瓶数量
		{
			currentNum = ( (FightSUD*)data.data )->heroHpAfterLastFight.A_teamCurrentBloodNum;
		}
		defaultNum = 4;
	}
}
void ClientTestProvider::getHeroCDTime(short& cdStepNum,unsigned heroId,ScriptBuilderUserData data)
{
	cdStepNum = 0;

	if (data.teamTag == TEAM_TAG_A)
	{
		cdStepNum = 3;
	}
}
void ClientTestProvider::getHeroDetail(HeroDetail& outData,unsigned int _heroId,ScriptBuilderUserData data)
{
	memset(&outData,0,sizeof(HeroDetail));
	// test
	{
		outData.basic_info.hero_id = _heroId;
		outData.basic_info.level = 1;
		outData.basic_info.vip_level = 1;
		strcpy(outData.basic_info.name,"");
		outData.basic_info.profession_id = Profession_Yemanren;

		outData.parent_hero_id = 0;
		outData.postion = 1;
		outData.exp_current = 0;
		outData.exp_need = 1;
		outData.vit_current = 10;

		outData.hero_attr_without_zhuangbei.mingjie = 10;
		outData.hero_attr_without_zhuangbei.liliang = 10;
		outData.hero_attr_without_zhuangbei.zhili = 10;
		outData.hero_attr_without_zhuangbei.tineng = 10;

		outData.hero_attr_without_zhuangbei.xueliang = 10;
		//outData.hero_attr_without_zhuangbei.gongji_min = 10;
		//outData.hero_attr_without_zhuangbei.gongji_max = 10;
		outData.hero_attr_without_zhuangbei.baoji = 10;
		outData.hero_attr_without_zhuangbei.baoshang = 10;
		outData.hero_attr_without_zhuangbei.jinengchufa = 10;
	}

}

unsigned int ClientTestProvider::fightEndCallback(ScriptBuilderUserData data,bool win)
{
	return 0;
}

vector<ItemSuipianInfo> ClientTestProvider::getFightEndItemSuiPians(ScriptBuilderUserData data,bool win)
{
	vector<ItemSuipianInfo> null_vec;
	return null_vec;

	ItemSuipianInfo temp;
	temp.item_id = ItemsType_SkillBook_CiSha;
	temp.suipian_id = 1;
	temp.suipian_count = 1;
	null_vec.push_back(temp);

	temp.suipian_id = 2;
	temp.suipian_count = 2;
	null_vec.push_back(temp);

	temp.suipian_id = 3;
	temp.suipian_count = 1;
	null_vec.push_back(temp);

	return null_vec;
}
vector<ShenbingSuipianInfo> ClientTestProvider::getFightEndShenBingSuiPians(ScriptBuilderUserData data,bool win)
{
	vector<ShenbingSuipianInfo> null_vec;
	return null_vec;

	ShenbingSuipianInfo temp;
	temp.zb_group_id = 1604; // 套装
	temp.suipian_id = 1;
	temp.suipian_count = 2;
	null_vec.push_back(temp);

	temp.zb_group_id = 1601; // 一般的
	temp.suipian_id = 2;
	temp.suipian_count = 2;
	null_vec.push_back(temp);

	temp.suipian_id = 3;
	temp.suipian_count = 2;
	null_vec.push_back(temp);

	return null_vec;
}

// 获取掉的装备
// 服务器必须得填
// 还得把数据持久化到数据库
vector<EquipData> ClientTestProvider::getFightEndEquips(ScriptBuilderUserData data,bool win)
{
	vector<EquipData> test_data;
	return test_data;

	EquipData zhuangbei;
	zhuangbei.diamond_type_1 = 0;
	zhuangbei.diamond_type_2 = 0;
	zhuangbei.diamond_type_3 = 0;
	zhuangbei.gongji_max = 0;
	zhuangbei.gongji_min = 0;
	zhuangbei.hole = 0;
	zhuangbei.hujia = 0;
	zhuangbei.unknownHideAttriNum = 0;
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	zhuangbei.equipDicrib = "equipDicrib";
	zhuangbei.zhuangbei_hero_id = 0;
	zhuangbei.zhuangbei_id = 0;
	zhuangbei.zhuangbei_level = 0;
	zhuangbei.equipName = "equipName";
	zhuangbei.equipPic = "zb_fu_1";
	zhuangbei.zhuangbei_type = ZhuangbeiType_Dunpai;

	{
		ZBAttr atti;
		atti.attr_type = AttrType_mingjie;
		atti.cuiqu_suc_ratio = 10;
		atti.attr_value = 20;
		zhuangbei.attr_vec.push_back(atti);
	}
	
	{
		ZBAttr atti;
		atti.attr_type = AttrType_zhili;
		atti.cuiqu_suc_ratio = 5;
		atti.attr_value = 10;
		zhuangbei.attr_vec.push_back(atti);
	}

	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Fumo;
	//test_data.push_back(zhuangbei);
	test_data.push_back(zhuangbei);

	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Xiyou;
	test_data.push_back(zhuangbei);
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Xiyou;
	test_data.push_back(zhuangbei);
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Baizhuang;
	test_data.push_back(zhuangbei);
	zhuangbei.group_id = 1601;
	zhuangbei.isShenBing = true;
	zhuangbei.unknownHideAttriNum = 5;
	zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	test_data.push_back(zhuangbei);

	//zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	//zhuangbei.group_id = 1601;
	//test_data.push_back(zhuangbei);

	//zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	//zhuangbei.group_id = 1602;
	//test_data.push_back(zhuangbei);

	//zhuangbei.zhuangbei_colour = ZhuangbeiColour_Chuanqi;
	//zhuangbei.group_id = 1602;
	//test_data.push_back(zhuangbei);

	return test_data;
}
// 获取掉的物品
// 服务器必须得填
// 还得把数据持久化到数据库
vector<ItemInfo> ClientTestProvider::getFightEndItems(ScriptBuilderUserData data,bool win)
{
	vector<ItemInfo> test_data;
	return test_data;

	ItemInfo item;
	item.count = 1;
	item.type = ItemsType_QianghuaNormal;

	test_data.push_back(item);
	test_data.push_back(item);

	return test_data;

	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);
	test_data.push_back(item);

	return test_data;
}

vector<PowerPoint> ClientTestProvider::getFightEndPowerPoints(ScriptBuilderUserData data,bool win)
{
	vector<PowerPoint> ret;
	return ret;

	PowerPoint pp;
	pp.power = 5;
	ret.push_back(pp);
	ret.push_back(pp);
	ret.push_back(pp);
	ret.push_back(pp);

	return ret;
}

void ClientTestProvider::teamFightEnd(bool win,TeamEndParam parm,TeamEndOutParam& gettedItems)
{

}

#endif	// _CLIENT_