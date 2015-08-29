//@ fight end layer
//@ this layer pushes to running scene when fight end;
//@ by _Richard


#pragma once

#include "cocos2d.h"
#include "protocol.h"
#include "CommonDefine.h"
#include "IMainLayerBody.h"
#include "BaseLayer.h"
#include "FightBagLayer.h"
#include "LevelUpLayer.h"
#include "flat.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

#define  TAG_FIGHT_END_LAYER	4643+20

USING_NS_CC;

typedef struct  TaskSettlement
{
	string taskName;	//副本/任务名
	string bossName;	//副本boss关卡才有


	unsigned int win;		//通关评级  0表示输，1通关, -1步数太长失败

	unsigned int exp;		//经验
	
	HeroBaseProperty oldArmy[4];	//参战人物 
	HeroBaseProperty curArmy[4];    
	
	vector<ItemInfo> items;	//银币 和 其他的所有物品
	vector<EquipData> equips;

////////////////////////////////////////////////////////////////////////	
	TaskSettlement()
	{
		win = 0; 
		exp = 0;
	}

	TaskSettlement(TaskSettlement* task)
	{
		taskName = task->taskName;
		bossName = task->bossName;
		win      = task->win;
		exp      = task->exp;
		items	 = task->items;
		equips	 = task->equips;
		for (int i=0;i<4;i++)
		{
			oldArmy[i] = task->oldArmy[i];
			curArmy[i] = task->curArmy[i];
		}
	}

}Settlement;


// 任务结算 层          //把改层添加到战斗层之上
class FightSettlementLayer : public BaseLayer, public IMainLayerBody, public BagCallBack, public LevelUpEndCallBackInterface, public CCTableViewDataSource
{
public:
	/*
	*1. 输0/赢1   2.结算信息
	**自动释放
	*/
	static FightSettlementLayer* create(Settlement* task_Info);
	bool init(Settlement* taskInfo);
	
	FightSettlementLayer()
	{ 
		mTaskInfo = NULL;
		mQueryTimes = 0;
		mCurQueryTiems = 0;
	};  //若用constructor， 必须用init初始化， 并手动调用autorelease 或 手动释放 
	~FightSettlementLayer()
	{ 
		removeObserver(MSG_QueryFubenDiaoluoRsp_F);
		CC_SAFE_DELETE(mTaskInfo);
	};


//	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);

protected:

	//关卡信息：经验、银币、头像
	void displayInfomation_Ordinary(float dt);
	void displayInfomation_Boss();
	void displayBossInfoDelaySchedule(float dt);//win

	void addArmy();  //add army to bg
	void addMenu();

	void menuItemClicked_HeroImg(CCObject *pSender);
	void menuItemClicked_MainMenu(CCObject *pSender);
	void menuItemClicked_HuanZhuang(CCObject *pSender);

	void armyLevelUp();
	void teachGuide();


	void callback_BeginFightNormal(CCObject* obj);
	void callback_BeginFightHard(CCObject *msg_f);
//	void callbackRemove(CCNode* pSender);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){}
	virtual void onEnter();
	virtual void onExit();

	/////////////////////////////////////////////// BagCallBack
	virtual void disableAllTouch();
	virtual void enableAllTouch();
	//////////////////////////////////////////////// LevelUpEndCallBackInterface
	virtual void LevelUpEndCallBackFunc();
	//////////////////////////////////////////////////////


	void QueryFubenDiaoluo_rsp(CCObject* pMsg);

	/////////////////////////////////////////////////////////////////////////////
	// CCScrollViewDelegate
	virtual void scrollViewDidScroll(CCScrollView* view){};
	virtual void scrollViewDidZoom(CCScrollView* view){};
	// CCTableViewDelegate
	virtual void tableCellTouched(CCTableView* table, CCTableViewCell* cell){};
	virtual void tableCellHighlight(CCTableView* table, CCTableViewCell* cell){};
	virtual void tableCellUnhighlight(CCTableView* table, CCTableViewCell* cell){};
	virtual void tableCellWillRecycle(CCTableView* table, CCTableViewCell* cell){};
	// CCTableViewDataSource
	virtual CCSize tableCellSizeForIndex(CCTableView *table, unsigned int idx);
	virtual CCTableViewCell* tableCellAtIndex(CCTableView *table, unsigned int idx);
	virtual unsigned int numberOfCellsInTableView(CCTableView *table);
	///////////////////////////////////////////////////////////////////////////////

private:
	string getNewFirendName(unsigned int id);
	void playNewFriendAdd();
	bool isNeedExp();
	void QueryEquipFall_rsp(CCObject* pMsg);
	void showBoosFailedUI();

	void addFriendCallBack(CCNode *node);
private:
	CCSize mWinSize;

	Settlement *mTaskInfo;

	CCSprite *mBg;
	CCMenu *mMenu;

	Flat_FubenDiaoluoZBInfo mCur_ZBinfo;

	vector<Equipment>	mBeibaoWupins;
	vector<WuPin>		mWupins;

	bool mTouched;

	bool mFightAgain;

	int mCurTaskGridNum;
	
	int			mQueryTimes;
	int         mCurQueryTiems;


	bool		m_bIsFinish;
	bool		m_bLevelUp;

};