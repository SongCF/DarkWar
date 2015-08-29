#pragma once

#include "Equipment.h"
#include <string>
#include <vector>
#include <list>
#include "BaseSprite.h"
#include "LFUtile.h"
#include "cocos2d.h"
#include "CommonDefine.h"
#include "CalculateData.h"
#include "SkillTree.h"
#include "AutoReleaseAble.h"

class PropertyCalculator;
class SkillData;

using namespace cocos2d;

using namespace std;

/**
 *	游戏的角色类
 *	包括他的全局唯一的Id
 *	所属于的主人的Id
 *	只是游戏角色的直属信息。
 *
 *	游戏角色的武器，装备，穿着等由其他单独的类定义。
 *	他们的关系由关系类来标记。中介者模式。

 *	不应该直接继续BaseSprite，而是应该直接连带BaseSprite
 */
class BaseActor : public CCObject,AutoReleaseAble
{
	friend class MyselfManager;

private:
	PropertyCalculator* mCalulator;
	BaseActor* mappingBody;		// 映射的角色，是元神或者是真身
protected:
	SkillData* mCurrentUsedSkill;
	vector<SkillData*> mSkillVec;			// 拥有的技能,也就是学会的技能
	vector<Equipment*> mEquipVec;			// 装备上的装备
	ResourcePathManager* mResMgr;

	BaseActorProperty mBaseProperty;

	// 获取mainPro是0的Data,用于获取4大属性用的：体能，智力，敏捷，力量
	CalculateData getCalculateDataWithMainProZero() const;
public:
	BaseActor(BaseActorProperty p);

	void initSkill(vector<SkillData*> skills);
	void initEquipment(vector<Equipment*> items);

	// 设置元神/真身,任意一个方法都会触发另一个方法
	// 会相互关联的
	void setYuanShen(BaseActor* yuanshen);
	void setZhenShen(BaseActor* zhenshen);

	EquipRequirePlace getPlaceToEquip(Equipment* equip);

	// 要之前都跟服务器交流完毕，成功了，才能在本地添加
	void attachEquipment(Equipment* equip);
	// 移除一个装备，也需要服务器交流
	void detachEuipment(Equipment* equip);
	//
	void detatchAllEquipment();

	//// 得到一个装备
	//// 需要知道装备的类型：武器，防具，饰品
	//// 需要知道该装备的小类别，也就是装备的位置
	//vector<Equipment*> getEquipment(EquipType type,int typeId);

	// 获取放在place的武器
	Equipment* getEquipmentAt(EquipRequirePlace place);

	// 计算如果装上一个装备，那么需要卸下的装备
	vector<Equipment*> getNeedTakeOffIfWareOnEqui(ZhuangbeiType type,EquipRequirePlace needPlace);
	vector<Equipment*> getEquipments(){return mEquipVec;};
	// 添加技能,新学会的，如果该技能是已装备的，那么就装备上
	void attatchSkill(SkillData* skill);
	//// 移除技能
	//void detachSkill(SkillData* skill);

	// 设置和获取当前使用的技能
	void setCurrentUseSkill(SkillData* skill);
	SkillData* getCurrentUseSkill();

	// 获得当前技能的信息，包含了装备的影响数据的
	unsigned int getCurrentSkillZhouQi();
	unsigned int getCurrentSkillTurn();
	unsigned int getCurrentSkillResultPercent();

	int getSkillNum(){return mSkillVec.size();};
	// 学会的技能
	vector<SkillData*> getAllSkills(){return mSkillVec;};

#pragma region 快速获取属性
	HeroPropertySummery getSummeryProperty() const;
	// 获得当前状态的CalculateData
	CalculateData getCalculateData() const;
	// 计算如果装上了toEquip这个装备之后的CalculateData
	// 如果指定了willBeRepalced，那么就直接模拟toEquip替换willBeRepalced
	// 但是toEquip跟willBeRepalced必须是同一个类型的才可以替换
	// 如果willBeRepalced为空，那么就直接根据要穿上的脱去对应的就是了，对于戒指，必须指定
	CalculateData getCalculateDataWhenEquipOne(Equipment* toEquip,Equipment* willBeRepalced = NULL);
	unsigned int getId(){return mBaseProperty.hero_id;};
	unsigned int getParId(){return mBaseProperty.parent_hero_id;};
	unsigned int getChessBoardPos(){return mBaseProperty.postion;};
	// 是基础数据，如果想得到比如等级，角色类型什么的就用这个方法
	BaseActorProperty getCurrentProperty() const;
	// 只读
	const BaseActorProperty* getCurrentProperty_R() const;

	int getLevel();
	bool canUnlock();
	bool isLocked();
	void setLocked(bool isLocked);
	void setCanUnlock(bool canUnlock);

	Profession getActorType();
	// 获得主属性的值
	unsigned int getMainPropertyValue() const;
	float getXueliang() const;
	float getGongji_MacroValue() const;	// 宏观的攻击值 
	float getGongji() const;
	float getHujia_MacroValue() const;	// 宏观的护甲值
	float getHujia() const;
	float getMokang_MacroValue() const;	// 宏观的魔抗值
	float getMokang() const;
	float getMyZhanDouLi(int range) const;
	unsigned int getBaoji() const;
	unsigned int getBaoshang() const;
	float getMinjie() const;
	float getLiliang() const;
	float getZhili() const;
	float getTineng() const;
	unsigned int getVipLevel() const {return mBaseProperty.vip_level;};
	// 得到当前经验数据
	void getExpInfor(unsigned int& out_cur_exp,unsigned int & out_exp_need) const;
	unsigned int getJinengChufaLv() const;
	float getGedangZhi() const;
	unsigned int getGedangLv() const;
	unsigned int getShanbiLv() const;
	// 获取当前的阶
	unsigned int getCurrentStage()const;
	// 是否可以被洗礼
	bool canBeXiLiByCurrentJiaoTang()const;
	// 是否是元神
	bool isYuanShen() const;

	// 取得元神，如果本来就是元神，返回NULL
	BaseActor* getMyYuanShen() const;
	// 取得真身，如果本来是真身就返回NULL
	BaseActor* getMyZhenShen() const;
#pragma endregion


#pragma region 修改数据接口
	// 设置位置
	void setActorBuZhengPos(unsigned int posIndex);
	// 刷新现在的基本数据
	void refreshBaseProPerty(HeroDetail data);
	void refreshBaseProPerty(BaseActorProperty data);
	// 设置体力
	void setVitality(int value);
	//
	void setExpInfor(unsigned int cur_exp);
	// 升级
	void actorLevelAdd(int toAdd);
	// 设置vip等级
	void setVipLevel(unsigned int level){mBaseProperty.vip_level = level;};
	// 设置阶
	void setCurrentStage(unsigned int stage);
#pragma endregion
	// 是否是助手
	bool isHelper();
	bool canUpdateLevel(int &clientErrorCode);
	bool canUpdateStage(int &clientErrorCode);
	~BaseActor(void);
};
