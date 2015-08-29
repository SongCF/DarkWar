#include "JumpHolder.h"
#include "Debug.h"
//#include "GlobalTickTrigle.h"
#include "ChessBoardStateManager.h"
#include "TaskManager.h"

#define MSG_TRIGLE_TICK_JUMP  "___MSG_TRIGLE_TICK_JUMP___"

JumpHolder* JumpHolder::jHolder = NULL;

JumpHolder::JumpHolder(void)
{
//	trigleId = 0;
	minTimeNeedWaitBeforeJump = 0;
//	CCNotificationCenter::sharedNotificationCenter()->addObserver(this,SEL_CallFuncO(&JumpHolder::tick),MSG_TRIGLE_TICK_JUMP,NULL);
}

JumpHolder* JumpHolder::getHolder()
{
	if (jHolder == NULL)
	{
		jHolder = new JumpHolder();
	}
	return jHolder;
}
bool JumpHolder::canJump(bool bIgnoreCDTime)
{
#if JUMP_ENY_TIME == 1
	return true;
#endif

	if (mContext.battleType == BattleType2_TestMode)
	{
		return true;
	}

	if (!bIgnoreCDTime && minTimeNeedWaitBeforeJump > 0)
	{
		return false;
	}

	// 引导不能跳过
	if (mContext.battleType == BattleType2_InitGuide)
	{
		return false;
	}

	// 世界boss绝对不能跳//20140617现在要求5s后跳过！
	if (mContext.battleType == BattleType2_WorldBoss)
	{
		return true;
	}


	// 主线,并且打过了，并且不是boss关，那么就可以直接等时间到了就可以跳
	if (mContext.battleType == BattleType2_MainTask && mContext.taskId < HellTaskId_Min)
	{
		TaskChessBoard board;
		bool mustFund = ChessBoardStateManager::getManager()->getChessBoard(mContext.taskId,board);
		if(!mustFund)
		{
			//CCAssert(false,"");
			CCLog("Error : %s ---> BattleType2_MainTask ,but not found in ChessBoardStateManager",__FUNCTION__);
			return false;
		}
		// 如果正打到这一关，并且格子已经是探过的，并且不是boss，就可以跳过

//scf : 由于更改TaskGridInfo后 暂时注释
/*		if (TaskManager::getManager()->getLastActivedMainTaskId() == mContext.taskId && 
			board.gridstate[mContext.gridId] == true && 
			!ChessBoardStateManager::getManager()->isBossSection(mContext.taskId,mContext.gridId))
		{
			return true;
		}*/

		// 如果打的以前通关过的关卡，那么只要是非boss，都可以跳过
		if (TaskManager::getManager()->getLastActivedMainTaskId() > mContext.taskId && 
			!ChessBoardStateManager::getManager()->isBossSection(mContext.taskId,mContext.gridId))
		{
			return true;
		}

		return false;
	}

	// 地狱副本
	if (mContext.battleType == BattleType2_MainTask && mContext.taskId >= HellTaskId_Min)
	{
		return false;
	}

	if (mContext.battleType == BattleType2_TopHeroChallenge)
	{
		return true;
	}

	if (mContext.battleType == BattleType2_TopArenaPlayBack)
	{
		return true;
	}

	if (mContext.battleType == BattleType2_Arena)
	{
		return true;
	}

	if (mContext.battleType == BattleType2_ChuanQiLu)
	{
		return false;
	}

	if (mContext.battleType == BattleType2_XueSeJiaoTang)
	{
		return true;
	}

	return false;
}

void JumpHolder::setFightContext(FightContext contex)
{
	mContext = contex;
	resetNeedWaitTime(mContext.waitTime);
}

void JumpHolder::resetNeedWaitTime(float time)
{
	minTimeNeedWaitBeforeJump = time;
/*	GlobalTickTrigle::getTrigle()->removeTrigle(trigleId);

	OneTrigle trigle = OneTrigle::makeTrigleDelay(MSG_TRIGLE_TICK_JUMP,0,true);
	//trigle.loop = true;
	//trigle.type = Trigle_Delay;
	//trigle.time = 0;
	//trigle.notifyMsg = MSG_TRIGLE_TICK_JUMP;
	trigleId = GlobalTickTrigle::getTrigle()->addOneTrigle(trigle);*/
	CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(JumpHolder::postCanJumpNow), this);
	// delay + 2  战斗开始的时候有2秒延迟
	CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(JumpHolder::postCanJumpNow), this,1,minTimeNeedWaitBeforeJump+1,1+2,false);
}
void JumpHolder::postCanJumpNow(float dt)
{
	--minTimeNeedWaitBeforeJump;
	if (canJump())
	{
		// 通知可以jump了
		CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(JumpHolder::postCanJumpNow), this);
		CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_JUMP_CAN_JUMP,NULL);
	}
}
void JumpHolder::tick(CCObject* tickObjectData)
{
	//不再使用以前的通知方式
// 	TickObjectData* tickData = (TickObjectData*)tickObjectData;
// 
// 	minTimeNeedWaitBeforeJump -= tickData->timePastFromLast;
// 	if (minTimeNeedWaitBeforeJump <= 0)
// 	{
// 		GlobalTickTrigle::getTrigle()->removeTrigle(trigleId);
// 		if (canJump())
// 		{
// 			// 通知可以jump了
// 			CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_JUMP_CAN_JUMP,NULL);
// 		}
// 	}
}

JumpHolder::~JumpHolder(void)
{
}
