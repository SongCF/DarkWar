#pragma once

#include "BaseLayer.h"
#include "FightPrepare.h"

/**
 *	账号输入和登陆的界面
 */
class TestCmdLayer : public BaseLayer,FightPrepareCallback
{
private:

	CCPoint lastPos;
	FightPrepare* fight;

	void testClicked(CCObject* obj);
	void testCallBack(CCObject* msg_rsp);
	void battleCallBack(CCObject* msg_rsp);
	void jingJiBattleCallBack(CCObject* f_msg_rsp);
	void tuanduiBattleCallBack(CCObject* f_msg_rsp);
	void cityGuardBattleCallBack(CCObject* f_msg_rsp);
	void topHeroBattleCallBack(CCObject* f_msg_rsp);
	void topArenaTV_BattleCallback(CCObject* f_msg_rsp);

	void back(CCObject* obj);

	void lfTouchesBegan(CCPoint glPoint);
	void lfTouchesMoved(CCPoint glPoint);

	// 失败，返回一个Code，就是协议定义的SimpleProtocolErrorCode
	virtual void prepareError(int code);
	// 提供背景
	virtual CCSprite* getBg(int taskId,int gridIndex);
	virtual void prepareSuccess();
public:
	TestCmdLayer(void);
	~TestCmdLayer(void);
};

