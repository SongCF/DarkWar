#pragma once
#include <stdlib.h>
#include <vector>
#include "protocol.h"

using namespace std;

class GiftActivtyManager
{
	friend class CmdHelper;
private:
	static GiftActivtyManager* mInstance;
	vector<ActivityInfo> activties;

	GiftActivtyManager(void);

	ActivityInfo* getActivityByType(int type);

public:
	static GiftActivtyManager* getManager();

	void initActivityInfos(vector<ActivityInfo> datas);


	enum ActivtyFilter
	{
		AF_All = 0,
		AF_CanGet,		// 可以领了
		AF_CanGetLater,	// 不可领，但还没领，将来是绝对可以领的，就是时间还没到的情况
		AF_TimeOver,	// 时间过了
		AF_Getted,		// 已经领过了
	};
	// 获取所有的奖励
	vector<ActivityInfo> getActivities(ActivtyFilter filter = AF_All);
	// 没找到就返回NULL
	const ActivityInfo* getActivity(int type);

	// 获取某个activity的状态
	// AF_All不包括在返回情况里面
	ActivtyFilter getActivityState(const ActivityInfo& infor);
	ActivtyFilter getActivityState(const ActivityInfo* infor);

	~GiftActivtyManager(void);
};

