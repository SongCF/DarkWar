#pragma once

#include "cocos2d.h"
using namespace cocos2d;

// 是否一律可以跳过
#define JUMP_ENY_TIME	0


#if JUMP_ENY_TIME == 1
#define MIN_WAIT_BEFOER_JUMP	1.0f
#else
// 跳之前必须等10秒
#define MIN_WAIT_BEFOER_JUMP	10.0f
#endif




enum BattleType_2
{
	//pve
	BattleType2_InitGuide = 100,		// 初始引导的战斗
	BattleType2_MainTask,				// 主线任务
	BattleType2_WorldBoss,				// 世界boss  极限守城
	BattleType2_TestMode,				// 
	BattleType2_ChuanQiLu,				// 
	BattleType2_XueSeJiaoTang,			// 

	BattleType2_SHZL,			// pvp ^ pve
	//pvp
	BattleType2_TopHeroChallenge,		// 英雄榜
	BattleType2_TopArenaPlayBack,		// 巅峰对决回放
	BattleType2_Arena,					// 竞技场，自由竞技+随机模式


	//BattleType2_Arena,
	//BattleType2_MainTask,
	//BattleType2_CityGuard,
	//BattleType2_TopHeroChallenge,
	//BattleType2_TopArena,
};

// 战斗的context
struct FightContext
{
	BattleType_2 battleType; //BattleType_2
	int taskId;		//非主线，这个字段就没用
	int gridId;		//非主线，这个字段就没用

	float waitTime; // 秒

	FightContext()
	{
	}
	FightContext(BattleType_2 _battleType,float _waitTime)
	{
		battleType = _battleType;
		waitTime = _waitTime;
	}
};

#define MSG_JUMP_CAN_JUMP	"MSG_JUMP_CAN_JUMP"

class JumpHolder : public CCObject
{
private:
	static JumpHolder* jHolder;

//  不再使用以前的方式。
//	unsigned int trigleId;

	FightContext mContext;
	// 至少要等多少秒才可以使用跳过
	float minTimeNeedWaitBeforeJump;
	JumpHolder(void);
	void tick(CCObject* tickObjectData);//不再使用
	void postCanJumpNow(float dt);
public:
	static JumpHolder* getHolder();
	// 设置跳过之前需要等待的时间（秒数）
	void resetNeedWaitTime(float time);
	// 设置当前的战斗信息,并且开始倒计时
	void setFightContext(FightContext contex);
	bool canJump(bool bIgnoreCDTime=false);
	~JumpHolder(void);
};

