#pragma once
#include "CommonDefine.h"
#include "GameBuff.h"

class TeamHero;
struct SkillDataForFight;
struct CalculateData;
class EquipCalculator;
class BuffCalculator;
class Guaiwu;

// 值域
enum ValueRange
{
	Range_Ramdom = 0,	// 随机值
	Range_Max,			// 最大值
	Range_Average,		// 平均值
	Range_Min,			// 最小值
};
// 
class PropertyCalculator
{
private:
	float calculateHuJiaZhi(unsigned int baseHeroLiliang,vector<EquipData> allEquips,list<GameBuff> allBuffs) const;
	float calculateGongJiLi_MacroValue(const CalculateData& attack,bool skillOK, const SkillDataForFight& skill, ValueRange range) const;
	float calculateZhanDouLi(const CalculateData& attack,bool skillOK, const SkillDataForFight& skill,ValueRange range) const;

public:
	PropertyCalculator(void);
	PropertyCalculator(const PropertyCalculator& cal);
	PropertyCalculator& operator = (const PropertyCalculator& rightData); //赋值符重载

	// 计算actor的护甲值
	float calculateHuJiaZhi(const CalculateData& actor) const;
	
	// 计算beAttacked的护甲减免率,减免物理攻击
	// 需要知道攻击者(fightor)的属性
	unsigned int calculateHuJiaJianMianLv(int fightorLevel,const CalculateData& beAttacked) const;
	unsigned int calculateHuJiaJianMianLv(int fightorLevel,float hujiazhi) const;
	// 计算抗性减免率,减免魔法攻击
	unsigned int calculateKangXingMianLv(int fightLevel,const CalculateData& beAttacked) const;
	unsigned int calculateKangXingMianLv(int fightLevel,float kangxingzhi) const;
	// 计算物理的格挡值
	float calculateWuLiGeDangZhi(const CalculateData& beAttacked) const;
	// 计算格挡率
	unsigned int calculateGeDangLv(const CalculateData& beAttacked) const;
	// 计算抗性值
	float calculateKuangXingZhi(const CalculateData& beAttacked) const;
	// 计算actor的闪避率
	unsigned int  calculateShanBiLv(const CalculateData& actor) const;
	// 计算暴击率
	unsigned int calculateBaoJiLv(const CalculateData& actor) const;
	//
	unsigned int calculateJiNengChuFaLv(const CalculateData& actor) const;
	unsigned int calculateJiNengZhouQi(const CalculateData& actor) const;
	unsigned int calculateJiNengTurn(const CalculateData& actor) const;
	unsigned int calculateJiNengResult(const CalculateData& actor) const;


	// 暴击因子
	unsigned int calculateBaoJiShangHaiYinzi(const CalculateData& actor) const;

	float calculateLiliang(const CalculateData& actor) const;
	float calculateMinJie(const CalculateData& actor) const;
	float calculateZhili(const CalculateData& actor) const;
	float calculateTineng(const CalculateData& actor) const;
	float calculateHp(const CalculateData& actor) const;

	// 计算攻击力：就是当前角色的攻击力，就是一般攻击的能力,得到的就是数值
	float calculateGongJiLi(const CalculateData& attack,ValueRange range = Range_Ramdom) const;

	// 计算普适的技能伤害
	// 技能伤害=当前技能的效果倍数*角色当前攻击力*升级效果
	//
	// 因为不是所有的技能都是攻击，所有我们大致则算出他的伤害
	// 这就是计算的地方
	float calculateSkillShangHai(float actorPutongGongJiLi,const SkillDataForFight& skill) const;
	//角色宏观攻击力=角色当前攻击力+（暴击几率*暴击伤害因子*攻击力）+（1/技能触发周期*技能伤害）
	float calculateGongJiLi_MacroValue(const CalculateData& attack,ValueRange range = Range_Ramdom) const;
	
	// 计算承受力
	// 角色承受力=角色当前血量/(1-角色当前护甲减免率(用角色自己的等级折算))
	float calculateChengShouLi(const CalculateData& attack)const;
	
	// 计算战斗力
	float calculateZhanDouLi(const CalculateData& attack,ValueRange range = Range_Ramdom) const;
	
	// 计算战斗力
	// 角色战斗力=角色承受力+4*角色宏观攻击力
	float calculateZhanDouLi_2(const CalculateData& attack,ValueRange range = Range_Ramdom) const;
	//
	float calculateHuJiaZhi_MacroValue(const CalculateData& actor) const;
	float calculateKuangXingZhi_MacroValue(const CalculateData& beAttacked) const;

	// 计算一个角色攻击另一个角色，产生的伤害值,得到的就是数值
	// 根据角色类型，自动确定是物理还是，魔法伤害
	// 只是攻击的伤害值，没有技能，要计算暴击，就在这个基础上计算即可
	// 综合了攻击力，护甲减免，抗性减免，格挡值，就是直接需要减去的值
	float calculateAttackShangHaiValue(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	// 计算attack攻击beAttacked照成的物理伤害
	float calculateWuLiShangHai(float gongjiLi,int huJiaJianMianLv,int gedanglv,float gedangzhi,int jiansahnglv) const;
	float calculateWuLiShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	// 计算attack攻击beAttacked照成的魔法伤害
	float calculateMoFaShangHai(float gongjiLi,int huJiaJianMianLv,int kangxingJianMianLv,int jiansahnglv) const;
	float calculateMoFaShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	// 计算伤害值
	float calculateAttackShangHaiValue(const CalculateData& attack,const Guaiwu* beAttacked,ValueRange range = Range_Ramdom) const;
	float calculateAttackShangHaiValue(const Guaiwu* attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	float calculateAttackShangHaiValue(const Guaiwu* attack,const Guaiwu* beAttacked,ValueRange range = Range_Ramdom) const;
	float calculateAttackShangHaiValue(const Guaiwu* attack,const TeamHero* beAttacked,ValueRange range = Range_Ramdom) const;
	float calculateAttackShangHaiValue(const TeamHero* attack,const Guaiwu* beAttacked,ValueRange range = Range_Ramdom) const;

	~PropertyCalculator(void);
private:
	EquipCalculator* euipCal;
	BuffCalculator* buffCal;
};

// 装备的计算
// 一个vector<EquipData> equipVec里面的数据，如果有套装的就会计算套装效果
class EquipCalculator
{
public:
	EquipCalculator();

	float getEquipHuJiaZhi(vector<EquipData> equipVec) const;
	float getEquipGeDangZhi(vector<EquipData> equipVec) const;
	unsigned int getEquipGeDangLv(vector<EquipData> equipVec) const;
	float getEquipKangXingZhi(vector<EquipData> equipVec) const;
	void getEquipGongJiLi(vector<EquipData> equipVec,float& gongji_min,float& gongji_max,float& gongji_min_add,float& gongji_max_add) const;
	float getEquipLiliang(vector<EquipData> equipVec) const;
	float getEquipMingJie(vector<EquipData> equipVec) const;
	float getEquipZhili(vector<EquipData> equipVec) const;
	float getEquipTineng(vector<EquipData> equipVec) const;
	unsigned int getEquipBaojiLv(vector<EquipData> equipVec) const;
	unsigned int getEquipBaoshangYinZi(vector<EquipData> equipVec) const;
	unsigned int getEquipHp_ratio(vector<EquipData> equipVec) const;
	unsigned int getEquipGongJiAdd_ratio(vector<EquipData> equipVec) const;
	unsigned int getSkillRatioAdd(vector<EquipData> equipVec) const;
	unsigned int getSkillZhouQiSub(vector<EquipData> equipVec) const; // 计算技能的触发周期缩短值
	unsigned int getSkillTurnSub(vector<EquipData> equipVec) const;	// 计算技能的触发点缩短值
	unsigned int getSkillResultAddPercent(vector<EquipData> equipVec) const;

	void getEquipBaoShiJiaCheng(vector<EquipData> equipVec,float& liLiang,float& minJie,float& zhiLi,float& tiNeng) const;

};

class BuffCalculator
{
public:
	BuffCalculator();
	~BuffCalculator();
	// 正数就是加，负数就是减
	int getBuffBaoJiLv(list<GameBuff> buffVec) const;
	int getBuffShanBiLv(list<GameBuff> buffVec) const;
	float getBuffHuJiaZhi(list<GameBuff> buffVec) const;
	// 只减少物理伤害
	int getBuffJianShangLv(list<GameBuff> buffVec) const;
	// 攻击力提升百分比
	int getGongJiLiAddLv(list<GameBuff> buffVec) const;
};
