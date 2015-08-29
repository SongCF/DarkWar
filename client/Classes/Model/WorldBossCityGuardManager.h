#pragma once
#include "protocol.h"
#include "flat.h"
#include <map>

using namespace std;

class CmdHelper;

struct CityGuardProcessNotify
{
	unsigned int startTime;
	unsigned int endTime;
	unsigned int start_hp;
	unsigned int end_hp;


	// 描述上面的数据是否可用，初始化了
	bool isDataOk; 
	// 记录当前post的索引，也就是notifyTaskQueue的key
	// 也就是刚post过了的key
	int currentPostIndex;
	// 触发post的触发器id
	unsigned int postTrigleId;

	//CityGuardProcessNotify()
	//{
	//	currentPostIndex = 0;
	//	postTrigleId = 0; // 必须初始化为0
	//}

};

// 客户端显示世界boss减血的事件
#define MSG_CLIENT_SHOW_BOSS_HP_SUB		"MSG_CLIENT_SHOW_BOSS_HP_SUB"
struct BossHpSub
{
	vector<CityGuardAnotherHeroShanghai> shangHaiVec;
};


// 集成CCObject，用于接收CCNotifycation的通知
class WorldBossCityGuardManager : public CCObject
{
	friend class CmdHelper;
private:
	bool inited;

	bool isLastDayReportOK;
	Flat_getCityGuardLastDayReportRsp lastDayReport;

	bool is_opening;
	unsigned int boss_blood_total;	// boss原始总血量
	unsigned int mOpenningTime;		// 开放时间点
	bool have_reward;

	// 战斗的数据
	unsigned int total_shanghai; //对boss的总伤害
	unsigned int shanghai_order; //当前伤害排名


	// CD时间结束的点的时间
	unsigned int mCdEndTime; 

	// 保存的系统战斗历史通知
	// manager主动通知外界，外界收到消息就播放就是了
	// 对于boss的血量，外界不断轮询就是了
	CityGuardProcessNotify notify;
	// 通知的队列
	// 每触发一次发送事件，就会取出一个发出去
	// 第i次触发，就调用key为i的消息发送出去
	map<int,vector<CityGuardAnotherHeroShanghai> >notifyTaskQueue;

	static WorldBossCityGuardManager* mManager;

	WorldBossCityGuardManager(void);

	unsigned int calculatePostTimeByNotifyData(const CityGuardProcessNotify& _notify);

	// trrigle回调
	void timePostShangHai(CCObject* tickObjectData);
	// 移除触发器
	void removeCurrentTriggle();

	void initState(cityGuardStatInfo _state,bool have_reward);
	void addCityGuardProcessNotify(Flat_CityGuardProcessNotify notify);
	void setHaveReward(bool have);

	void setCurrentTotalShanghaiAndOrder(unsigned int shanghai,unsigned int order);

	void setBossDead();

	// 设置上次的战报
	void setCityGuardLastDayReport(Flat_getCityGuardLastDayReportRsp data);
	void clearLastDayReport();
public:

	static WorldBossCityGuardManager* getManager();

	// 开始CD时间计时
	void startCDTimeCounter();

	// 当前是否有奖励
	bool haveReward();

	// 是个manager可用了，是否初始化过了
	bool isManagerOk();

	// 是否处于正在开打状态
	bool isOpening();
	// 获取boss的总血量
	unsigned int getBossTotalBlood();
	// 获取当前还需要等待多久才可以开打，已经开打的话这里就是0
	unsigned int getTimeNeedWaitForOpen();
	// 获取boss的当前血量(等待状态的话，返回值就是满血量)
	// 战斗状态这值就是实时更新的
	unsigned int getBossCurrentBlood();
	// 获取CD时间等待还有多久
	unsigned int getTimeNeedWaitForCD();

	// 已经打了多久了，没开战就是0
	unsigned int getAlreadyFightTime();
	// 我对boss的总伤害，打完了也会保留这个数据直到下次开始，就会清空
	unsigned int getMyTotalShangHai();
	// 获取我当前伤害的排名，打完了也会保留这个数据直到下次开始，就会清空
	unsigned int getMyCurrentShangHaiOrder();

	bool isLastCityGuardDayReportOK();
	// 获取战报
	Flat_getCityGuardLastDayReportRsp getLastCityGuardDayReport();

	~WorldBossCityGuardManager(void);
};

