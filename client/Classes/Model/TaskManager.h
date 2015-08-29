#pragma once

#include "protocol.h"
#include <vector>
#include "flat.h"

using namespace std;

struct Flat_SubTaskInfor;

// 每日任务完成的奖励
struct SubTaskRewardItem
{
	unsigned int task_id;
	unsigned int reward_item_id;
	unsigned int reward_item_count;
};

class TaskManager
{
private:
	vector<main_task_reward> mainTaskRewardVec;
	vector<Flat_SubTaskInfor> subTaskes;

	// 每日任务中完成之后，得到了奖励，还没显示的就放在这里
	vector<SubTaskRewardItem> subTaskNotShowedRewardVec;

	// 最近的激活的任务
	TaskId maintask_last_actived;
	// 最近激活的任务的状态
	TaskStatus maintask_status;

	// 地狱副本的记录
	unsigned int hell_battle_time_remainder;	//本日还剩几次
	TaskId last_active_hell_task_id;			//当前进度到那个副本了

	static TaskManager* manager;

	TaskManager(void);

	//记录当前正在打第几关，第几个格子
	int mCurFightTaskId; //初始化-1
	int mCurFightTaskGridIdx;

public:
	static TaskManager* getManager();
	// 初始化
	void initMainTask(TaskId maintask_last_actived,TaskStatus maintask_status);
	void initSubTask(vector<Flat_SubTaskInfor> taskes);
	void initMainTaskReward(vector<main_task_reward> rewards);

	void setLastActivedMainTaskId(TaskId taskId);
	void setLastActivedMainTaskStatus(TaskStatus taskStatus);

	// 4章
	unsigned int getSeasonNum();

	TaskId getLastActivedMainTaskId();
	TaskStatus getLastActivedMainTaskStatus();
	//主线战斗关卡记录
	int getCurrentFightTaskId(){
		return mCurFightTaskId;
	}
	int getCurrentFightZhangJie()
	{
		int iZhangjie = 0;
		if (mCurFightTaskId <= 10)
		{
			iZhangjie = 1;
		}
		else if (mCurFightTaskId > 10 && mCurFightTaskId <= 20)
		{
			iZhangjie = 2;
		}
		else if (mCurFightTaskId > 20 && mCurFightTaskId <= 30)
		{
			iZhangjie = 3;
		}
		else if (mCurFightTaskId > 30 && mCurFightTaskId <= 40)
		{
			iZhangjie = 4;
		}

		return iZhangjie;
	}
	int getCurrentFightGridIdx(){
		return mCurFightTaskGridIdx;
	}
	void setCurrentFightTaskId(int id){
		mCurFightTaskId = id;
	}
	void setCurrentFightGridIdx(int idx){
		mCurFightTaskGridIdx = idx;
	}

	// 获取没完成的任务
	vector<Flat_SubTaskInfor> getNotFinishedSubTaskList(){return subTaskes;};
	// 查询主线任务的数据
	bool getMainTaskReward(int taskId,main_task_reward& out_data);
	vector<main_task_reward> getMainTaskRewardVec(){return mainTaskRewardVec;};

	// 设置/获取 地狱副本的信息
	void setHellBattelTimeRemainder(unsigned int remainder){this->hell_battle_time_remainder = remainder;};
	void setLastActiveHellTaskId(TaskId _id){this->last_active_hell_task_id = _id;};
	TaskId getLastActiveHellTaskId(){return last_active_hell_task_id;};
	unsigned int getHellBattelTimeRemainder(){return hell_battle_time_remainder;};
	void subHellBattelTimeRemainder();


	void removeSubTask(int taskId);

	// 存放一个每日任务的奖励
	// 消息MSG_subTaskSubmitedRsp
	void putSubTaskRewardItem(SubTaskRewardItem reward);
	void clearAllSubTaskRewardItems();
	void removeSubTaskRewardItem(int subTaskId);
	// 获取奖励，一般里面应该只有一个
	// 获取了之后，需要调用clear来清空
	vector<SubTaskRewardItem> getAllSubTaskRewards();
	bool haveSubTaskReward();

	//新加boss name
	string getBossName(Profession profId);

	~TaskManager(void);
};

