#pragma once

#include "protocol.h"
#include <vector>
#include "ScriptDataProvider.h"

using namespace std;

#define		NO_MOVE				-1
#define		PVE_MAX_STEP_NUM	100		// pve��ʱ��ÿһ�������ٲ�

//// ���������
//enum SpecialControl
//{
//	SC_NONE = 0,		// û��������ƣ�����һ���ս��
//	SC_BOSS_FREEZE,	// boss����
//};


enum StepType
{
	StepType_Fight=1,
	StepType_Move,
	StepType_Skill,
	StepType_FailByTakeTooMuchTime,	// ��ʱ��̫�ã�ֱ������
	StepType_Wait,					// �ȴ���ʲôҲ��������ռ����
	StepType_UseBlood,				// ʹ��Ѫƿ����ռ����
	StepType_ChangeRound,			// ��һ������

	StepType_ReLoadBlood,			// �ŶӴ�boss������,��StepUseBlood�����ݽṹ
};
enum FightPattern
{
	Fight_Normal = 0,
	Fight_Normal_BaoJi,
};
enum SkillPattern
{
	Skill_Noraml = 0,
	Skill_Noraml_BaoJI,
};
enum FightResult
{
	Result_Win = 0,		// �ɹ�
	Result_Lose,		// ʧ��
};

struct StepMove
{
	unsigned int uniqueId;	// ˭�ƶ�
	int destIndex;			// Ŀ�� ������λ�ã����<0�ʹ����ƶ�
	bool inSelfChessBoard;	// �Ƿ����������̵�����
	StepMove()
	{
		destIndex = NO_MOVE;	// ����
		uniqueId = 0;
		inSelfChessBoard = true;
	}
};

struct StepChangeRound
{
	unsigned int round;	// �غ���
};

struct StepUseBlood
{
	unsigned int uniqueId;	// ˭ʹ�ã�cdʱ���ǹ̶��ģ��ڳ�ʼ����Ϣ���档
	int bloodChangeForUser;	// Ѫ���仯
};

struct StepFight
{
	// �����ߺͱ������ߵ�Id
	unsigned int fightorUniqueId;
	FightPattern fightPattern;
	
	vector<unsigned int> targetUniqueVec;	// �������Ķ���
	vector<bool> canMissVec;				// �ܷ���
	vector<int> targetBloodChange;			// Ŀ���Ѫ���仯
};

struct StepSkill
{
	unsigned int fightorUniqueId;
	SkillPattern skillPatten;
	SkillType skillType;					// ��������

	vector<ClientGameBuff> firedBuff;		// �����Buff,���涨�嵽���ĸ�buff��ӵ��ĸ�������
	vector<unsigned int> targetUniqueVec;	// �������Ķ���
	vector<bool> canMissVec;				// �ܷ���
	vector<int> targetBloodChange;			// Ŀ���Ѫ���仯
};

// ���̳�ʼ����pve�õ�
struct InitChessBoard
{
	// �ҷ���ս������,[0]������index0��λ�õĽ�ɫ����
	HeroBase myTeam[FormationHeroAcountMax];
	// ȫս��һ��
	unsigned int myTeamUniqueId[FormationHeroAcountMax];
	int myCurBloodNum;		// �ҷ���ǰ����Ѫƿ���� -----> class ChessBoard -->currentBloodNum
	int myDefaultBloodNum;	// �ҷ���ǰȫ��Ѫƿ����

	// �Է��Ĳ���
	HeroBase oppositeTeam[FormationHeroAcountMax];
	unsigned int oppositeUniqueId[FormationHeroAcountMax];
	int oppositeCurBloodNum;		// �Է���ǰ����Ѫƿ����
	int oppositeDefaultBloodNum;	// �Է���ǰȫ��Ѫƿ����

	InitChessBoard()
	{
		myCurBloodNum = 0;
		myDefaultBloodNum = 0;
		oppositeCurBloodNum = 0;
		oppositeDefaultBloodNum = 0;
	}

};

// ս������
struct FightEnd
{
	// �ҷ���ս�����4B
	FightResult myResult;	
	// �õ��ľ���
	unsigned int gettedExp;
	// �õ���Item��Ƭ
	vector<ItemSuipianInfo> gettedItemSuiPian;
	// �õ��������Ƭ
	vector<ShenbingSuipianInfo> gettedShenBingSuiPian;
	// �õ���װ��
	vector<EquipData> gettedZhuangBei;
	// ��õ���Ʒ 4->4+nB
	vector<ItemInfo> gettedItem;
	// �õ���������
	vector<PowerPoint> gettedPowerPoints;
};
// ĳһ��Item��cdʱ�������
struct CdDiscr
{
	short cdTime;
	unsigned int uniqueId;
	CdDiscr()
	{
		cdTime = 0;
		uniqueId = 0;
	}
};


struct Buff_Doing
{
	unsigned int target;	// ˭�����buff��Ѫ�Ķ���
	int hp_add;	// Ѫ�������ӣ��������Ǽ���
	short relativedBuffStateId;	// ��������buff

	bool isFantan;				// ����Ƿ�������������ݲ�����
	unsigned int fightor_u_id;	// ������Ŀ�꣬˭�������Ķ���
	

	Buff_Doing()
	{
		target = 0;
		hp_add = 0;
		relativedBuffStateId = BuffState_WhatEver;

		isFantan = false;
		fightor_u_id = 0;
	}
};

// ֻ���������̻��͵�buff
// ��ΪҲֻ�й̻��͵�buff����Ҫ�����ָ���ĸ�ʱ������(��Ѫ����Ѫ)
// ����״̬�͵�buff������ȥ�����ˣ��ͻ���Ҳ����Ҫ��⣬��Ϊ��ֵϵͳ�Ѿ�ֱ����õġ�
// ����ֻ��Ҫ���ƹ̻��͵ľͿ����ˡ�
//
// ֱ�ӵ����ֻҪ��ֱ���޸Ľ�ɫ��ֵ��buff�����ǹ̻��͵ģ������Ѫ���������ֱ�ӼӼ���ɫ��Ѫ��
// �ٱ���ӷ����������ʵ����״̬�͵ģ���Ϊbuff�Ƴ��������û���ˣ�û�й̻�����ɫ��
struct BuffInStep
{
	//Buff_Merge merge;
	vector<int> remove_buff_array;	// �Ƴ���buff������
	vector<Buff_Doing> effects; // ��������ж��target����ͬһ���ˣ��ͻ�����Ҫ����ĳ��target��buffdoingȫ���ŵ��Ǹ�������
};

// ����
struct OneStep
{
	// ������ʽ���ƶ���������ֱ�ӹ���(������Ժ����ƶ�);
	// ������ʽ�Ĳ�ͬ����ȡ��ͬ������
	StepType stepType;	
	BuffInStep buff;	// ��һ����buff����
	vector<CdDiscr> cdTime_arr;		// ����ÿһ���˵ĵ�ǰcdʱ�䣬0����û�С�

	StepMove moveData;
	StepFight fightData;
	StepSkill skillData;
	StepUseBlood useBlood;
	StepChangeRound changeRound;


	// 1,2,3
	string getBuffString();
	//
	string getCurrentCdTimeString();
};

// ��ĳһ���ͻ��˵ľ籾
struct FightScript
{
	// ��ʼ�����ݣ��ͻ��˲�һ��
	InitChessBoard init;

	// ��������,�ͻ�����һ����
	vector<OneStep> stepVec;

	// ������ݣ��ͻ��˲�һ��
	FightEnd endResult;
	// 
	bool isEqual(const FightScript& test);
};

// �μ�SPCmd_BattleRsp
struct OneBattle
{
	SimpleProtocolErrorCode err_code;
	vector<FightScript> scriptVec;
};

