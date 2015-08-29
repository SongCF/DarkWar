#include "WorldBossCityGuardManager.h"
#include "GlobalTickTrigle.h"
#include "ServerTime.h"
#include "CS_Common.h"
#include "MsgDefine.h"

#define POST_GAP	1.0f // 每秒通知一次

// 触发器
#define MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE	"MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE"

WorldBossCityGuardManager* WorldBossCityGuardManager::mManager = NULL; 


class AutoDelete
{
private:
	char* data;
public:
	AutoDelete(char* data)
	{
		this->data = data;
	}
	~AutoDelete()
	{
		if (data != NULL)
		{
			delete data;
		}
	}
};

WorldBossCityGuardManager* WorldBossCityGuardManager::getManager()
{
	if (mManager == NULL)
	{
		mManager = new WorldBossCityGuardManager();
	}
	return mManager;
}


WorldBossCityGuardManager::WorldBossCityGuardManager(void)
{
	inited = false;
	is_opening = false;
	isLastDayReportOK = false;

	notify.isDataOk = false;
	notify.postTrigleId = 0;	// 必须初始化为0，不然都不知道当前是否有触发器
	notify.currentPostIndex = 0;
	notify.startTime = 0;
	notify.endTime = 0;
	notify.start_hp = 0;
	notify.end_hp = 0;

	total_shanghai = 0;
	shanghai_order = 0;

	mCdEndTime = ServerTime::getTime();
}

WorldBossCityGuardManager::~WorldBossCityGuardManager(void)
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE);
}

void WorldBossCityGuardManager::removeCurrentTriggle()
{
	CCNotificationCenter::sharedNotificationCenter()->removeObserver(this,MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE);

	//GlobalTickTrigle::getTrigle()->removeTrigleWithoutClearId(notify.postTrigleId);
	//notify.postTrigleId = 0; // 移除

	GlobalTickTrigle::getTrigle()->removeTrigle(notify.postTrigleId);
}

void WorldBossCityGuardManager::initState(cityGuardStatInfo _state,bool have_reward)
{
	// 从关闭到开启状态时，清空排名，伤害的数值，清空战报
	if (!is_opening && _state.is_opening)
	{
		total_shanghai = 0;
		shanghai_order = 0;
		clearLastDayReport();
	}

	this->have_reward = have_reward;
	this->mOpenningTime = _state.time_before_open + ServerTime::getTime();
	this->boss_blood_total = _state.boss_blood_total;
	this->is_opening = _state.is_opening;
	inited = true;
}

unsigned int WorldBossCityGuardManager::calculatePostTimeByNotifyData(const CityGuardProcessNotify& _notify)
{
	float _post_num = ((float)(_notify.endTime-_notify.startTime))/POST_GAP;
	int postNum = (int)(_post_num+0.5f); // 需要post的次数
	return postNum;
}

void WorldBossCityGuardManager::addCityGuardProcessNotify(Flat_CityGuardProcessNotify _notify)
{
	//CCAssert(inited,"");
	if (!inited)
	{
		// 扔掉
		return;
	}

	// 添加触发器
	if (this->notify.postTrigleId == 0)
	{
		OneTrigle t = OneTrigle::makeTrigleDelay(MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE,0,true);
		//t.loop = true;
		//t.time = 0; // 每帧都触发，回调函数里根据时间来发送消息
		//t.notifyMsg = MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE;
		//t.type = Trigle_Delay;
		this->notify.postTrigleId = GlobalTickTrigle::getTrigle()->addOneTrigle(t);
		CCNotificationCenter::sharedNotificationCenter()->addObserver(this,SEL_CallFuncO(&WorldBossCityGuardManager::timePostShangHai),MSG_CLIENT_SHOW_BOSS_HP_SUB_TRIGLE,NULL);
	}

	// 刷新,丢弃上次的数据，直接使用新数据
	notifyTaskQueue.clear();
	this->notify.start_hp = _notify.boss_hp_start;
	this->notify.end_hp = _notify.boss_hp_end;
	this->notify.startTime = ServerTime::getTime();
	this->notify.endTime = ServerTime::getTime() + _notify.play_time;
	this->notify.currentPostIndex = -1; // 设为-1，还没post过
	this->notify.isDataOk = true;// 数据可用了
	CCAssert(notify.start_hp >= notify.end_hp,"血量不可能反增");

	// 分布post伤害的数据
	int postNum = calculatePostTimeByNotifyData(notify);

	// 保存每一次post，post出多少个伤害个数
	int* shanghaiItemNumInEachPost = new int[postNum];
	AutoDelete temp((char*)shanghaiItemNumInEachPost); // 自动delete
	memset(shanghaiItemNumInEachPost,0,postNum*sizeof(int)); // 初始化为0

	// 1.先均匀散开,不够也无所谓，就把后面的空着就是了
	for (int i=0,max_count = _notify.shanghaiVec.size();i<postNum && i<max_count;i++)
	{
		shanghaiItemNumInEachPost[i] = 1;
	}
	// 2.随机添加多余的
	int shangHaiNum = _notify.shanghaiVec.size();
	for (int i=0,more_num = shangHaiNum-postNum; i<more_num; i++)
	{
		int index = CCRANDOM_0_1() * postNum; // 取值在0到postNum
		if (index >= postNum)
			index = postNum - 1;

		// 添加一个
		shanghaiItemNumInEachPost[index] += 1;
	}

	// 3.根据每个key需要post的数量，挨个取数据，初始化
	int num_used = 0;
	for (int i=0;i<postNum;i++)
	{
		// 取指定的个数
		vector<CityGuardAnotherHeroShanghai> shanghais;
		for (int num_index = 0;num_index < shanghaiItemNumInEachPost[i]; num_index++)
		{
			shanghais.push_back(_notify.shanghaiVec[num_used]);
			num_used ++;
		}
		notifyTaskQueue.insert(pair<int,vector<CityGuardAnotherHeroShanghai> >(i,shanghais));
	}
}

void WorldBossCityGuardManager::timePostShangHai(CCObject* tickObjectData)
{
	// 数据还不可用，返回
	if (!notify.isDataOk)
	{
		return;
	}

	int post_num = calculatePostTimeByNotifyData(notify);
	// 已经发完了
	if (notify.currentPostIndex >= post_num-1) 
	{
		return;
	}

	// 根据绝对时间确定当前应该发送的通知index
	int index = 0;
	unsigned int currentTime = ServerTime::getTime();
	if (currentTime > notify.endTime)
	{
		index = post_num -1;
	}
	else if (currentTime < notify.startTime)
	{
		index = 0;
	}
	else
	{
		float _index = ((float)(currentTime - notify.startTime))/((float)(notify.endTime-notify.startTime)) * (float)(post_num-1);
		index = _index;
	}
	// 已经是发送过的了，直接返回
	if (index <= notify.currentPostIndex)
	{
		return;
	}


	// 记录post的索引
	notify.currentPostIndex = index; 
	BossHpSub post;
	map<int,vector<CityGuardAnotherHeroShanghai> >::iterator it = notifyTaskQueue.find(index);
	if (it == notifyTaskQueue.end())
	{// 没找到就报错
		CCAssert(false,"不可能找不到");
		return;
	}

	// 设置要发送的数据
	post.shangHaiVec = it->second;
	if (post.shangHaiVec.empty())
	{// 没数据，直接返回
		CCLog("MSG_CLIENT_SHOW_BOSS_HP_SUB --> index = %d no data",notify.currentPostIndex);
		return;
	}

	// 通知
	CCLog("MSG_CLIENT_SHOW_BOSS_HP_SUB --> index = %d data num = %d",notify.currentPostIndex,post.shangHaiVec.size());
	CCNotificationCenter::sharedNotificationCenter()->postNotification(MSG_CLIENT_SHOW_BOSS_HP_SUB,MSG_Rsp::createMsg(&post));

	// 当前发送的数据已经是最后一个了，并且boss已经死了，那么就不是open了
	if (notify.currentPostIndex >= post_num-1 )
	{
		if (getBossCurrentBlood() <= 0)
		{
			is_opening = false;
			notify.isDataOk = false;	// 数据不可用了
			isLastDayReportOK = false;	// 清空上次的战报，外界需要重新查询一次战报
			removeCurrentTriggle();		// 移除触发器，不需要了
		}
	}

}

void WorldBossCityGuardManager::setCityGuardLastDayReport(Flat_getCityGuardLastDayReportRsp data)
{
	CCAssert(data.err_code == Success,"必须是正确的数据");

	isLastDayReportOK = true;
	lastDayReport = data;

	if (lastDayReport.have_reward)
	{
		setHaveReward(true); // 有奖励
	}
}

void WorldBossCityGuardManager::clearLastDayReport()
{
	isLastDayReportOK = false;
}

Flat_getCityGuardLastDayReportRsp WorldBossCityGuardManager::getLastCityGuardDayReport()
{
	CCAssert(isLastDayReportOK,"");
	return lastDayReport;
}

bool WorldBossCityGuardManager::isLastCityGuardDayReportOK()
{
	return isLastDayReportOK;
}

//
void WorldBossCityGuardManager::setHaveReward(bool have)
{
	CCAssert(inited,"");
	have_reward = have;
}

void WorldBossCityGuardManager::setCurrentTotalShanghaiAndOrder(unsigned int shanghai,unsigned int order)
{
	total_shanghai = shanghai;
	shanghai_order = order;
}

bool WorldBossCityGuardManager::haveReward()
{
	CCAssert(inited,"");
	return have_reward;
}

void WorldBossCityGuardManager::startCDTimeCounter()
{
	CCAssert(inited,"");

	mCdEndTime = ServerTime::getTime() + CS::getCityGuardBattleEndCDTime();
}

bool WorldBossCityGuardManager::isManagerOk()
{
	return inited;
}

bool WorldBossCityGuardManager::isOpening()
{
	CCAssert(inited,"");
	return is_opening;
}
unsigned int WorldBossCityGuardManager::getBossTotalBlood()
{
	CCAssert(inited,"");
	return boss_blood_total;
}
unsigned int WorldBossCityGuardManager::getTimeNeedWaitForOpen()
{
	CCAssert(inited,"");

	if (ServerTime::getTime() >= mOpenningTime)
	{
		return 0;
	}

	return mOpenningTime - ServerTime::getTime();
}
unsigned int WorldBossCityGuardManager::getBossCurrentBlood()
{
	CCAssert(inited,"");

	// 还没取回来，就返回满血
	if (!notify.isDataOk)
	{
		return boss_blood_total;
	}

	// 随时间均匀变化
	unsigned int currentTime = ServerTime::getTime();
	if (currentTime < notify.startTime)
	{
		return notify.start_hp;
	}
	if (currentTime > notify.endTime)
	{
		return notify.end_hp;
	}

	float blood = ((float)notify.start_hp) - ((float)(currentTime-notify.startTime)*(notify.start_hp-notify.end_hp))/(float)(notify.endTime-notify.startTime);
	/*CCLog("Boss_Blood : %f",blood);
	if (blood <= 0.001f)
	{
		int a = 0;
	}*/
	return blood;
}
unsigned int WorldBossCityGuardManager::getTimeNeedWaitForCD()
{
	CCAssert(inited,"");

	if (ServerTime::getTime() >= mCdEndTime)
	{
		return 0;
	}
	return mCdEndTime - ServerTime::getTime();
}

unsigned int WorldBossCityGuardManager::getAlreadyFightTime()
{
	if (!isOpening())
	{
		return 0;
	}

	return ServerTime::getTime() - mOpenningTime;
}
unsigned int WorldBossCityGuardManager::getMyTotalShangHai()
{
	return total_shanghai;
}
unsigned int WorldBossCityGuardManager::getMyCurrentShangHaiOrder()
{
	return shanghai_order;
}

void WorldBossCityGuardManager::setBossDead()
{
	// 直接把血量刷成0
	notify.start_hp = 0;
	notify.end_hp = 0;
}

