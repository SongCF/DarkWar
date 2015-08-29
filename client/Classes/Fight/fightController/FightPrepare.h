#pragma once

#include "cocos2d.h"
#include "CommonDefine.h"
#include "FightDirector.h"
using namespace cocos2d;

class FightPrepareCallback
{
public:
	// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
	virtual void prepareError(int code) = 0;
	// 提供背景
	virtual CCSprite* getBg(int taskId,int gridIndex) = 0;
	virtual void prepareSuccess(){};
};

#define  Prepare_State_Loading	0		// 正在载入
#define  Prepare_State_Loaded	1		// 载入完毕
#define  Prepare_State_NULL		2		// 还是空的,默认

// 战斗的准备类
// 准备网络的数据，准备好了就到战斗界面
// 准备错误就回调。
class FightPrepare : public CCObject
{
private:
	FightPrepareCallback* mCallback;
	bool mTestMode;
	vector<FightDirector> mPreparedDir_vec;
	int mTaskId;
	int mGridIndex;
	int mState;
	// 战斗之前的数据
	HeroBaseProperty beforeFightPro[FormationHeroAcountMax];

	void battleCallBack(CCObject* msg_rsp);

public:
	// 准备战斗，需要指定taskId和gridIndex(0~8)
	// 反馈信息会在callback中回调
	FightPrepare(FightPrepareCallback* callback);
	// 载入数据
	void prepareData(int taskId,int gridIndex);
	void fight();
	void setTest(bool testMode);
	~FightPrepare(void);
};

