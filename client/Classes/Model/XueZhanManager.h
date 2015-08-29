#pragma once
#include <stdlib.h>
#include <string>
#include <vector>
#include "BaseSprite.h"
#include "protocol.h"

using namespace cocos2d;
using namespace std;

enum XueZhanState
{
	State_Out = 0,	// 没进去
	State_In,		// 在里面了
};

enum Difficult
{
	Difficult_PuTong	= 0,	// 普通
	Difficult_KunNan	= 1,	// 困难
	Difficult_DiYu		= 2,	// 地狱
	Difficult_EMeng		= 3,	// 恶魔
};

struct XueZhanData
{
	unsigned int currentChlgNumLeft;
	unsigned int currentPassedNum;
	unsigned int currentPowerNum;

	XueZhanState currentState;

	unsigned int open_time;			//开启时间
	unsigned int close_time;		//关闭时间

	Difficult difficult;	// 难度

	unsigned int currentHpAddPercent;
	unsigned int currentGongJiAddPercent;
	unsigned int currentHuJiaAddPercent;

	XueZhanData()
	{
		memset(this,0,sizeof(XueZhanData));
		currentState = State_Out;
	}
};

/**
 * 血战的服务和管理的类
 */
class XueZhanManager
{
private:

	XueZhanData xuezhanData;
	unsigned int lastGettedSilverNum;
	GiftBag _giftBag;
	bool mCanOpenBox;
	bool isDiffFixed; // 难度定下来了没

private:
	static XueZhanManager* mInstance;
	XueZhanManager(void);
public:
	static XueZhanManager* getManager();
	SelfChlgDiffType convertToSelfChlgDiffType(Difficult diff);
	Difficult convertToDifficult(SelfChlgDiffType diff);

	// UI必须在第一次点击了血战的时候调用这个接口
	// 当然每次都调用也无所谓
	void enter();

	// 刷新时间
	void refreshData(SPCmd_QuerySelfChallengeInfoRsp data);
	// 添加通关的个数
	void addPassedNum(unsigned int toAdd);
	// 添加能量点
	void addPowerNum(unsigned int toAdd);
	// 设置能量点
	void setPowerNum(unsigned int num);
	// 减少能量点
	void subPowerNum(unsigned int toSub);
	// 减少可挑战的次数，每日只有3次机会
	void subCanChlgNum(unsigned int toSub);
	// 设置可挑战次数
	void setCanChlgNum(unsigned int num);

	// 设置当前的状态
	void setCurrentState(XueZhanState state);
	// 通过的关数
	void setCurrentPassedNum(unsigned int num);
	// 刷新成一个新的挑战
	// 会减去一次挑战次数，并置状态为State_Out
	void makeNewChlgWhenFaild();


	// 设置加成的数据
	void setCurrentHpAddPercent(unsigned int percent);
	void setCurrentGongJiAddPercent(unsigned int percent);
	void setCurrentHuJiaAddPercent(unsigned int percent);
	// 添加数据的加成
	void addCurrentHpAddPercent(int percentToAdd);
	void addCurrentGongJiAddPercent(int percentToAdd);
	void addCurrentHuJiaAddPercent(int percentToAdd);

	// 设置可以开宝箱了
	void setCanOpenBox(bool canOpen);
	// 设置最近一次获取到的银币的数量
	// 每20关都会有一次  每20关也会有一次礼包
	void setLastGettedSilverNum(unsigned int silverNum);
	void setLastGettedGiftBag(GiftBag& gift);
	GiftBag getLastGettedGiftBag();


	enum ErrorCode
	{
		EC_OK = 0,
		EC_Error,
		EC_LevelNotEnough,
		EC_TaskNotEnough,
		EC_NotOpenThisTime,
		EC_NoChlgTime,
	};
	// 返回EC_OK就可以
	ErrorCode canIJoinXueZhan();

	// 获取通关的个数
	// 默认显示为0关，玩家每通过1关（即战斗胜利），关数+1
	unsigned int getPassedNum();

	// 获取即将获得的能量点，就是在继续闯关界面用的
	// 第一次打就是0，其他的时候就是60
	unsigned int powerWillAddIfChlg();
	// 获取通关当前关卡会获得的能量，15点
	unsigned int getWillGetPowerAfterWin();

	// 获取当前能量点的个数
	unsigned int getCurrentPowerNum();

	// 获取还能挑战的次数
	unsigned int getCanChlgNum();
	// 获取默认的挑战次数，就是每天默认几次。3次
	unsigned int getDefaultChlgNum();

	// 获取当前难度系数
	Difficult getCurrentDifficult();

	void setCurrentDiffFixed(bool fixed);

	XueZhanState getCurrentState();

	// 难度和等级的映射关系
	struct Diff_Level_Map
	{
		Difficult diff;
		unsigned int min_level;
		unsigned int max_level;
	};
	// 获取所有的难度等级映射
	// 用于难度描述
	// 顺序保证是[0]->普通，递增
	// 完全可以用Difficult作为下标获取
	vector<Diff_Level_Map> getAllDiff_Level_Map();

	// 创建一个当前需要的背景
	BaseSprite* createCurrentBackgroud();
	// 获取当前背景，场景的描述
	string getCurrentBackgroundDescri();


	// 获取当前的血量/攻击/护甲的百分比加成
	unsigned int getCurrentHpAddPercent();
	unsigned int getCurrentGongJiAddPercent();
	unsigned int getCurrentHuJiaAddPercent();

	// 是否可以开宝箱了
	bool canOpenBox();
	// 是否需要显示银币奖励，在开启了宝箱之后
	bool canShowRewards();
	void setRewardShowed();
	// 获取到的银币数量
	unsigned int getGettedSilverNum();
	// 获得到的礼包数量
	const GiftBag* getGettedGiftBag();

	~XueZhanManager(void);
};

