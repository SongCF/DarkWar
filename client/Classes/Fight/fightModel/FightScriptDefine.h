#pragma once

#include "protocol.h"
#include <vector>
#include "ScriptDataProvider.h"

using namespace std;

#define		NO_MOVE				-1
#define		PVE_MAX_STEP_NUM	100		// pve的时候每一场最多多少步

//// 特殊控制器
//enum SpecialControl
//{
//	SC_NONE = 0,		// 没有特殊控制，就是一般的战斗
//	SC_BOSS_FREEZE,	// boss不动
//};


enum StepType
{
	StepType_Fight=1,
	StepType_Move,
	StepType_Skill,
	StepType_FailByTakeTooMuchTime,	// 花时间太久，直接判输
	StepType_Wait,					// 等待，什么也不做，不占步数
	StepType_UseBlood,				// 使用血瓶，不占步数
	StepType_ChangeRound,			// 新一大轮了

	StepType_ReLoadBlood,			// 团队打boss，复活,用StepUseBlood的数据结构
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
	Result_Win = 0,		// 成功
	Result_Lose,		// 失败
};

struct StepMove
{
	unsigned int uniqueId;	// 谁移动
	int destIndex;			// 目的 的索引位置，如果<0就代表不移动
	bool inSelfChessBoard;	// 是否是自身棋盘的坐标
	StepMove()
	{
		destIndex = NO_MOVE;	// 不动
		uniqueId = 0;
		inSelfChessBoard = true;
	}
};

struct StepChangeRound
{
	unsigned int round;	// 回合数
};

struct StepUseBlood
{
	unsigned int uniqueId;	// 谁使用，cd时间是固定的，在初始化信息里面。
	int bloodChangeForUser;	// 血量变化
};

struct StepFight
{
	// 攻击者和被攻击者的Id
	unsigned int fightorUniqueId;
	FightPattern fightPattern;
	
	vector<unsigned int> targetUniqueVec;	// 被攻击的对象
	vector<bool> canMissVec;				// 能否躲避
	vector<int> targetBloodChange;			// 目标的血量变化
};

struct StepSkill
{
	unsigned int fightorUniqueId;
	SkillPattern skillPatten;
	SkillType skillType;					// 技能类型

	vector<ClientGameBuff> firedBuff;		// 打出的Buff,里面定义到有哪个buff添加到哪个人身上
	vector<unsigned int> targetUniqueVec;	// 被攻击的对象
	vector<bool> canMissVec;				// 能否躲避
	vector<int> targetBloodChange;			// 目标的血量变化
};

// 棋盘初始化，pve用的
struct InitChessBoard
{
	// 我方的战斗布局,[0]就是在index0号位置的角色数据
	HeroBase myTeam[FormationHeroAcountMax];
	// 全战斗一的
	unsigned int myTeamUniqueId[FormationHeroAcountMax];
	int myCurBloodNum;		// 我方当前可用血瓶数量 -----> class ChessBoard -->currentBloodNum
	int myDefaultBloodNum;	// 我方当前全部血瓶数量

	// 对方的布局
	HeroBase oppositeTeam[FormationHeroAcountMax];
	unsigned int oppositeUniqueId[FormationHeroAcountMax];
	int oppositeCurBloodNum;		// 对方当前可用血瓶数量
	int oppositeDefaultBloodNum;	// 对方当前全部血瓶数量
    
    // by scf
    const InitChessBoard& operator=(const InitChessBoard& ini)
    {
        for(int i=0; i<FormationHeroAcountMax; ++i)
        {
            myTeam[i] = ini.myTeam[i];
            oppositeTeam[i] = ini.oppositeTeam[i];
        }
        memcpy(myTeamUniqueId, ini.myTeamUniqueId, FormationHeroAcountMax*sizeof(unsigned int));
        myCurBloodNum = ini.myCurBloodNum;
        myDefaultBloodNum = ini.myDefaultBloodNum;
        memcpy(oppositeUniqueId , ini.oppositeUniqueId, FormationHeroAcountMax*sizeof(unsigned int));
        oppositeCurBloodNum = ini.oppositeCurBloodNum;
        oppositeDefaultBloodNum = ini.oppositeDefaultBloodNum;
        
        return *this;
    }
    
	//by scf
    InitChessBoard(const InitChessBoard& ini)
    {
        for(int i=0; i<FormationHeroAcountMax; ++i)
        {
            myTeam[i] = ini.myTeam[i];
            oppositeTeam[i] = ini.oppositeTeam[i];
        }
        memcpy(myTeamUniqueId, ini.myTeamUniqueId, FormationHeroAcountMax*sizeof(unsigned int));
        myCurBloodNum = ini.myCurBloodNum;
        myDefaultBloodNum = ini.myDefaultBloodNum;
        memcpy(oppositeUniqueId , ini.oppositeUniqueId, FormationHeroAcountMax*sizeof(unsigned int));
        oppositeCurBloodNum = ini.oppositeCurBloodNum;
        oppositeDefaultBloodNum = ini.oppositeDefaultBloodNum;
    }
    

	InitChessBoard()
	{
		myCurBloodNum = 0;
		myDefaultBloodNum = 0;
		oppositeCurBloodNum = 0;
		oppositeDefaultBloodNum = 0;
	}

};

// 战斗结束
struct FightEnd
{
	// 我方的战斗结果4B
	FightResult myResult;	
	// 得到的经验
	unsigned int gettedExp;
	// 得到的Item碎片
	vector<ItemSuipianInfo> gettedItemSuiPian;
	// 得到的神兵碎片
	vector<ShenbingSuipianInfo> gettedShenBingSuiPian;
	// 得到的装备
	vector<EquipData> gettedZhuangBei;
	// 获得的物品 4->4+nB
	vector<ItemInfo> gettedItem;
	// 得到的能量点
	vector<PowerPoint> gettedPowerPoints;
    
	// by scf
    FightEnd(const FightEnd& f)
    {
        myResult = f.myResult;
        gettedExp = f.gettedExp;
        gettedItemSuiPian = f.gettedItemSuiPian;
        gettedShenBingSuiPian = f.gettedShenBingSuiPian;
        gettedZhuangBei = f.gettedZhuangBei;
        gettedItem = f.gettedItem;
        gettedPowerPoints = f.gettedPowerPoints;
    }
	//by scf
    const FightEnd& operator=(const FightEnd& f)
    {
        myResult = f.myResult;
        gettedExp = f.gettedExp;
        gettedItemSuiPian = f.gettedItemSuiPian;
        gettedShenBingSuiPian = f.gettedShenBingSuiPian;
        gettedZhuangBei = f.gettedZhuangBei;
        gettedItem = f.gettedItem;
        gettedPowerPoints = f.gettedPowerPoints;
        
        return *this;
    }
	//by scf
    FightEnd()
    {
        
    }
};
// 某一个Item的cd时间的描素
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
	unsigned int target;	// 谁做这个buff减血的动作
	int hp_add;	// 血量的增加，负数就是减少
	short relativedBuffStateId;	// 关联到的buff

	bool isFantan;				// 如果是反弹，下面的数据才有用
	unsigned int fightor_u_id;	// 反弹的目标，谁做反击的动画
	

	Buff_Doing()
	{
		target = 0;
		hp_add = 0;
		relativedBuffStateId = BuffState_WhatEver;

		isFantan = false;
		fightor_u_id = 0;
	}
};

// 只用于描述固化型的buff
// 因为也只有固化型的buff才需要提出来指定哪个时候作用(加血，减血)
// 其他状态型的buff，加上去就是了，客户端也不需要理解，因为数值系统已经直接算好的。
// 所有只需要控制固化型的就可以了。
//
// 直接点就是只要有直接修改角色数值的buff，都是固化型的，比如加血，这个就是直接加减角色的血量
// 再比如加防御，这个其实就是状态型的，因为buff移除，这个就没有了，没有固化给角色。
struct BuffInStep
{
	//Buff_Merge merge;
	vector<int> remove_buff_array;	// 移除的buff的数组
	vector<Buff_Doing> effects; // 里面可以有多个target都是同一个人，客户端需要把是某个target的buffdoing全部放到那个人身上
};

// 步伐
struct OneStep
{
	// 步调形式：移动不攻击，直接攻击(里面可以涵盖移动);
	// 根据形式的不同，就取不同的数据
	StepType stepType;	
	BuffInStep buff;	// 这一步的buff数据
	vector<CdDiscr> cdTime_arr;		// 描素每一个人的当前cd时间，0就是没有。

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

// 给某一个客户端的剧本
struct FightScript
{
	// 初始化数据，客户端不一样
	InitChessBoard init;

	// 过程数据,客户端是一样的
	vector<OneStep> stepVec;

	// 结果数据，客户端不一样
	FightEnd endResult;
	// 
	bool isEqual(const FightScript& test);
};

// 参见SPCmd_BattleRsp
struct OneBattle
{
	SimpleProtocolErrorCode err_code;
	vector<FightScript> scriptVec;
};

