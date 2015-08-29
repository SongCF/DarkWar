#pragma once
#include "CommonDefine.h"
#include "GameBuff.h"

class TeamHero;
struct SkillDataForFight;
struct CalculateData;
class EquipCalculator;
class BuffCalculator;
class Guaiwu;

// ֵ��
enum ValueRange
{
	Range_Ramdom = 0,	// ���ֵ
	Range_Max,			// ���ֵ
	Range_Average,		// ƽ��ֵ
	Range_Min,			// ��Сֵ
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
	PropertyCalculator& operator = (const PropertyCalculator& rightData); //��ֵ������

	// ����actor�Ļ���ֵ
	float calculateHuJiaZhi(const CalculateData& actor) const;
	
	// ����beAttacked�Ļ��׼�����,����������
	// ��Ҫ֪��������(fightor)������
	unsigned int calculateHuJiaJianMianLv(int fightorLevel,const CalculateData& beAttacked) const;
	unsigned int calculateHuJiaJianMianLv(int fightorLevel,float hujiazhi) const;
	// ���㿹�Լ�����,����ħ������
	unsigned int calculateKangXingMianLv(int fightLevel,const CalculateData& beAttacked) const;
	unsigned int calculateKangXingMianLv(int fightLevel,float kangxingzhi) const;
	// ��������ĸ�ֵ
	float calculateWuLiGeDangZhi(const CalculateData& beAttacked) const;
	// �������
	unsigned int calculateGeDangLv(const CalculateData& beAttacked) const;
	// ���㿹��ֵ
	float calculateKuangXingZhi(const CalculateData& beAttacked) const;
	// ����actor��������
	unsigned int  calculateShanBiLv(const CalculateData& actor) const;
	// ���㱩����
	unsigned int calculateBaoJiLv(const CalculateData& actor) const;
	//
	unsigned int calculateJiNengChuFaLv(const CalculateData& actor) const;
	unsigned int calculateJiNengZhouQi(const CalculateData& actor) const;
	unsigned int calculateJiNengTurn(const CalculateData& actor) const;
	unsigned int calculateJiNengResult(const CalculateData& actor) const;


	// ��������
	unsigned int calculateBaoJiShangHaiYinzi(const CalculateData& actor) const;

	float calculateLiliang(const CalculateData& actor) const;
	float calculateMinJie(const CalculateData& actor) const;
	float calculateZhili(const CalculateData& actor) const;
	float calculateTineng(const CalculateData& actor) const;
	float calculateHp(const CalculateData& actor) const;

	// ���㹥���������ǵ�ǰ��ɫ�Ĺ�����������һ�㹥��������,�õ��ľ�����ֵ
	float calculateGongJiLi(const CalculateData& attack,ValueRange range = Range_Ramdom) const;

	// �������ʵļ����˺�
	// �����˺�=��ǰ���ܵ�Ч������*��ɫ��ǰ������*����Ч��
	//
	// ��Ϊ�������еļ��ܶ��ǹ������������Ǵ�������������˺�
	// ����Ǽ���ĵط�
	float calculateSkillShangHai(float actorPutongGongJiLi,const SkillDataForFight& skill) const;
	//��ɫ��۹�����=��ɫ��ǰ������+����������*�����˺�����*��������+��1/���ܴ�������*�����˺���
	float calculateGongJiLi_MacroValue(const CalculateData& attack,ValueRange range = Range_Ramdom) const;
	
	// ���������
	// ��ɫ������=��ɫ��ǰѪ��/(1-��ɫ��ǰ���׼�����(�ý�ɫ�Լ��ĵȼ�����))
	float calculateChengShouLi(const CalculateData& attack)const;
	
	// ����ս����
	float calculateZhanDouLi(const CalculateData& attack,ValueRange range = Range_Ramdom) const;
	
	// ����ս����
	// ��ɫս����=��ɫ������+4*��ɫ��۹�����
	float calculateZhanDouLi_2(const CalculateData& attack,ValueRange range = Range_Ramdom) const;
	//
	float calculateHuJiaZhi_MacroValue(const CalculateData& actor) const;
	float calculateKuangXingZhi_MacroValue(const CalculateData& beAttacked) const;

	// ����һ����ɫ������һ����ɫ���������˺�ֵ,�õ��ľ�����ֵ
	// ���ݽ�ɫ���ͣ��Զ�ȷ���������ǣ�ħ���˺�
	// ֻ�ǹ������˺�ֵ��û�м��ܣ�Ҫ���㱩����������������ϼ��㼴��
	// �ۺ��˹����������׼��⣬���Լ��⣬��ֵ������ֱ����Ҫ��ȥ��ֵ
	float calculateAttackShangHaiValue(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	// ����attack����beAttacked�ճɵ������˺�
	float calculateWuLiShangHai(float gongjiLi,int huJiaJianMianLv,int gedanglv,float gedangzhi,int jiansahnglv) const;
	float calculateWuLiShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	// ����attack����beAttacked�ճɵ�ħ���˺�
	float calculateMoFaShangHai(float gongjiLi,int huJiaJianMianLv,int kangxingJianMianLv,int jiansahnglv) const;
	float calculateMoFaShangHai(const CalculateData& attack,const CalculateData& beAttacked,ValueRange range = Range_Ramdom) const;
	// �����˺�ֵ
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

// װ���ļ���
// һ��vector<EquipData> equipVec��������ݣ��������װ�ľͻ������װЧ��
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
	unsigned int getSkillZhouQiSub(vector<EquipData> equipVec) const; // ���㼼�ܵĴ�����������ֵ
	unsigned int getSkillTurnSub(vector<EquipData> equipVec) const;	// ���㼼�ܵĴ���������ֵ
	unsigned int getSkillResultAddPercent(vector<EquipData> equipVec) const;

	void getEquipBaoShiJiaCheng(vector<EquipData> equipVec,float& liLiang,float& minJie,float& zhiLi,float& tiNeng) const;

};

class BuffCalculator
{
public:
	BuffCalculator();
	~BuffCalculator();
	// �������Ǽӣ��������Ǽ�
	int getBuffBaoJiLv(list<GameBuff> buffVec) const;
	int getBuffShanBiLv(list<GameBuff> buffVec) const;
	float getBuffHuJiaZhi(list<GameBuff> buffVec) const;
	// ֻ���������˺�
	int getBuffJianShangLv(list<GameBuff> buffVec) const;
	// �����������ٷֱ�
	int getGongJiLiAddLv(list<GameBuff> buffVec) const;
};
