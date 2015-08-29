#include "ChessBoardStateManager.h"

ChessBoardStateManager* ChessBoardStateManager::manager = new ChessBoardStateManager();

ChessBoardStateManager::ChessBoardStateManager(void)
{
	isTaskRateOk = false;
	isKillMonstorBonusOk = false;
	killedPuTongMonsterNum = 0;
	killedJingYinMonsterNum = 0;
}
ChessBoardStateManager::~ChessBoardStateManager(void)
{
}
ChessBoardStateManager* ChessBoardStateManager::getManager()
{
	return manager;
}

void ChessBoardStateManager::initOneChessboard(TaskChessBoard board)
{
	int num = taskChessboards.size();
	for (int i=0;i<num;i++)
	{
		if (taskChessboards[i].taskId == board.taskId)
		{
			taskChessboards[i] = board;
			return;
		}
	}

	taskChessboards.push_back(board);
}

bool ChessBoardStateManager::getChessBoard(int taskId,TaskChessBoard& out_data)
{
	int taskNum = taskChessboards.size();
	for (int i=0;i<taskNum;i++)
	{
		if (taskChessboards[i].taskId == taskId)
		{
			out_data = taskChessboards[i];
			return true;
		}
	}
	return false;
}

bool ChessBoardStateManager::isBossSection(int taskId,int gridId)
{
	return gridId == 0;
// 	int taskNum = taskChessboards.size();
// 	for (int i=0;i<taskNum;i++)
// 	{
// 		if (taskChessboards[i].taskId == taskId)
// 		{
// 			// 最后一关就是boss
// 			//return taskChessboards[i].gridstate.size() == (gridId+1);
// 			return gridId == 0;
// 		}
// 	}
// 	return false;
}

int ChessBoardStateManager::getSectionTotalFightNum(int taskId,int gridId)
{
	
	if (taskId == 15)
	{//第二章 5守护遗迹 
		return 6; // 暂定6场
	}
	else if(taskId == 25)
	{//第三章 5再现西蒙斯
		return 6; // 暂定6场
	}
	else if(taskId == 29)
	{//第三章 9寒冰陷阱 
		return 6; // 暂定6场
	}else
	{
		// 第三章 7三重门 
		// 第四章 9摧毁熔炉之心 
		// 以及普通关
		return 1;	// 其他的默认都是1场
	}
}


void ChessBoardStateManager::setCurrentTaskRate(Flat_QueryRateTaskRsp rate)
{
	isTaskRateOk = true;
	taskRate = rate;
}

void ChessBoardStateManager::setKilledMonsterBonus(Flat_QueryKilledMonsterBonusRsp bonus)
{
	isKillMonstorBonusOk = true;
	killMonstorBonus = bonus;
}

void ChessBoardStateManager::setKilledMonsterNum(unsigned int putongNum,unsigned int jingyinNum)
{
	killedPuTongMonsterNum = putongNum;
	killedJingYinMonsterNum = jingyinNum;
}

void ChessBoardStateManager::subKilledPutongMonster(unsigned int toSub)
{
	if (toSub > killedPuTongMonsterNum)
	{
		killedPuTongMonsterNum = 0;
		CCAssert(false,"");
	}
	
	killedPuTongMonsterNum -= toSub;
}
void ChessBoardStateManager::subKilledJingyinMonster(unsigned int toSub)
{
	if (toSub > killedJingYinMonsterNum)
	{
		killedJingYinMonsterNum = 0;
		CCAssert(false,"");
	}

	killedJingYinMonsterNum -= toSub;
}

void ChessBoardStateManager::addKilledPutongMonster(unsigned int toAdd)
{
	killedPuTongMonsterNum += toAdd;
}
void ChessBoardStateManager::addKilledJingyinMonster(unsigned int toAdd)
{
	killedJingYinMonsterNum += toAdd;
}

void ChessBoardStateManager::setTaskRateBonusList(unsigned int seasonOrSectionId,Flat_QueryRateTaskBonusRsp data)
{
	map<unsigned int,Flat_QueryRateTaskBonusRsp>::iterator it;
	if (data.type == RateStarType_Chapter)
	{
		it = seasonRate.find(seasonOrSectionId);
		if (it == seasonRate.end())
		{
			seasonRate.insert(pair<unsigned int,Flat_QueryRateTaskBonusRsp>(seasonOrSectionId,data));
		}else
		{
			it->second = data;
		}
	}
	else  if(data.type == RateStarType_Task)
	{
		it = sectionRate.find(seasonOrSectionId);
		if (it == sectionRate.end())
		{
			sectionRate.insert(pair<unsigned int,Flat_QueryRateTaskBonusRsp>(seasonOrSectionId,data));
		}else
		{
			it->second = data;
		}
	}else
	{
		CCAssert(false,"不支持");
	}
}
Flat_QueryRateTaskBonusRsp* ChessBoardStateManager::getSeasonRateBonus(int seasonId)
{
	map<unsigned int,Flat_QueryRateTaskBonusRsp>::iterator it = seasonRate.find(seasonId);
	if (it != seasonRate.end())
	{
		return &it->second;
	}
	return NULL;
}
Flat_QueryRateTaskBonusRsp* ChessBoardStateManager::getTaskRateBonus(int taskId)
{
	map<unsigned int,Flat_QueryRateTaskBonusRsp>::iterator it = sectionRate.find(taskId);
	if (it != sectionRate.end())
	{
		return &it->second;
	}
	return NULL;
}

// 获取当前的星级
Flat_QueryRateTaskRsp* ChessBoardStateManager::getCurrentRate()
{
	return isTaskRateOk ? (&taskRate) : NULL;
}

Flat_QueryKilledMonsterBonusRsp ChessBoardStateManager::getKilledMonsterBonusData()
{
	CCAssert(isKillMonstorBonusOk,"");
	return killMonstorBonus;
}

unsigned int ChessBoardStateManager::getKilledPutongMonsterNum()
{
	return killedPuTongMonsterNum;
}
unsigned int ChessBoardStateManager::getKilledJingYinMonsterNum()
{
	return killedJingYinMonsterNum;
}