#include "TaskManager.h"

TaskManager* TaskManager::manager = new TaskManager();

TaskManager::TaskManager(void)
{
	this->maintask_last_actived = 0;

	mCurFightTaskId = -1;//默认-1
	mCurFightTaskGridIdx = -1;
}
TaskManager* TaskManager::getManager()
{
	return manager;
}

//void TaskManager::init(TaskId maintask_last_actived,TaskStatus maintask_status,vector<SubTaskItem> subStaskVec)
//{
//	this->maintask_last_actived = maintask_last_actived;
//	this->maintask_status = maintask_status;
//	this->subStaskVec = subStaskVec;
//
//	{
//		CCLog("TaskManager::init----> maintask_last_actived = %d,maintask_status = %d",maintask_last_actived,maintask_status);
//		int num = subStaskVec.size();
//		for (int i=0;i<num;i++)
//		{
//			CCLog("TaskManager::init---->subStaskVec [%d] <--------",i);
//			subStaskVec[i].log();
//			CCLog("\n");
//		}
//		CCLog("TaskManager::init----> over");
//	}
//}

void TaskManager::initMainTask(TaskId maintask_last_actived,TaskStatus maintask_status)
{
	this->maintask_last_actived = maintask_last_actived;
	this->maintask_status = maintask_status;
}
void TaskManager::initSubTask(vector<Flat_SubTaskInfor> taskes)
{
	this->subTaskes = taskes;
}
void TaskManager::initMainTaskReward(vector<main_task_reward> rewards)
{
	this->mainTaskRewardVec = rewards;
}

void TaskManager::subHellBattelTimeRemainder()
{
	if (this->hell_battle_time_remainder == 0)
	{
		CCAssert(false,"");
		return;
	}
	this->hell_battle_time_remainder --;
}

void TaskManager::removeSubTask(int taskId)
{
	int taskNum = subTaskes.size();
	for (vector<Flat_SubTaskInfor>::iterator it = subTaskes.begin();it != subTaskes.end();it++)
	{
		if (it->task_id == taskId)
		{
			subTaskes.erase(it);
			break;
		}
	}
}

void TaskManager::putSubTaskRewardItem(SubTaskRewardItem reward)
{
	for (vector<SubTaskRewardItem>::iterator it = subTaskNotShowedRewardVec.begin();it != subTaskNotShowedRewardVec.end();it++)
	{
		if (it->task_id == reward.task_id)
		{
			CCAssert(false,"已经完成了，怎么还有奖励返回？？bug？？");
			return;
		}
	}

	subTaskNotShowedRewardVec.push_back(reward);
}
void TaskManager::clearAllSubTaskRewardItems()
{
	subTaskNotShowedRewardVec.clear();
}
void TaskManager::removeSubTaskRewardItem(int subTaskId)
{
	for (vector<SubTaskRewardItem>::iterator it = subTaskNotShowedRewardVec.begin();it != subTaskNotShowedRewardVec.end();it++)
	{
		if (it->task_id == subTaskId)
		{
			subTaskNotShowedRewardVec.erase(it);
			break;
		}
	}
}

bool TaskManager::haveSubTaskReward()
{
	return !subTaskNotShowedRewardVec.empty();
}

vector<SubTaskRewardItem> TaskManager::getAllSubTaskRewards()
{
	return subTaskNotShowedRewardVec;
}

unsigned int TaskManager::getSeasonNum()
{
	return 4;
}

void TaskManager::setLastActivedMainTaskId(TaskId taskId)
{
	maintask_last_actived = taskId;
}
void TaskManager::setLastActivedMainTaskStatus(TaskStatus taskStatus)
{
	maintask_status = taskStatus;
}

TaskId TaskManager::getLastActivedMainTaskId()
{
	return maintask_last_actived;
}
TaskStatus TaskManager::getLastActivedMainTaskStatus()
{
	return maintask_status;
}

bool TaskManager::getMainTaskReward(int taskId,main_task_reward& out_data)
{
	int num = mainTaskRewardVec.size();
	for (int i=0;i<num;i++)
	{
		if (mainTaskRewardVec[i].task_id == taskId)
		{
			out_data = mainTaskRewardVec[i];
			return true;
		}
	}

	//CCAssert(false,"没有找到，是不是没有调用网络接口查询过....");
	return false;
}

//vector<SubTaskItem> TaskManager::getAllSubTasks()
//{
//	return subStaskVec;
//}


string TaskManager::getBossName(Profession profId)
{
	switch (profId)
	{
	case 64:
		return LFStrings::getValue("BossName64");
	case 88:
		return LFStrings::getValue("BossName88");
	case 89:
		return LFStrings::getValue("BossName89");
	case 90:
		return LFStrings::getValue("BossName90");
	case 91:
		return LFStrings::getValue("BossName91");
	case 92:
		return LFStrings::getValue("BossName92");
	case 93:
		return LFStrings::getValue("BossName93");
	case 94:
		return LFStrings::getValue("BossName94");
	case 95:
		return LFStrings::getValue("BossName95");
	case 112:
		return LFStrings::getValue("BossName112");
	case 113:
		return LFStrings::getValue("BossName113");
	default:
		CCAssert(false, "没说这是boss");
		break;
	}
	return"";
}


TaskManager::~TaskManager(void)
{
}
