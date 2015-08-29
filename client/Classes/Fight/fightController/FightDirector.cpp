#include "FightDirector.h"


FightDirector::FightDirector(void)
{
	inited = false;
}
void FightDirector::initScript(FightScript script)
{
	mScript = script;
	//mScript.stepVec.clear();
	//mScript.init = script.init;
	//mScript.endResult = script.endResult;
	//// 修改脚本，把preMove 提出来编程move
	//int stepNum = script.stepVec.size();
	//for (int i = 0; i < stepNum; i++)
	//{
	//	// fight有premove
	//	if (script.stepVec[i].stepType == StepType_Fight && script.stepVec[i].fightData.preMove.destIndex != NO_MOVE)
	//	{
	//		//CCAssert(false,"不需要了");
	//		OneStep step;
	//		step.stepType = StepType_Move;
	//		step.moveData = script.stepVec[i].fightData.preMove;
	//		mScript.stepVec.push_back(step);

	//		script.stepVec[i].fightData.preMove.destIndex = NO_MOVE;	// 取消移动
	//	}
	//	// skill有premove
	//	if (script.stepVec[i].stepType == StepType_Skill && script.stepVec[i].skillData.preMove.destIndex != NO_MOVE)
	//	{
	//		//CCAssert(false,"不需要了");
	//		OneStep step;
	//		step.stepType = StepType_Move;
	//		step.moveData = script.stepVec[i].skillData.preMove;
	//		mScript.stepVec.push_back(step);

	//		script.stepVec[i].skillData.preMove.destIndex = NO_MOVE;	// 取消移动
	//	}

	//	mScript.stepVec.push_back(script.stepVec[i]);
	//	//
	//}
	inited = true;
}

bool FightDirector::getStep(int stepIndex,OneStep& out_step)
{
	if (stepIndex < 0 || stepIndex+1 > mScript.stepVec.size())
	{
		return false;
	}
	out_step = mScript.stepVec[stepIndex];

	return true;
}


FightDirector::~FightDirector(void)
{
}
